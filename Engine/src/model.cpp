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
    ,mInstanceTransforms()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mIndirectBO(0)
    ,mInstanceTransformsData()
    ,mSphHarmCoeffs()
    ,mSphHarmFuncs()
    ,mSphHarmFuncsData()
    ,mSphere(sphereRes)
    ,mNbVertices(mSphere.getPositions().size())
{
    // Generate primitives
    genPrimitives();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mNeighboursBO = genVBO<glm::uvec2>(mSphere.getNeighboursID());
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
    mPositionsBO = genVBO<glm::vec3>(mPositions);

    addToVAO(mPositionsBO, GPUData::BindableProperty::position);
    addToVAO(mNeighboursBO, GPUData::BindableProperty::neighbours);

    // Bind uniform buffers to GPU
    mInstanceTransformsData = genShaderData<glm::mat4*>(mInstanceTransforms.data(),
                                                        GPUData::BindableProperty::model,
                                                        sizeof(glm::mat4) * mInstanceTransforms.size(),
                                                        true);
    mSphHarmCoeffsData = genShaderData<float*>(mSphHarmCoeffs.data(),
                                               GPUData::BindableProperty::sphHarmCoeffs,
                                               sizeof(float)* mSphHarmCoeffs.size(),
                                               true);
    mSphHarmFuncsData = genShaderData<float*>(mSphere.getSHFuncs().data(),
                                              GPUData::BindableProperty::sphHarmFunc,
                                              sizeof(float) * mSphere.getSHFuncs().size(),
                                              true);
    mNbVerticesData = genShaderData<uint*>(&mNbVertices,
                                           GPUData::BindableProperty::nbVertices,
                                           sizeof(uint),
                                           true);
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

    mPositions.reserve(nVox * mSphere.getPositions().size());
    mIndices.reserve(nVox * mSphere.getIndices().size());
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

        // Add sphere vertices
        mPositions.insert(mPositions.end(),
                          mSphere.getPositions().begin(),
                          mSphere.getPositions().end());

        // Add sphere faces
        mIndices.insert(mIndices.end(),
                        mSphere.getIndices().begin(),
                        mSphere.getIndices().end());

        // Add transform associated to current sphere (grid position)
        mInstanceTransforms.push_back(glm::translate(modelMat,
            glm::vec3(indice3D.x, indice3D.y, indice3D.z)));

        // Add indirect draw command for current sphere
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(
                mSphere.getIndices().size(), // num of elements to draw per drawID
                1, // number of identical instances
                mIndirectCmd.size() * mSphere.getIndices().size(),
                mIndirectCmd.size() * mSphere.getPositions().size(),
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

template <typename T>
GPUData::ShaderData<T> Model::genShaderData(const T& data,
                                            const GPUData::BindableProperty& binding) const
{
    return GPUData::ShaderData<T>(data, binding);
}

template <typename T>
GPUData::ShaderData<T> Model::genShaderData(const T& data,
                                            const GPUData::BindableProperty& binding,
                                            size_t sizeofT, bool isPtr) const
{
    return GPUData::ShaderData<T>(data, binding, sizeofT, isPtr);
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
    case GPUData::BindableProperty::neighbours:
        type = GL_UNSIGNED_INT;
        size = sizeof(uint) * 2;
        count = 2;
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

void Model::Draw()
{
    mInstanceTransformsData.ToGPU();
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mNbVerticesData.ToGPU();

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}
} // namespace Scene
} // namespace Engine