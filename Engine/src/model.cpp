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
    ,mIndirectCmd()
    ,mIndices()
    ,mCoordinates()
    ,mInstanceTransforms()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mIndirectBO(0)
    ,mInstanceTransformsData()
    ,mSphHarmCoeffs()
    ,mSphHarmFuncs()
    ,mSphHarmFuncsData()
    ,mSphere(sphereRes)
    ,mSphereInfo()
    ,mSphereInfoData()
{
    // Generate primitives
    genPrimitives();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    // Bind uniform buffers to GPU
    mInstanceTransformsData = GPUData::ShaderData(mInstanceTransforms.data(),
                                                  GPUData::BindableProperty::model,
                                                  sizeof(glm::mat4) * mInstanceTransforms.size());
    mSphHarmCoeffsData = GPUData::ShaderData(mSphHarmCoeffs.data(),
                                             GPUData::BindableProperty::sphHarmCoeffs,
                                             sizeof(float)* mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPUData::ShaderData(mSphere.getSHFuncs().data(),
                                            GPUData::BindableProperty::sphHarmFunc,
                                            sizeof(float) * mSphere.getSHFuncs().size());
    mCoordinatesData = GPUData::ShaderData(mCoordinates.data(),
                                           GPUData::BindableProperty::vertices,
                                           sizeof(float)  * mCoordinates.size(),
                                           GL_DYNAMIC_DRAW);
    mNormalsData = GPUData::ShaderData(mNormals.data(),
                                       GPUData::BindableProperty::normals,
                                       sizeof(float)  * mNormals.size(),
                                       GL_DYNAMIC_DRAW);
    mIndicesData = GPUData::ShaderData(mIndices.data(),
                                       GPUData::BindableProperty::indices,
                                       sizeof(uint)  * mIndices.size());
    mSphereInfoData = GPUData::ShaderData(&mSphereInfo,
                                          GPUData::BindableProperty::sphereInfo,
                                          sizeof(GPUData::SphereInfo));
}

Model::~Model()
{
}

void Model::genPrimitives()
{
    const glm::vec3 gridCenter((mGridDims.x - 1) / 2.0f,
                               (mGridDims.y - 1) / 2.0f,
                               (mGridDims.z - 1) / 2.0f);

    const uint nVox = mGridDims.x * mGridDims.y * mGridDims.z;
    const size_t numVertices = mSphere.getCoordinates().size();
    const size_t numIndices = mSphere.getIndices().size();
    mSphereInfo.numVertices = numVertices;
    mSphereInfo.numIndices = numIndices;

    mCoordinates.reserve(nVox * numVertices);
    mNormals.reserve(nVox * numIndices);
    mIndices.reserve(nVox * numIndices);
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f),
        glm::vec3(-gridCenter.x, -gridCenter.y, -gridCenter.z));

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

        // Add sphere vertices and normals
        for(const glm::vec3& p: mSphere.getPoints())
        {
            mCoordinates.push_back(p.x);
            mCoordinates.push_back(p.y);
            mCoordinates.push_back(p.z);

            // normals correspond to vertices on a unit sphere
            mNormals.push_back(p.x);
            mNormals.push_back(p.y);
            mNormals.push_back(p.z);
        }

        // Add sphere faces
        // vector.insert is linear in complexity,
        // therefore equivalent to:
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
                numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                mIndirectCmd.size() * numIndices, // offset in element buffer array
                0, // offset in VBO
                mIndirectCmd.size()
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
    case GPUData::BindableProperty::position:
        type = GL_FLOAT;
        size = sizeof(float) * 3;
        count = 3;
        break;
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
    mCoordinatesData.ToGPU();
    mNormalsData.ToGPU();
    mIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
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