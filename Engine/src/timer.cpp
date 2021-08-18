#include <timer.h>
#include <iostream>

namespace Slicer
{
namespace Utilities
{
AutoTimer::AutoTimer()
:mLabel()
{
    mStart = std::chrono::high_resolution_clock::now();
}

AutoTimer::AutoTimer(const std::string& label)
:mLabel(label)
{
    mStart = std::chrono::high_resolution_clock::now();
}

AutoTimer::~AutoTimer()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - mStart);
    std::cout << mLabel << ": DURATION (S):" << duration.count() << std::endl;
}

Timer::Timer()
:mLabel()
,mStart()
{
}

Timer::Timer(const std::string& label)
:mLabel(label)
,mStart()
{
}

void Timer::Start()
{
    mStart = std::chrono::high_resolution_clock::now();
}

double Timer::Stop()
{
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - mStart);
    std::cout << mLabel << ": DURATION (S): " << duration.count() << std::endl;
    return duration.count();
}
} // namespace Utilities
} // namespace Slicer
