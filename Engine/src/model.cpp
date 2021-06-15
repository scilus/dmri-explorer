#include "model.h"
#include <utils.hpp>
#include <glad/glad.h>

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <spherical_harmonic.h>

#include <timer.h>

namespace
{
const size_t NB_SH = 45;
}

namespace Engine
{
namespace Scene
{
Model::Model(std::shared_ptr<Image::NiftiImageWrapper> image,
             const Scene::ShaderProgram& computeShader,
             uint sphereRes)
    :mImage(image)
    ,mIndices()
    ,mAllSpheresVertices()
    ,mAllSpheresNormals()
    ,mInstanceTransforms()
    ,mSphHarmCoeffs()
    ,mSphHarmFuncs()
    ,mSphere(sphereRes)
    ,mNbSpheres(0)
    ,mSphereInfo()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mIndirectBO(0)
    ,mComputeShader(computeShader)
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
    ,mGridInfo()
    ,mSliceIsDirty(true)
{
    initializeArrays();
    initializeGPUData();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
}

Model::~Model()
{
}

void Model::initializeArrays()
{
    // offset to substract from model to center image on (0, 0, 0)
    const glm::vec3 gridCenter((mImage->dims().x - 1) / 2.0f,
                               (mImage->dims().y - 1) / 2.0f,
                               (mImage->dims().z - 1) / 2.0f);

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), -gridCenter);

    mSphereInfo.numVertices = mSphere.getPoints().size();
    mSphereInfo.numIndices = mSphere.getIndices().size();

    mGridInfo.gridDims = mImage->dims();
    mGridInfo.sliceIndex = mGridInfo.gridDims / 2;

    mNbSpheres = mImage->dims().x * mImage->dims().y  // Z-slice
               + mImage->dims().x * mImage->dims().z  // Y-slice
               + mImage->dims().y * mImage->dims().z; // X-slice

    mSphHarmCoeffs.reserve(mImage->length());
    mInstanceTransforms.reserve(mImage->nbVox());
    mIndirectCmd.reserve(mImage->nbVox());

    // Fill SH coefficients and model matrix
    for(uint flatIndex = 0; flatIndex < mImage->nbVox(); ++flatIndex)
    {
        glm::vec<3, uint> id3D = mImage->unravelIndex3d(flatIndex);
        //logVec3(id3D, "Indice 3D");

        // Fill SH coefficients table
        for(int k = 0; k < NB_SH; ++k)
        {
            mSphHarmCoeffs.push_back(static_cast<float>(mImage->at(id3D.x, id3D.y, id3D.z, k)));
        }

        // Add transform associated to current grid position
        mInstanceTransforms.push_back(glm::translate(modelMat, glm::vec3(id3D)));
    }

    // zero-initialized arrays (will be filled in compute shader call)
    mAllSpheresVertices.resize(mNbSpheres * mSphereInfo.numVertices);
    mAllSpheresNormals.resize(mNbSpheres * mSphereInfo.numVertices);
    mIndices.reserve(mNbSpheres * mSphereInfo.numIndices);

    // Prepare draw command
    for(uint i = 0; i < mNbSpheres; ++i)
    {
        // Add sphere faces
        for(const uint& i: mSphere.getIndices())
        {
            mIndices.push_back(i);
        }

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

void Model::initializeGPUData()
{
    mAllSpheresVerticesData = GPUData::ShaderData(mAllSpheresVertices.data(),
                                                  GPUData::BindableProperty::allSpheresVertices,
                                                  sizeof(glm::vec4) * mAllSpheresVertices.size(),
                                                  GL_DYNAMIC_DRAW);
    mAllSpheresNormalsData = GPUData::ShaderData(mAllSpheresNormals.data(),
                                                 GPUData::BindableProperty::allSpheresNormals,
                                                 sizeof(glm::vec4) * mAllSpheresNormals.size(),
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
    mGridInfoData = GPUData::ShaderData(&mGridInfo,
                                        GPUData::BindableProperty::gridInfo,
                                        sizeof(GPUData::GridInfo));
    // push all data to GPU
    mInstanceTransformsData.ToGPU();
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mSphereVerticesData.ToGPU();
    mSphereNormalsData.ToGPU();
    mSphereIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
    mAllSpheresVerticesData.ToGPU();
    mAllSpheresNormalsData.ToGPU();
    mGridInfoData.ToGPU();
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

glm::ivec4 Model::GetSliceIndex() const
{
    return mGridInfo.sliceIndex;
}

void Model::SetSliceIndex(int i, int j, int k)
{
    const glm::ivec4 newSliceIndex = glm::ivec4(i, j, k, 0);
    if(newSliceIndex.x != mGridInfo.sliceIndex.x
    || newSliceIndex.y != mGridInfo.sliceIndex.y
    || newSliceIndex.z != mGridInfo.sliceIndex.z)
    {
        Utilities::Timer timer("Slice index");
        timer.Start();
        mGridInfo.sliceIndex = glm::ivec4(i, j, k, 0);
        mGridInfoData.ModifySubData(sizeof(glm::ivec4),
                                    sizeof(glm::ivec4),
                                    &mGridInfo.sliceIndex);
        mGridInfoData.ToGPU();
        mSliceIsDirty = true;
        timer.Stop();
    }
}

glm::ivec4 Model::GetGridDims() const
{
    return mGridInfo.gridDims;
}

void Model::ScaleSpheres()
{
    if(!mSliceIsDirty)
    {
        return;
    }
    Utilities::Timer timer("Scaling");
    timer.Start();
    glUseProgram(mComputeShader.ID());
    glDispatchCompute(mNbSpheres, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
    mSliceIsDirty = false;
    timer.Stop();
}

void Model::Draw()
{
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}
} // namespace Scene
} // namespace Engine