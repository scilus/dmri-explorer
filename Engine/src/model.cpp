#include "model.h"
#include <utils.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdint>

namespace Engine
{
namespace GL
{
Model::Model(std::shared_ptr<Loader::Image> image)
    :mImage(image)
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
    addToVAO(mVerticesBO, BindableProperty::position);
    addToVAO(mNormalsBO, BindableProperty::normal);
    addToVAO(mColorBO, BindableProperty::color);
    addToVAO(mIndicesBO, BindableProperty::indice);

    // Bind uniform buffers to GPU
    mInstanceTransformsData = genShaderData<glm::mat4*>(mInstanceTransforms.data(),
                                                        BindableProperty::model,
                                                        sizeof(glm::mat4) * mInstanceTransforms.size(),
                                                        true);
}

Model::~Model()
{
}

uint to1d(uint i, uint j, uint k, glm::vec<4, int> gridDims)
{
    return i * (gridDims.y * gridDims.z) + j * gridDims.z + k;
};

void Model::genPrimitives()
{
    Sphere sphere = genUnitSphere(10);
    const glm::vec<4, int> gridDims = mImage->dims();
    const glm::vec3 gridCenter((gridDims.x - 1) / 2.0f,
                               (gridDims.y - 1) / 2.0f,
                               (gridDims.z - 1) / 2.0f);

    for(uint i = 0; i < gridDims.x; ++i)
    {
        for(uint j = 0; j < gridDims.y; ++j)
        {
            for(uint k = 0; k < gridDims.z; ++k)
            {
                mVertices.insert(mVertices.end(),
                                 sphere.vertices.begin(),
                                 sphere.vertices.end());
                mNormals.insert(mNormals.end(),
                                sphere.normals.begin(),
                                sphere.normals.end());
                mIndices.insert(mIndices.end(),
                                sphere.indices.begin(),
                                sphere.indices.end());
                mColors.insert(mColors.end(),
                               sphere.color.begin(),
                               sphere.color.end());
                glm::mat4 modelMat = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f,
                                               0.0f, 0.5f, 0.0f, 0.0f,
                                               0.0f, 0.0f, 0.5f, 0.0f,
                                               i - gridCenter.x,
                                               j - gridCenter.y,
                                               k - gridCenter.z,
                                               1.0f);
                mInstanceTransforms.push_back(modelMat);
                const uint index1d = to1d(i, j, k, gridDims);
                mIndirectCmd.push_back(
                    DrawElementsIndirectCommand(sphere.indices.size(),
                                                1,
                                                index1d*sphere.indices.size(),
                                                index1d*sphere.vertices.size(),
                                                index1d));
            }
        }
    }
}

Sphere Model::genUnitSphere(const int resolution) const
{
    Sphere sphere;
    const float thetaMax = M_PI;
    const float phiMax = 2.0 * M_PI;
    const int maxThetaSteps = resolution;
    const int maxPhiSteps = 2 * maxThetaSteps;

    // Create sphere vertices and normals
    float theta;
    float phi;
    glm::vec3 vertice;
    for(int i = 0; i <= maxPhiSteps; ++i)
    {
        for(int j = 0; j <= maxThetaSteps; ++j)
        {
            theta = j * thetaMax / maxThetaSteps;
            phi = i * phiMax / maxPhiSteps;
            vertice = sphericalToCartesian(1.0f, theta, phi);
            sphere.vertices.push_back(vertice);
            sphere.normals.push_back(vertice);
            sphere.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }

    // Create faces from vertices
    int ii, jj, kk, ll, mm, nn;
    for(int i = 0; i < maxPhiSteps; ++i)
    {
        for(int j = 0; j < maxThetaSteps; ++j)
        {
            ii = i * (maxThetaSteps + 1) + j;
            sphere.indices.push_back(ii);
            jj = ii + 1;
            sphere.indices.push_back(jj);
            kk = jj + maxThetaSteps;
            sphere.indices.push_back(kk);
            ll = jj;
            sphere.indices.push_back(ll);
            mm = kk + 1;
            sphere.indices.push_back(mm);
            nn = kk;
            sphere.indices.push_back(nn);
        }
    }
    return sphere;
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
    case BindableProperty::color:
    case BindableProperty::normal:
    case BindableProperty::position:
        type = GL_FLOAT;
        size = sizeof(float) * 3;
        count = 3;
        break;
    case BindableProperty::indice:
        type = GL_UNSIGNED_INT;
        size = sizeof(GLuint);
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

void Model::multiDrawElementsIndirect(GLenum mode, GLenum type,
                                      const void* indirect,
                                      GLsizei drawcount,
                                      GLsizei stride) const
{
    if(type != GL_UNSIGNED_INT)
    {
        throw std::runtime_error("Invalid type for element buffer.");
    }
    size_t sizeOfType = sizeof(GLuint);

    GLsizei n;
    ShaderData<GLsizei> drawID(0, BindableProperty::drawID);
    for (n = 0; n < drawcount; n++)
    {
        const DrawElementsIndirectCommand *cmd;
        if (stride != 0)
        {
            cmd = (const DrawElementsIndirectCommand*)((uintptr_t)indirect + n * stride);
        } else
        {
            cmd = (const DrawElementsIndirectCommand*)indirect + n;
        }

        drawID.ModifySubData(0, sizeof(GLsizei), &n);
        drawID.ToGPU();

        glDrawElementsInstancedBaseVertexBaseInstance(mode,
                                                      cmd->count,
                                                      type,
                                                      (void*)(cmd->firstIndex * sizeOfType),
                                                      cmd->instanceCount,
                                                      cmd->baseVertex,
                                                      cmd->baseInstance);
    }
}

void Model::Draw() const
{
    mInstanceTransformsData.ToGPU();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    multiDrawElementsIndirect(GL_TRIANGLES,
                              GL_UNSIGNED_INT,
                              &mIndirectCmd[0],
                              mImage->flatGridSize(),
                              0);
}
} // namespace GL
} // namespace Engine