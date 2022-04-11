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
,mIsSliceDirty(true)
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

    const int dimX = image.dims().x;
    const int dimY = image.dims().y;
    const int dimZ = image.dims().z;
    const int nCoeffs = image.dims().w;

    const double max = image.getMax();

    for(int k = 0; k < dimZ; ++k)
    {
        for(int j = 0; j < dimY; ++j)
        {
            for(int i = 0; i < dimX; ++i)
            {
                for(int l = 0; l < nCoeffs; ++l)
                {
                    mData.push_back(float(image.at(i,j,k,l))/max);
                }
            }
        }
    }


    //Plan XY
    mVertices.push_back(glm::vec3(0.0f,0.0f,ceil(dimZ/2.0f)));
    mVertices.push_back(glm::vec3(0.0f,dimY,ceil(dimZ/2.0f)));
    mVertices.push_back(glm::vec3(dimX,0.0f,ceil(dimZ/2.0f)));

    mVertices.push_back(glm::vec3(0.0f,dimY,ceil(dimZ/2.0f)));
    mVertices.push_back(glm::vec3(dimX,dimY,ceil(dimZ/2.0f)));
    mVertices.push_back(glm::vec3(dimX,0.0f,ceil(dimZ/2.0f)));

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
    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),0.0f,0.0f));
    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),0.0f,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),dimY,0.0f));

    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),0.0f,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),dimY,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2.0f),dimY,0.0f));

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
    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2.0f),0.0f));
    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2.0f),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2.0f),0.0f));

    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2.0f),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2.0f),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2.0f),0.0f));

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

    unsigned int texture;
    glCreateTextures(GL_TEXTURE_3D, 1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);

    auto type = GL_RGB;
    if (nCoeffs == 1)
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
    
    glTexImage3D(GL_TEXTURE_3D, 0, type, dimX, dimY, dimZ, 0, type, GL_FLOAT, &mData[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    glCreateVertexArrays(1, &mVAO);

    const GLuint verticesIndex = 0;
    mVerticesBO = genVBO<glm::vec3>(mVertices);

    glEnableVertexArrayAttrib(mVAO, verticesIndex);
    glVertexArrayAttribFormat(mVAO, verticesIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, verticesIndex, mVerticesBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, verticesIndex, 0);
    glVertexArrayAttribBinding(mVAO, verticesIndex, verticesIndex);

    const GLuint texIndex = 1;
    mTextureCoordsBO = genVBO<glm::vec3>(mTextureCoords);

    glEnableVertexArrayAttrib(mVAO, texIndex);
    glVertexArrayAttribFormat(mVAO, texIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, texIndex, mTextureCoordsBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, texIndex, 0);
    glVertexArrayAttribBinding(mVAO, texIndex, texIndex);

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
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}
} // namespace Slicer
