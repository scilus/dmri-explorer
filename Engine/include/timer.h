#pragma once
#include <chrono>
#include <string>

namespace Slicer
{
namespace Utilities
{
/// \brief Timer that counts its duration of existence.
///
/// The timer starts counting on construction and stops when it
/// gets destroyed, printing total elapsed time.
class AutoTimer
{
public:
    /// Default constructor.
    AutoTimer();

    /// Constructor.
    /// \param[in] label Message to print with time.
    AutoTimer(const std::string& label);

    /// \brief Destructor.
    ///
    /// Calculates and prints elapsed time.
    ~AutoTimer();

private:
    /// Start time point.
    std::chrono::high_resolution_clock::time_point mStart;

    /// Optional message to print.
    std::string mLabel;
};

/// Classic timer.
class Timer
{
public:
    /// Default constructor.
    Timer();

    /// Constructor.
    /// \param[in] label Message to print with time.
    Timer(const std::string& label);

    /// Destructor.
    ~Timer() = default;

    /// Start (or restart) timer.
    void Start();

    /// Stop timer and print elapsed time.
    double Stop();
private:
    /// Start time point.
    std::chrono::high_resolution_clock::time_point mStart;

    /// Optional message to print.
    std::string mLabel;
};
} // namespace Utilities
} // namespace Slicer
