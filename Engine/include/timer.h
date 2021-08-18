#pragma once
#include <chrono>
#include <string>

namespace Slicer
{
namespace Utilities
{
class AutoTimer
{
public:
    AutoTimer();
    AutoTimer(const std::string& label);
    ~AutoTimer();
private:
    std::chrono::high_resolution_clock::time_point mStart;
    std::string mLabel;
};

class Timer
{
public:
    Timer();
    Timer(const std::string& label);
    ~Timer() = default;
    void Start();
    double Stop();
private:
    std::chrono::high_resolution_clock::time_point mStart;
    std::string mLabel;
};
} // namespace Utilities
} // namespace Slicer
