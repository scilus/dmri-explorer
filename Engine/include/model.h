#pragma once
#include <glm/matrix.hpp>
#include <coordinate_system.h>
#include <memory>
#include <data.h>

class Model : public WorldObject
{
public:
    Model();
    ~Model();

    void Draw();
protected:
    virtual void drawSpecific() = 0;
    virtual void initOptions() = 0;
    virtual void initOptionsCallbacks() = 0;
};