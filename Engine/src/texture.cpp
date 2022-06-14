#include <texture.h>
#include <glad/glad.h>
#include <timer.h>
#include <math.h>

namespace Slicer
{
Texture::Texture(const std::shared_ptr<ApplicationState>& state,
                 std::shared_ptr<CoordinateSystem> parent)
:Model(state)
,mVAO(0)
,mVerticesBO(0)
,mVertices()
,mTextureCoordsBO(0)
,mTextureCoords()
,mSliceBO(0)
,mSlice()
,mData()
{
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));
    initializeModel();
    initializeMembers();
}

Texture::~Texture()
{
}

void Texture::updateApplicationStateAtInit()
{
}

void Texture::registerStateCallbacks()
{
}


void Texture::initProgramPipeline()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/texture_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/texture_frag.glsl");
    std::vector<GPU::ShaderProgram> shaders;

    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));

    mProgramPipeline = GPU::ProgramPipeline(shaders);
}

void Texture::initializeMembers()
{
    const auto& image = mState->BackgroundImage.Get();

    // copy image data because we need to normalize
    mData = image.GetVoxelData();
    const auto max = image.GetMax();
    for(size_t i = 0; i < mData.size(); ++i)
    {
        mData[i] = mData[i] / max;
    }
    const auto dims = image.GetDims();

    //Create 2 triangles to create a plan for texture
    //Plan XY
    mVertices.push_back(glm::vec3(0.0f,0.0f,ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(0.0f,dims.y,ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x,0.0f,ceil(dims.z/2.0f)));

    mVertices.push_back(glm::vec3(0.0f,dims.y,ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x,dims.y,ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x,0.0f,ceil(dims.z/2.0f)));

    mTextureCoords.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));

    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSlice.push_back(glm::vec3(0.0f,0.0f,1.0f));

    //Plan YZ
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,0.0f));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,0.0f));

    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,0.0f));

    mTextureCoords.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSlice.push_back(glm::vec3(1.0f,0.0f,0.0f));

    //Plan XZ
    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),0.0f));
    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),0.0f));

    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),0.0f));

    mTextureCoords.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTextureCoords.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTextureCoords.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));

    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSlice.push_back(glm::vec3(0.0f,1.0f,0.0f));

    //Create texture
    unsigned int texture;
    glCreateTextures(GL_TEXTURE_3D, 1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);

    auto type = GL_RGB;
    if (dims.w == 1)
    {
        type = GL_RED;
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage3D(GL_TEXTURE_3D, 0, type, dims.x, dims.y, dims.z, 0, type, GL_FLOAT, &mData[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    glCreateVertexArrays(1, &mVAO);

    //Bind vertices
    const GLuint verticesIndex = 0;
    mVerticesBO = genVBO<glm::vec3>(mVertices);

    glEnableVertexArrayAttrib(mVAO, verticesIndex);
    glVertexArrayAttribFormat(mVAO, verticesIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, verticesIndex, mVerticesBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, verticesIndex, 0);
    glVertexArrayAttribBinding(mVAO, verticesIndex, verticesIndex);

    //Bind texture coordinates
    const GLuint texIndex = 1;
    mTextureCoordsBO = genVBO<glm::vec3>(mTextureCoords);

    glEnableVertexArrayAttrib(mVAO, texIndex);
    glVertexArrayAttribFormat(mVAO, texIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, texIndex, mTextureCoordsBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, texIndex, 0);
    glVertexArrayAttribBinding(mVAO, texIndex, texIndex);

    //Bind Slices
    const GLuint sliceIndex = 2;
    mSliceBO = genVBO<glm::vec3>(mSlice);

    glEnableVertexArrayAttrib(mVAO, sliceIndex);
    glVertexArrayAttribFormat(mVAO, sliceIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, sliceIndex, mSliceBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, sliceIndex, 0);
    glVertexArrayAttribBinding(mVAO, sliceIndex, sliceIndex);
}

template <typename T>
GLuint Texture::genVBO(const std::vector<T>& data) const
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void Texture::drawSpecific()
{
    glDisable(GL_CULL_FACE);
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mVertices.size()));
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}
} // namespace Slicer
