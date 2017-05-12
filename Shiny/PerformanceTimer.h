#pragma once
#include <cstdint>
namespace Shiny
{
class PerformanceTimer {
public:
    PerformanceTimer();
    ~PerformanceTimer() = default;
    void Reset();
    void Tick();
    float GetDeltaTime() const { return deltaTime_; };
private:
    uint64_t lastTime_ = 0;
    uint64_t currentTime_ = 0;
    double invFrequency_ = 0;
    float deltaTime_ = 0;
};
}