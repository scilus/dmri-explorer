#pragma once
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <image.h>
#include <iostream>

namespace Slicer
{
/// \brief An application parameter.
///
/// Callbacks can be registered to this parameter. They will be
/// called whenever the value of type T associated to the parameter is
/// updated.
template <typename T> class ApplicationParameter
{
public:
    /// Default constructor.
    ApplicationParameter()
    :mValue()
    ,mCallbacks()
    ,mIsInit(false)
    {};

    /// Constructor.
    /// \param[in] value The value associated with this parameter.
    ApplicationParameter(const T& value)
    :mValue(value)
    ,mCallbacks()
    ,mIsInit(true)
    {};

    /// Register a callback.
    /// \param[in] callback A function taking two parameters of type T.
    ///                     Will be called whenever Update is called. The
    ///                     first parameter is the previous value and the
    ///                     second is the updated value.
    /// \see ApplicationParameter::Update(const T&)
    void RegisterCallback(const std::function<void(T, T)>& callback)
    {
        mCallbacks.push_back(callback);
    };

    /// Update the value associated with the parameter and call callbacks.
    /// \param[in] value Updated value.
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

    /// Get the value contained in parameter instance.
    /// \return The value.
    T Get() const
    {
        if(!mIsInit)
            std::cout << "WARNING: Accessing non-initialized parameter!" << std::endl;
        return mValue;
    };

    /// Check if the value field is initialized.
    /// \return True if the value field is initialized.
    inline bool IsInit() const { return mIsInit; };

private:
    /// Call all callbacks registered to this object.
    /// \param[in] old Previous value associated to the parameter.
    void onChange(const T& old) const
    {
        for(auto cb : mCallbacks)
        {
            cb(old, mValue);
        }
    };

    /// Is the value initialized?
    bool mIsInit;

    /// The value associated to this object.
    T mValue;

    /// Callbacks to call when updating the value of this object.
    std::vector<std::function<void(T, T)>> mCallbacks;
};

namespace State
{
/// Struct containing global parameters for the voxel grid.
struct Grid
{
    /// Default constructor
    Grid()
    :SliceIndices()
    ,VolumeShape(){};

    /// The 3D indice of the current slice of interest.
    ApplicationParameter<glm::ivec3> SliceIndices;

    /// The maximum dimensions of the voxel grid.
    ApplicationParameter<glm::ivec3> VolumeShape;
};

/// Struct containing global parameters for the fODF sphere.
struct Sphere
{
    /// Default constructor
    Sphere()
    :Scaling()
    ,SH0Threshold()
    ,IsNormalized()
    ,Resolution()
    ,FadeIfHidden(){};

    /// The scaling factor for the sphere glyphs.
    ApplicationParameter<float> Scaling;

    /// The threshold on the 0th SH coefficient.
    ApplicationParameter<float> SH0Threshold;

    /// Whether the glyphs are normalized per voxel or not.
    ApplicationParameter<bool> IsNormalized;

    /// Resolution of the sphere. The higher, the more
    /// detailed the representation.
    ApplicationParameter<int> Resolution;

    /// Whether hidden glyphs should fade to black.
    ApplicationParameter<bool> FadeIfHidden;
};

/// Struct containing global parameters for window and inputs.
struct Window
{
    /// Default constructor
    Window()
    :Width()
    ,Height()
    ,TranslationSpeed()
    ,RotationSpeed()
    ,ZoomSpeed(){};

    /// Window width in pixels
    ApplicationParameter<int> Width;

    /// Window height in pixels
    ApplicationParameter<int> Height;

    /// Multiplier on cursor movement to control scene translation speed.
    ApplicationParameter<float> TranslationSpeed;

    /// Multiplier on cursor movement to control scene rotation speed.
    ApplicationParameter<float> RotationSpeed;

    /// Multiplier on mouse wheel movement to control camera zoom speed.
    ApplicationParameter<float> ZoomSpeed;
};
} // namespace State

/// Class containing global parameters for the application.
class ApplicationState
{
public:
    /// Default constructor
    ApplicationState();

    /// Parameters pertaining to the voxel grid.
    State::Grid VoxelGrid;

    /// Parameters pertaining to the fODF sphere.
    State::Sphere Sphere;

    /// Parameters pertaining to the Window state.
    State::Window Window;

    /// Parameter containing the fODF image object.
    ApplicationParameter<NiftiImageWrapper> FODFImage;
};
} // namespace Slicer
