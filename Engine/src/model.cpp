#include "model.h"
#include <utils.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <spherical_harmonic.h>
#include <sphere.h>

namespace Engine
{
namespace GL
{
Model::Model(std::shared_ptr<Image::NiftiImageWrapper> image)
    :mImage(image)
    ,mGridDims(image->dims())
    ,mThetas()
    ,mPhis()
    ,mIndirectCmd()
    ,mIndices()
    ,mInstanceTransforms()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mVerticesBO(0)
    ,mNormalsBO(0)
    ,mColorBO(0)
    ,mIndirectBO(0)
    ,mInstanceTransformsData()
    ,mSphHarmCoeffs()
{
    // Generate primitives
    genPrimitives();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
    mThetasBO = genVBO<float>(mThetas);
    mPhisBO = genVBO<float>(mPhis);

    addToVAO(mThetasBO, BindableProperty::theta);
    addToVAO(mPhisBO, BindableProperty::phi);

    // Bind uniform buffers to GPU
    mInstanceTransformsData = genShaderData<glm::mat4*>(mInstanceTransforms.data(),
                                                        BindableProperty::model,
                                                        sizeof(glm::mat4) * mInstanceTransforms.size(),
                                                        true);
    mSphHarmCoeffsData = genShaderData<float*>(mSphHarmCoeffs.data(),
                                               BindableProperty::sphHarm,
                                               sizeof(float)* mSphHarmCoeffs.size(),
                                               true);
}

Model::~Model()
{
}

void Model::genPrimitives()
{
    Primitive::Sphere sphere(14);
    const glm::vec3 gridCenter((mGridDims.x - 1) / 2.0f,
                               (mGridDims.y - 1) / 2.0f,
                               (mGridDims.z - 1) / 2.0f);

    const uint nVox = mGridDims.x * mGridDims.y * mGridDims.z;

    mThetas.reserve(nVox * sphere.nbVertices());
    mPhis.reserve(nVox * sphere.nbVertices());
    mIndices.reserve(nVox * sphere.nbIndices());
    const float scale = 5.0f;
    for(uint flatIndex = 0; flatIndex < nVox; ++flatIndex)
    {
        glm::vec<3, uint> indice3D = mImage->unravelIndex3d(flatIndex);
        for(int k = 0; k < 45; ++k)
        {
            mSphHarmCoeffs.push_back(
                static_cast<float>(mImage->at(indice3D.x,
                                              indice3D.y,
                                              indice3D.z,
                                              k)
                )
            );
        }

        mPhis.insert(mPhis.end(),
                     sphere.getPhis().begin(),
                     sphere.getPhis().end());
        mThetas.insert(mThetas.end(),
                       sphere.getThetas().begin(),
                       sphere.getThetas().end());
        mIndices.insert(mIndices.end(),
                        sphere.getIndices().begin(),
                        sphere.getIndices().end());

        glm::mat4 modelMat = glm::mat4(scale, 0.0f, 0.0f, 0.0f,
                                       0.0f, scale, 0.0f, 0.0f,
                                       0.0f, 0.0f, scale, 0.0f,
                                       indice3D.x - gridCenter.x,
                                       indice3D.y - gridCenter.y,
                                       indice3D.z - gridCenter.z,
                                       1.0f);
        mInstanceTransforms.push_back(modelMat);
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(sphere.nbIndices(),
                                        1,
                                        mIndirectCmd.size() * sphere.nbIndices(),
                                        mIndirectCmd.size() * sphere.nbVertices(),
                                        mIndirectCmd.size()));
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
ShaderData<T> Model::genShaderData(const T& data,
                                   const BindableProperty& binding) const
{
    return ShaderData<T>(data, binding);
}

template <typename T>
ShaderData<T> Model::genShaderData(const T& data,
                                   const BindableProperty& binding,
                                   size_t sizeofT, bool isPtr) const
{
    return ShaderData<T>(data, binding, sizeofT, isPtr);
}

void Model::addToVAO(const GLuint& vbo, const BindableProperty& binding)
{
    GLuint type, size, count;
    switch(binding)
    {
    case BindableProperty::theta:
    case BindableProperty::phi:
        type = GL_FLOAT;
        size = sizeof(float);
        count = 1;
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
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES,
                                GL_UNSIGNED_INT,
                                (GLvoid*)0,
                                mIndirectCmd.size(),
                                0);
}
} // namespace GL
} // namespace Engine