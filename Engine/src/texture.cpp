#include <texture.h>
#include <glad/glad.h>
#include <timer.h>

namespace Slicer
{
Texture::Texture(const std::shared_ptr<ApplicationState>& state,
                 std::shared_ptr<CoordinateSystem> parent)
:mIndices()
,mVAO(0)
,mIndicesBO(0)
,mIndirectBO(0)
,mIndirectCmd()
{
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));
    initializeModel();
    initializeMembers();
    initializeGPUData();
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
    std::cout<<"initProgramPipeline0"<<std::endl;

    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/texture_frag.glsl");
    std::cout<<"initProgramPipeline1"<<std::endl;

    std::vector<GPU::ShaderProgram> shaders;
    std::cout<<"initProgramPipeline2"<<std::endl;

    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    std::cout<<"initProgramPipeline3"<<std::endl;

    mProgramPipeline = GPU::ProgramPipeline(shaders);
}

void Texture::initializeMembers()
{
    std::cout<<"initializeMembers"<<std::endl;
    const auto& image = mState->BackgroundImage.Get();
    std::cout<<"1"<<std::endl;
    const unsigned int dimX = image.dims().x;
    const unsigned int dimY = image.dims().y;
    //First triangle
    std::cout<<"1"<<std::endl;
    for(int i = 0; i < 6; ++i)
    {
        mIndices[i] = i;
    }
    std::cout<<"2"<<std::endl;

    // const double r0 = image.at(0, 0, 0, 1)
    // const double g0 = image.at(0, 0, 0, 2)
    // const double b0 = image.at(0, 0, 0, 3)


    mIndirectCmd[0] = DrawArrays(0,0,0,0,0,0);
    mIndirectCmd[1] = DrawArrays(dimX,0,0,0,0,0);
    mIndirectCmd[2] = DrawArrays(0,dimY,0,0,0,0);

    //Second triangle
    mIndirectCmd[3] = DrawArrays(dimX,0,0,0,0,0);
    mIndirectCmd[4] = DrawArrays(0,dimY,0,0,0,0);
    mIndirectCmd[5] = DrawArrays(dimX,dimY,0,0,0,0);
    std::cout<<"3"<<std::endl;

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawArrays>(mIndirectCmd);
    std::cout<<"4"<<std::endl;

}

void Texture::initializeGPUData()
{
}

template <typename T>
GLuint Texture::genVBO(const std::vector<T>& data) const
{
    std::cout<<"genVBO"<<std::endl;
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void Texture::drawSpecific()
{
    std::cout<<"drawSpecific"<<std::endl;

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_ARRAY_BUFFER, mIndirectBO);
    glDrawArrays(GL_TRIANGLES, 0, mIndirectCmd.size());
}
} // namespace Slicer
