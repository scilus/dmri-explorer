#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

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
