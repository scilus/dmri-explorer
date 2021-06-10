#include "model.h"
#include <utils.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <spherical_harmonic.h>

namespace
{
const size_t NB_SH = 45;
}

namespace Engine
{
namespace Scene
{
Model::Model(std::shared_ptr<Image::NiftiImageWrapper> image, uint sphereRes)
    :mImage(image)
    ,mGridDims(image->dims())
    ,mIndices()
    ,mAllScaledSpheres()
    ,mAllNormals()
    ,mInstanceTransforms()
    ,mSphHarmCoeffs()
    ,mSphHarmFuncs()
    ,mSphere(sphereRes)
    ,mSphereInfo()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mIndirectBO(0)
    ,mInstanceTransformsData()
    ,mSphHarmCoeffsData()
    ,mSphHarmFuncsData()
    ,mSphereVerticesData()
    ,mSphereNormalsData()
    ,mSphereIndicesData()
    ,mSphereInfoData()
    ,mAllSpheresVerticesData()
    ,mAllSpheresNormalsData()
    ,mIndirectCmd()
{
    // Generate primitives
    initializeArrays();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    // Bind uniform buffers to GPU
    mAllSpheresVerticesData = GPUData::ShaderData(mAllScaledSpheres.data(),
                                                  GPUData::BindableProperty::allScaledSpheres,
                                                  sizeof(glm::vec4) * mAllScaledSpheres.size(),
                                                  GL_DYNAMIC_DRAW);
    mAllSpheresNormalsData = GPUData::ShaderData(mAllNormals.data(),
                                                 GPUData::BindableProperty::allNormals,
                                                 sizeof(glm::vec4) * mAllNormals.size(),
                                                 GL_DYNAMIC_DRAW);
    mInstanceTransformsData = GPUData::ShaderData(mInstanceTransforms.data(),
                                                  GPUData::BindableProperty::modelTransform,
                                                  sizeof(glm::mat4) * mInstanceTransforms.size());
    mSphHarmCoeffsData = GPUData::ShaderData(mSphHarmCoeffs.data(),
                                             GPUData::BindableProperty::shCoeffs,
                                             sizeof(float)* mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPUData::ShaderData(mSphere.getSHFuncs().data(),
                                            GPUData::BindableProperty::shFunctions,
                                            sizeof(float) * mSphere.getSHFuncs().size());
    mSphereVerticesData = GPUData::ShaderData(mSphere.getPoints().data(),
                                              GPUData::BindableProperty::sphereVertices,
                                              sizeof(glm::vec4) * mSphere.getNbVertices());
    mSphereNormalsData = GPUData::ShaderData(mSphere.getPoints().data(),
                                             GPUData::BindableProperty::sphereNormals,
                                             sizeof(glm::vec4) * mSphere.getNbVertices());
    mSphereIndicesData = GPUData::ShaderData(mSphere.getIndices().data(),
                                             GPUData::BindableProperty::sphereIndices,
                                             sizeof(uint) * mSphere.getIndices().size());
    mSphereInfoData = GPUData::ShaderData(&mSphereInfo,
                                          GPUData::BindableProperty::sphereInfo,
                                          sizeof(GPUData::SphereInfo));
}

Model::~Model()
{
}

void Model::initializeArrays()
{
    const glm::vec3 gridCenter((mGridDims.x - 1) / 2.0f,
                               (mGridDims.y - 1) / 2.0f,
                               (mGridDims.z - 1) / 2.0f);

    const uint nVox = mGridDims.x * mGridDims.y * mGridDims.z;

    mSphereInfo.numVertices = mSphere.getPoints().size();
    mSphereInfo.numIndices = mSphere.getIndices().size();

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f),
                                        glm::vec3(-gridCenter.x,
                                                  -gridCenter.y,
                                                  -gridCenter.z));

    // zero-initialized arrays (will be filled in compute shader call)
    mAllScaledSpheres.resize(nVox * mSphereInfo.numVertices,
                             glm::vec4(0.0, 0.0, 0.0, 0.0));
    mAllNormals.resize(nVox * mSphereInfo.numVertices,
                       glm::vec4(0.0, 0.0, 0.0, 0.0));

    mIndices.reserve(nVox * mSphereInfo.numIndices);
    for(uint flatIndex = 0; flatIndex < nVox; ++flatIndex)
    {
        glm::vec<3, uint> indice3D = mImage->unravelIndex3d(flatIndex);
        // Fill SH coefficients table
        for(int k = 0; k < NB_SH; ++k)
        {
            mSphHarmCoeffs.push_back(
                static_cast<float>(
                    mImage->at(indice3D.x, indice3D.y, indice3D.z, k)));
        }

        // Add sphere faces
        for(const uint& i: mSphere.getIndices())
        {
            mIndices.push_back(i);
        }

        // Add transform associated to current sphere (grid position)
        mInstanceTransforms.push_back(glm::translate(modelMat,
            glm::vec3(indice3D.x, indice3D.y, indice3D.z)));

        // Add indirect draw command for current sphere
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(
                mSphereInfo.numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                0, // offset in VBO
                mIndirectCmd.size() * mSphereInfo.numVertices, // offset in element buffer array
                0 // mIndirectCmd.size()
            )
        );
    }
}

template <typename T>
GLuint Model::genVBO(const std::vector<T>& data) const
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void Model::addToVAO(const GLuint& vbo, const GPUData::BindableProperty& binding)
{
    GLuint type, size, count;
    switch(binding)
    {
    default:
        throw std::runtime_error("Invalid binding.");
    }

    const GLuint bindingLocation = static_cast<GLuint>(binding);
    glEnableVertexArrayAttrib(mVAO, bindingLocation);
    glVertexArrayAttribFormat(mVAO, bindingLocation, count, type, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, bindingLocation, vbo, 0, size);
    glVertexArrayBindingDivisor(mVAO, bindingLocation, 0);
    glVertexArrayAttribBinding(mVAO, bindingLocation, bindingLocation);
}

void Model::SendShaderDataToGPU()
{
    mInstanceTransformsData.ToGPU();
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mSphereVerticesData.ToGPU();
    mSphereNormalsData.ToGPU();
    mSphereIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
    mAllSpheresVerticesData.ToGPU();
    mAllSpheresNormalsData.ToGPU();
}

void Model::Draw()
{
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES,
                                GL_UNSIGNED_INT,
                                (GLvoid*)0,
                                mIndirectCmd.size(),
                                0);
}
} // namespace Scene
} // namespace Engine