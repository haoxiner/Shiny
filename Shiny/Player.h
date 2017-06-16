#pragma once
#include "Entity.h"
#include "Input.h"
#include "MathUtil.h"
#include <memory>
namespace Shiny
{
class Player
{
public:
    Player(const std::shared_ptr<Entity>& entity);
    void Update(float deltaTime, const Input& input);
    Float3 GetPosition() const;
    float GetRotationToForward() const { return rotationToForword_; }
private:
    const float RUN_SPEED = 20;
    const float TURN_SPEED = 3.14 * 0.5;
    float currentRunSpeed_ = 0.0f;
    float currentTurnSpeed_ = 0.0f;
    float rotationToForword_ = 0.0f;
    float currentHorizontalRotationAngle_ = 0.0f;
private:
    std::shared_ptr<Entity> entity_;
};
}