#include <options.h>

namespace Slicer
{
Options::Options()
{
}

Options::~Options()
{
}

Options& Options::Instance() {
    // create instance by lazy initialization
    // guaranteed to be destroyed
    static Options instance;

    return instance;
}

int Options::SetFloat(const std::string& key, float value)
{
    auto it = mFloatField.find(key);
    if(it != mFloatField.end()) // update existing field
    {
        if(it->second != value)
        {
            it->second = value;
            dispatchCallbacks(key);
        }
        return KEYFOUND_STATUS;
    }
    else // initialize new field with value
    {
        if(validateUniqueKey(key))
        {
            mFloatField[key] = value;
            mCallbacks[key] = std::vector<std::function<void()>>();
            return KEYADD_STATUS;
        }
        return KEYINVALID_STATUS;
    }
}

int Options::SetInt(const std::string& key, int value)
{
    auto it = mIntField.find(key);
    if(it != mIntField.end()) // update existing field
    {
        if(it->second != value)
        {
            it->second = value;
            dispatchCallbacks(key);
        }
        return KEYFOUND_STATUS;
    }
    else // initialize new field with value
    {
        if(validateUniqueKey(key))
        {
            mIntField[key] = value;
            mCallbacks[key] = std::vector<std::function<void()>>();
            return KEYADD_STATUS;
        }
        return KEYINVALID_STATUS;
    }
}

int Options::SetBool(const std::string& key, bool value)
{
    auto it = mBoolField.find(key);
    if(it != mBoolField.end()) // update existing field
    {
        if(it->second != value)
        {
            it->second = value;
            dispatchCallbacks(key);
        }
        return KEYFOUND_STATUS;
    }
    else // initialize new field with value
    {
        if(validateUniqueKey(key))
        {
            mBoolField[key] = value;
            mCallbacks[key] = std::vector<std::function<void()>>();
            return KEYADD_STATUS;
        }
        return KEYINVALID_STATUS;
    }
}

int Options::SetString(const std::string& key, std::string value)
{
    auto it = mStringField.find(key);
    if(it != mStringField.end()) // update existing field
    {
        if(it->second != value)
        {
            it->second = value;
            dispatchCallbacks(key);
        }
        return KEYFOUND_STATUS;
    }
    else // initialize new field with value
    {
        if(validateUniqueKey(key))
        {
            mStringField[key] = value;
            mCallbacks[key] = std::vector<std::function<void()>>();
            return KEYADD_STATUS;
        }
        return KEYINVALID_STATUS;
    }
}

int Options::GetFloat(const std::string& key, float* value) const
{
    auto it = mFloatField.find(key);
    if(it != mFloatField.end()) // key is in dict
    {
        *value = it->second;
        return KEYFOUND_STATUS;
    }
    return KEYNOTFOUND_STATUS;
}

int Options::GetInt(const std::string& key, int* value) const
{
    auto it = mIntField.find(key);
    if(it != mIntField.end()) // key is in dict
    {
        *value = it->second;
        return KEYFOUND_STATUS;
    }
    return KEYNOTFOUND_STATUS;
}

int Options::GetBool(const std::string& key, bool* value) const
{
    auto it = mBoolField.find(key);
    if(it != mBoolField.end()) // key is in dict
    {
        *value = it->second;
        return KEYFOUND_STATUS;
    }
    return KEYNOTFOUND_STATUS;
}

int Options::GetString(const std::string& key, std::string* value) const
{
    auto it = mStringField.find(key);
    if(it != mStringField.end()) // key is in dict
    {
        *value = it->second;
        return KEYFOUND_STATUS;
    }
    return KEYNOTFOUND_STATUS;
}

bool Options::validateUniqueKey(const std::string& key) const
{
    auto it = mCallbacks.find(key);
    return it == mCallbacks.end();
}

void Options::RegisterCallback(const std::string& key, std::function<void()> callback)
{
    mCallbacks[key].push_back(callback);
}

void Options::dispatchCallbacks(const std::string& key) const
{
    auto callbacks = mCallbacks.at(key);
    for(auto cb : callbacks)
    {
        cb();
    }
}

ApplicationState::ApplicationState()
:SliceIndices()
,VolumeShape()
,SphereScaling()
,SH0Threshold()
,IsSphereNormalized()
,FODFImage()
{
}
} // namespace Slicer
