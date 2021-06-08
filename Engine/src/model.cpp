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
namespace Scene
{
Model::Model(std::shared_ptr<Image::NiftiImageWrapper> image)
    :mImage(image)
    ,mGridDims(image->dims())
    ,mVertices()
    ,mIndirectCmd()
    ,mColors()
    ,mNormals()
    ,mIndices()
    ,mInstanceTransforms()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mVerticesBO(0)
    ,mNormalsBO(0)
    ,mColorBO(0)
    ,mIndirectBO(0)
    ,mInstanceTransformsData()
{
    // Generate primitives
    genPrimitives();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mVerticesBO = genVBO<glm::vec3>(mVertices);
    mColorBO = genVBO<glm::vec3>(mColors);
    mNormalsBO = genVBO<glm::vec3>(mNormals);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    addToVAO(mVerticesBO, GPUData::BindableProperty::position);
    addToVAO(mNormalsBO, GPUData::BindableProperty::normal);
    addToVAO(mColorBO, GPUData::BindableProperty::color);

    // Bind uniform buffers to GPU
    mInstanceTransformsData = genShaderData<glm::mat4*>(mInstanceTransforms.data(),
                                                        GPUData::BindableProperty::model,
                                                        sizeof(glm::mat4) * mInstanceTransforms.size(),
                                                        true);
}

Model::~Model()
{
}

void Model::genPrimitives()
{
    Primitive::Sphere sphere(14);
    sphere.updateNormals();
    const glm::vec3 gridCenter((mGridDims.x - 1) / 2.0f,
                               (mGridDims.y - 1) / 2.0f,
                               (mGridDims.z - 1) / 2.0f);

    const uint nVox = mGridDims.x * mGridDims.y * mGridDims.z;
    double coeff0;
    Math::SH::SphHarmBasis basis(8);
    for(uint flatIndex = 0; flatIndex < nVox; ++flatIndex)
    {
        glm::vec<3, uint> indice3D = mImage->unravelIndex3d(flatIndex);
        coeff0 = mImage->at(indice3D.x, indice3D.y, indice3D.z, 0);
        if(!doubleEqual(coeff0, 0.0))
        {
            Primitive::Sphere scaledSphere = basis.evaluate(mImage,
                                                            indice3D.x,
                                                            indice3D.y,
                                                            indice3D.z,
                                                            sphere);
            mVertices.insert(mVertices.end(),
                             scaledSphere.getVertices().begin(),
                             scaledSphere.getVertices().end());
            mNormals.insert(mNormals.end(),
                            scaledSphere.getNormals().begin(),
                            scaledSphere.getNormals().end());
            mIndices.insert(mIndices.end(),
                            scaledSphere.getIndices().begin(),
                            scaledSphere.getIndices().end());
            mColors.insert(mColors.end(),
                           scaledSphere.getColors().begin(),
                           scaledSphere.getColors().end());

            float scale = 0.1f;
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
    case GPUData::BindableProperty::color:
    case GPUData::BindableProperty::normal:
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

void Model::Draw() const
{
    mInstanceTransformsData.ToGPU();
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