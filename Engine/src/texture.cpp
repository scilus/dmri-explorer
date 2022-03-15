#include <texture.h>
#include <glad/glad.h>
#include <timer.h>

namespace Slicer
{
Texture::Texture(const std::shared_ptr<ApplicationState>& state,
                 std::shared_ptr<CoordinateSystem> parent)
:Model(state)
,mIndices()
,mVAO(0)
,mIndicesBO(0)
,mIndirectBO(0)
,mIndirectCmd()
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
    std::cout<<"initProgramPipeline"<<std::endl;
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
    std::cout<<"initializeMembers"<<std::endl;
    const auto& image = mState->BackgroundImage.Get();

    const unsigned int dimX = image.dims().x;
    const unsigned int dimY = image.dims().y;
    mIndices.resize(6);
    mIndirectCmd.resize(6);
    //First triangle
    for(int i = 0; i < 6; ++i)
    {
        mIndices[i] = i;
    }

    // const double r0 = image.at(0, 0, 0, 1)
    // const double g0 = image.at(0, 0, 0, 2)
    // const double b0 = image.at(0, 0, 0, 3)

    mIndirectCmd[0] = DrawArrays(0,0,0,255,255,255);
    mIndirectCmd[1] = DrawArrays(dimX,0,0,255,255,255);
    mIndirectCmd[2] = DrawArrays(0,dimY,0,255,255,255);

    //Second triangle
    mIndirectCmd[3] = DrawArrays(dimX,0,0,255,255,255);
    mIndirectCmd[4] = DrawArrays(0,dimY,0,255,255,255);
    mIndirectCmd[5] = DrawArrays(dimX,dimY,0,255,255,255);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawArrays>(mIndirectCmd);
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
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_ARRAY_BUFFER, mIndirectBO);
    glDrawArrays(GL_TRIANGLES, 0, mIndirectCmd.size());
}
} // namespace Slicer
