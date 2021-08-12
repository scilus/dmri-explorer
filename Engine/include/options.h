#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include <image.h>

namespace Slicer
{
class Options
{
public:
    static Options& Instance();
    
    Options(Options const&) = delete; // Don't forget to disable copy
    void operator=(Options const&) = delete; // Don't forget to disable copy

    int SetFloat(const std::string& key, float value);
    int SetInt(const std::string& key, int value);
    int SetBool(const std::string& key, bool value);
    int SetString(const std::string& key, std::string value);

    void RegisterCallback(const std::string& key, std::function<void()> callback);

    int GetFloat(const std::string& key, float* value) const;
    int GetInt(const std::string& key, int* value) const;
    int GetBool(const std::string& key, bool* value) const;
    int GetString(const std::string& key, std::string* value) const;

    static const int KEYADD_STATUS = 0;
    static const int KEYFOUND_STATUS = 1;
    static const int KEYNOTFOUND_STATUS = -1;
    static const int KEYINVALID_STATUS = -2;
private:
    Options(); // forbid create instance outside
    ~Options(); // forbid to delete instance outside
    bool validateUniqueKey(const std::string& key) const;
    void dispatchCallbacks(const std::string& key) const;

    std::unordered_map<std::string, float> mFloatField;
    std::unordered_map<std::string, int> mIntField;
    std::unordered_map<std::string, bool> mBoolField;
    std::unordered_map<std::string, std::string> mStringField;

    std::unordered_map<std::string, std::vector<std::function<void()>>> mCallbacks;
};

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

    ApplicationParameter(const T& value, const std::function<void(T)>& callback)
    :mValue(value)
    ,mCallbacks(1, callback)
    ,mIsInit(true)
    {};

    void RegisterCallback(const std::function<void(T)>& callback)
    {
        mCallbacks.push_back(callback);
    };

    void Update(const T& value)
    {
        if(!mIsInit)
        {
            mIsInit = true;
        }
        mValue = value;
        onChange();
    };

    T Get() const
    {
        return mValue;
    };

    inline bool IsInit() const { return mIsInit; };

private:
    void onChange() const
    {
        for(auto cb : mCallbacks)
        {
            cb(mValue);
        }
    };

    bool mIsInit;
    T mValue;
    std::vector<std::function<void(T)>> mCallbacks;
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
    ,Height(){};

    ApplicationParameter<int> Width;
    ApplicationParameter<int> Height;
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
