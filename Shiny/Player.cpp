#include "Player.h"

Shiny::Player::Player(const std::shared_ptr<Entity>& entity):
    entity_(entity)
{
}

void Shiny::Player::Update(float deltaTime, const Input& input)
{
    // look direction
    float turnForwardDirection = input.GetRightHorizontalAxis();
    if (turnForwardDirection > 0) {
        currentTurnSpeed_ = -TURN_SPEED;
    } else if (turnForwardDirection < 0) {
        currentTurnSpeed_ = TURN_SPEED;
    } else {
        currentTurnSpeed_ = 0;
    }
    rotationToForword_ += currentTurnSpeed_ * deltaTime;
    rotationToForword_ = WrapAngle(rotationToForword_);

    // body rotation
    float vertical = input.GetLeftVerticalAxis();
    float horizontal = input.GetLeftHorizontalAxis();
    currentRunSpeed_ = RUN_SPEED;
    float bodyRotation = currentHorizontalRotationAngle_ - rotationToForword_;
    if (horizontal > 0) {
        if (vertical > 0) {
            bodyRotation = glm::radians(-45.0f);
        } else if (vertical < 0) {
            bodyRotation = glm::radians(-125.0f);
        } else {
            bodyRotation = glm::radians(-90.0f);
        }
    } else if (horizontal < 0) {
        if (vertical > 0) {
            bodyRotation = glm::radians(+45.0f);
        } else if (vertical < 0) {
            bodyRotation = glm::radians(+125.0f);
        } else {
            bodyRotation = glm::radians(+90.0f);
        }
    } else {
        if (vertical > 0) {
            bodyRotation = 0.0f;
        } else if (vertical < 0) {
            bodyRotation = glm::radians(180.0f);
        } else {
            currentRunSpeed_ = 0.0f;
        }
    }
    bodyRotation = WrapAngle(bodyRotation + rotationToForword_);
    float deltaRotation = WrapAngle(bodyRotation - currentHorizontalRotationAngle_);
    float totalTime = std::fabsf(deltaRotation * 1.0f / (4.0f * TWO_PI));
    if (totalTime <= deltaTime) {
        currentHorizontalRotationAngle_ = bodyRotation;
        float distance = currentRunSpeed_ * deltaTime;
        if (input.Fire0()) {
            entity_->position_.z += distance * vertical;
        } else {
            float dx = distance * std::sinf(currentHorizontalRotationAngle_);
            float dy = distance * std::cosf(currentHorizontalRotationAngle_);
            entity_->position_.x += dx;
            entity_->position_.y += dy;
        }
    } else {
        float rotationPercent = deltaTime / totalTime;
        currentHorizontalRotationAngle_ = WrapAngle(currentHorizontalRotationAngle_ + deltaRotation * rotationPercent);
    }
    float A = (DegreesToRadians(180) + currentHorizontalRotationAngle_) / 2;
    float sinA = std::sinf(A);
    float cosA = std::cosf(A);
    entity_->rotation_.x = 0;
    entity_->rotation_.y = 0;
    entity_->rotation_.z = sinA;
    entity_->rotation_.w = cosA;
}

Shiny::Float3 Shiny::Player::GetPosition() const
{
    return entity_->position_;
}
