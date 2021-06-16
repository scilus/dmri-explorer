#include "model.h"
#include <utils.hpp>
#include <glad/glad.h>

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <thread>
#include <glm/gtx/transform.hpp>
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
    ,mRotationSpeed(0.01f)
    ,mScalingSpeed(1.1f)
    ,mTranslationSpeed(0.1f)
    ,mIndices()
    ,mAllSpheresVertices()
    ,mAllSpheresNormals()
    ,mInstanceTransforms()
    ,mModelMatrix()
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
    ,mModelMatrixData()
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
{
    initializeMembers();
    initializeGPUData();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
}

Model::~Model()
{
}

void Model::initializeMembers()
{
    mSphereInfo.numVertices = mSphere.getPoints().size();
    mSphereInfo.numIndices = mSphere.getIndices().size();
    mSphereInfo.isNormalized = 0;
    mSphereInfo.sh0Threshold = 0.0f;

    mGridInfo.gridDims = mImage->dims();
    mGridInfo.sliceIndex = mGridInfo.gridDims / 2;
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0);

    mNbSpheres = mImage->dims().x * mImage->dims().y  // Z-slice
               + mImage->dims().x * mImage->dims().z  // Y-slice
               + mImage->dims().y * mImage->dims().z; // X-slice

    mModelMatrix = glm::identity<glm::mat4>();

    std::thread initVoxThread(&Model::initializePerVoxelAttributes, this);
    std::thread initSpheresThread(&Model::initializePerSphereAttributes, this);
    initVoxThread.join();
    initSpheresThread.join();
}

void Model::initializePerVoxelAttributes()
{
    // offset to substract from model to center image on (0, 0, 0)
    const glm::vec3 gridCenter((mImage->dims().x - 1) / 2.0f,
                               (mImage->dims().y - 1) / 2.0f,
                               (mImage->dims().z - 1) / 2.0f);

    glm::mat4 instanceMat = glm::translate(-gridCenter);

    Utilities::Timer voxelLoopTimer("Foreach voxel");
    voxelLoopTimer.Start();

    // safety when allocating shared memory
    mMutex.lock();
    mSphHarmCoeffs.reserve(mImage->length());
    mInstanceTransforms.reserve(mImage->nbVox());
    mMutex.unlock();

    // Fill SH coefficients and model matrix
    for(uint flatIndex = 0; flatIndex < mImage->nbVox(); ++flatIndex)
    {
        glm::vec<3, uint> id3D = mImage->unravelIndex3d(flatIndex);

        // Fill SH coefficients table
        for(int k = 0; k < NB_SH; ++k)
        {
            mSphHarmCoeffs.push_back(static_cast<float>(mImage->at(id3D.x, id3D.y, id3D.z, k)));
        }

        // Add transform associated to current grid position
        mInstanceTransforms.push_back(glm::translate(instanceMat, glm::vec3(id3D)));
    }
    voxelLoopTimer.Stop();
}

void Model::initializePerSphereAttributes()
{
    Utilities::Timer sphereLoopTimer("Foreach sphere");
    sphereLoopTimer.Start();

    // safety when allocating shared memory
    mMutex.lock();
    // zero-initialized arrays (will be filled in compute shader call)
    mAllSpheresVertices.resize(mNbSpheres * mSphereInfo.numVertices);
    mAllSpheresNormals.resize(mNbSpheres * mSphereInfo.numVertices);
    mIndices.reserve(mNbSpheres * mSphereInfo.numIndices);
    mIndirectCmd.reserve(mImage->nbVox());
    mMutex.unlock();

    // Prepare draw command
    for(uint i = 0; i < mNbSpheres; ++i)
    {
        // Add sphere faces
        for(const uint& idx: mSphere.getIndices())
        {
            mIndices.push_back(idx);
        }

        // Add indirect draw command for current sphere
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(
                mSphereInfo.numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                0, // offset in VBO
                mIndirectCmd.size() * mSphereInfo.numVertices, // offset in element buffer array
                0));
    }
    sphereLoopTimer.Stop();
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
                                                  GPUData::BindableProperty::instanceTransform,
                                                  sizeof(glm::mat4) * mInstanceTransforms.size());
    mModelMatrixData = GPUData::ShaderData(&mModelMatrix,
                                           GPUData::BindableProperty::modelTransform,
                                           sizeof(glm::mat4));
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
    mModelMatrixData.ToGPU();
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

glm::ivec4 Model::GetSliceIndex() const
{
    return mGridInfo.sliceIndex;
}

void Model::SetSliceIndex(int i, int j, int k)
{
    const glm::ivec4 sliceIndex(i, j, k, 0);
    const glm::ivec4 isDirtySlice((int)(sliceIndex.x != mGridInfo.sliceIndex.x),
                                  (int)(sliceIndex.y != mGridInfo.sliceIndex.y),
                                  (int)(sliceIndex.z != mGridInfo.sliceIndex.z),
                                  0);
    mGridInfo.sliceIndex = sliceIndex;
    mGridInfo.isSliceDirty = isDirtySlice;
    mGridInfoData.ModifySubData(0, sizeof(GPUData::GridInfo), &mGridInfo);
}

bool Model::isAnySliceDirty() const
{
    return mGridInfo.isSliceDirty.x +
           mGridInfo.isSliceDirty.y +
           mGridInfo.isSliceDirty.z > 0;
}

bool Model::GetNormalized() const
{
    return mSphereInfo.isNormalized > 0;
}

void Model::SetNormalized(bool isNormalized)
{
    mSphereInfo.isNormalized = (int)isNormalized;
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0);
    mGridInfoData.ModifySubData(0, sizeof(GPUData::GridInfo), &mGridInfo);
    mSphereInfoData.ModifySubData(0, sizeof(GPUData::SphereInfo), &mSphereInfo);
}

float Model::GetSH0Threshold() const
{
    return mSphereInfo.sh0Threshold;
}

void Model::SetSH0Threshold(float threshold)
{
    mSphereInfo.sh0Threshold = threshold;
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0); // all slices dirty
    mGridInfoData.ModifySubData(0, sizeof(GPUData::GridInfo), &mGridInfo);
    mSphereInfoData.ModifySubData(0, sizeof(GPUData::SphereInfo), &mSphereInfo);
}

glm::ivec4 Model::GetGridDims() const
{
    return mGridInfo.gridDims;
}

void Model::RotateModel(double deltaX, double deltaY)
{
    const float dx = -deltaX * mRotationSpeed;
    const float dy = -deltaY * mRotationSpeed;
    mModelMatrix = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0)) * mModelMatrix;
    mModelMatrix = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * mModelMatrix;

    mModelMatrixData.ModifySubData(0, sizeof(glm::mat4), &mModelMatrix);
}

void Model::TranslateModel(double deltaX, double deltaY)
{
    const float dx = deltaX * mTranslationSpeed;
    const float dy = deltaY * mTranslationSpeed;
    mModelMatrix = glm::translate(dx * glm::vec3(1.0, 0.0, 0.0) -
                                  dy * glm::vec3(0.0, 1.0, 0.0)) * mModelMatrix;

    mModelMatrixData.ModifySubData(0, sizeof(glm::mat4), &mModelMatrix);
}

void Model::ScaleModel(double deltaS)
{
    if(deltaS > std::numeric_limits<double>::epsilon())
    {
        deltaS = 1.0;
    }
    else if(deltaS < std::numeric_limits<double>::epsilon())
    {
        deltaS = -1.0;
    }
    else
    {
        deltaS = 0.0;
    }
    mModelMatrix = glm::scale(glm::vec3(pow(mScalingSpeed, deltaS))) * mModelMatrix;
    mModelMatrixData.ModifySubData(0, sizeof(glm::mat4), &mModelMatrix);
}

float Model::GetTranslationSpeed() const
{
    return mTranslationSpeed;
}

float Model::GetRotationSpeed() const
{
    return mRotationSpeed;
}

float Model::GetScalingSpeed() const
{
    return mScalingSpeed;
}

void Model::SetTranslationSpeed(float speed)
{
    mTranslationSpeed = speed;
    if(mTranslationSpeed < 0.0f)
    {
        mTranslationSpeed = 0.0f;
    }
}

void Model::SetRotationSpeed(float speed)
{
    mRotationSpeed = speed;
    if(mRotationSpeed < 0.0f)
    {
        mRotationSpeed = 0.0f;
    }
}

void Model::SetScalingSpeed(float speed)
{
    mScalingSpeed = speed;
    if(mScalingSpeed < 1.0f)
    {
        mScalingSpeed = 1.0f;
    }
}

void Model::Draw()
{
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}

void Model::ScaleSpheres()
{
    if(!isAnySliceDirty())
    {
        return;
    }
    glUseProgram(mComputeShader.ID());
    glDispatchCompute(mNbSpheres, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
    mGridInfo.isSliceDirty = glm::ivec4(0, 0, 0, 0);
}
} // namespace Scene
} // namespace Engine