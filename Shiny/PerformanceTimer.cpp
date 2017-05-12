#include "PerformanceTimer.h"
#include <SDL/SDL.h>
#include <iostream>

Shiny::PerformanceTimer::PerformanceTimer() {
    Reset();
}

void Shiny::PerformanceTimer::Reset() {
    invFrequency_ = 1.0 / static_cast<double>(SDL_GetPerformanceFrequency());
    lastTime_ = SDL_GetPerformanceCounter();
    currentTime_ = lastTime_;
}

void Shiny::PerformanceTimer::Tick() {
    lastTime_ = currentTime_;
    currentTime_ = SDL_GetPerformanceCounter();
    deltaTime_ = static_cast<float>(static_cast<double>(currentTime_ - lastTime_) * invFrequency_);

    static int frames = 0;
    static float seconds = 0;
    frames++;
    seconds += deltaTime_;
    if (seconds >= 1.0f) {
        std::cerr << "\r" << frames / seconds;
        frames = 0;
        seconds = 0.0f;
    }
}