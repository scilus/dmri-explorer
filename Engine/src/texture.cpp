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
    //C'est ici que quand on change la slice on change le plan
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
    Utilities::Timer timer("Initialize members texture");
    timer.Start();

    const auto& image = mState->BackgroundImage.Get();

    const int dimX = image.dims().x;
    const int dimY = image.dims().y;
    const int dimZ = image.dims().z;
    const int nCoeffs = image.dims().w;

    for(int i = 0; i < image.nbVox(); ++i)
    {
        glm::vec<3, uint> id3D = image.unravelIndex3d(i);
        for(int j = 0; j < nCoeffs; ++j)
        {
            mData.push_back(static_cast<float>(image.at(id3D.x,id3D.y,id3D.z,j)));
        }
    }

    //Plan XYid3D,dimY,ceil(dimZ/2)));
    mVertices.push_back(glm::vec3(dimX,0.0f,ceil(dimZ/2)));

    //Plan YZ
    mVertices.push_back(glm::vec3(ceil(dimX/2),0.0f,0.0f));
    mVertices.push_back(glm::vec3(ceil(dimX/2),0.0f,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2),dimY,0.0f));

    mVertices.push_back(glm::vec3(ceil(dimX/2),0.0f,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2),dimY,dimZ));
    mVertices.push_back(glm::vec3(ceil(dimX/2),dimY,0.0f));

    //Plan XZ
    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2),0.0f));
    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2),0.0f));

    mVertices.push_back(glm::vec3(0.0f,ceil(dimY/2),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2),dimZ));
    mVertices.push_back(glm::vec3(dimX,ceil(dimY/2),0.0f));


    //Plan XY
    mTextureCoords.push_back(glm::vec2(0.0f,0.0f));
    mTextureCoords.push_back(glm::vec2(0.0f,dimY));
    mTextureCoords.push_back(glm::vec2(dimX,0.0f));

    mTextureCoords.push_back(glm::vec2(0.0f,dimY));
    mTextureCoords.push_back(glm::vec2(dimX,dimY));
    mTextureCoords.push_back(glm::vec2(dimX,0.0f));

    mTextureCoords.push_back(glm::vec2(0.0f,0.0f));
    mTextureCoords.push_back(glm::vec2(0.0f,dimZ));
    mTextureCoords.push_back(glm::vec2(dimY,0.0f));

    mTextureCoords.push_back(glm::vec2(0.0f,dimZ));
    mTextureCoords.push_back(glm::vec2(dimY,dimZ));
    mTextureCoords.push_back(glm::vec2(dimY,0.0f));

    mTextureCoords.push_back(glm::vec2(0.0f,0.0f));
    mTextureCoords.push_back(glm::vec2(0.0f,dimZ));
    mTextureCoords.push_back(glm::vec2(dimX,0.0f));

    mTextureCoords.push_back(glm::vec2(0.0f,dimZ));
    mTextureCoords.push_back(glm::vec2(dimX,dimZ));
    mTextureCoords.push_back(glm::vec2(dimX,0.0f));

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    // void * data = &mData;
    void* data = image.getData();
    //Create texture
    unsigned int texture;

    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_3D, texture);  

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, dimX, dimY, dimZ, 0, GL_RGB, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_3D);

    const GLuint VAOIndex = 0;
    mVerticesBO = genVBO<glm::vec3>(mVertices);

    glEnableVertexArrayAttrib(mVAO, VAOIndex);
    glVertexArrayAttribFormat(mVAO, VAOIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, VAOIndex, mVerticesBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVAO, VAOIndex, 0);
    glVertexArrayAttribBinding(mVAO, VAOIndex, 0);

    const GLuint TexIndex = 1;
    mTextureCoordsBO = genVBO<glm::vec2>(mTextureCoords);

    glEnableVertexArrayAttrib(mVAO, TexIndex);
    glVertexArrayAttribFormat(mVAO, TexIndex, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, TexIndex, mTextureCoordsBO, 0, sizeof(float)*2);
    glVertexArrayBindingDivisor(mVAO, TexIndex, 0);
    glVertexArrayAttribBinding(mVAO, TexIndex, 0);


    timer.Stop();
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
