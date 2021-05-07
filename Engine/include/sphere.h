#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

namespace Engine
{
namespace Primitive
{
class Sphere
{
public:
    Sphere();
    Sphere(unsigned int resolution);
    Sphere(const Sphere& s);
    Sphere& operator=(const Sphere& s);
    std::vector<glm::vec3>& getVertices() { return mVertices; };
    const std::vector<glm::vec3>& getVertices() const { return mVertices; };
    std::vector<GLuint>& getIndices() { return mIndices; };
    std::vector<glm::vec3>& getNormals() { return mNormals; };
    std::vector<glm::vec3>& getColors() { return mColors; };
    std::vector<float>& getThetas() { return mThetas; };
    const std::vector<float>& getThetas() const { return mThetas; };
    std::vector<float>& getPhis() { return mPhis; };
    const std::vector<float>& getPhis() const { return mPhis; };
    int nbVertices() const { return mVertices.size(); };
    int nbIndices() const { return mIndices.size(); };
    void updateNormals();
private:
    void genUnitSphere();
    void addPoint(float theta, float phi,
                  const glm::vec3& color);

    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<GLuint> mIndices;
    std::vector<glm::vec3> mColors;
    unsigned int mResolution;
    std::vector<float> mThetas;
    std::vector<float> mPhis;
};
} // Primitive
} // Engine