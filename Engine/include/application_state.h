#pragma once
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <image.h>
#include <iostream>

namespace Slicer
{
template <typename T>
class ApplicationParameter
{
public:
    ApplicationParameter()
    :mValue()
    ,mCallbacks()
    ,mIsInit(false)
    {};

    ApplicationParameter(const T& value)
    :mValue(value)
    ,mCallbacks()
    ,mIsInit(true)
    {};

    ApplicationParameter(const T& value, const std::function<void(T, T)>& callback)
    :mValue(value)
    ,mCallbacks(1, callback)
    ,mIsInit(true)
    {};

    void RegisterCallback(const std::function<void(T, T)>& callback)
    {
        mCallbacks.push_back(callback);
    };

    void Update(const T& value)
    {
        if(!mIsInit)
        {
            mIsInit = true;
        }
        const T vOld = mValue;
        mValue = value;
        onChange(vOld);
    };

    T Get() const
    {
        if(!mIsInit)
            std::cout << "WARNING: Accessing non-initialized parameter!" << std::endl;
        return mValue;
    };

    inline bool IsInit() const { return mIsInit; };

private:
    void onChange(const T& old) const
    {
        for(auto cb : mCallbacks)
        {
            cb(old, mValue);
        }
    };

    bool mIsInit;
    T mValue;
    std::vector<std::function<void(T, T)>> mCallbacks;
};

namespace State
{
struct Grid
{
    Grid()
    :SliceIndices()
    ,VolumeShape(){};

    ApplicationParameter<glm::ivec3> SliceIndices;
    ApplicationParameter<glm::ivec3> VolumeShape;
};

struct Sphere
{
    Sphere()
    :Scaling()
    ,SH0Threshold()
    ,IsNormalized()
    ,Resolution(){};

    ApplicationParameter<float> Scaling;
    ApplicationParameter<float> SH0Threshold;
    ApplicationParameter<bool> IsNormalized;
    ApplicationParameter<int> Resolution;
};

struct Window
{
    Window()
    :Width()
    ,Height()
    ,TranslationSpeed()
    ,RotationSpeed()
    ,ZoomSpeed(){};

    ApplicationParameter<int> Width;
    ApplicationParameter<int> Height;
    ApplicationParameter<float> TranslationSpeed;
    ApplicationParameter<float> RotationSpeed;
    ApplicationParameter<float> ZoomSpeed;
};
} // namespace State

class ApplicationState
{
public:
    ApplicationState();
    State::Grid VoxelGrid;
    State::Sphere Sphere;
    State::Window Window;
    ApplicationParameter<NiftiImageWrapper> FODFImage;
};
} // namespace Slicer
