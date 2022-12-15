#include "camera.h"

#include "../utils/log.h"

#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace ngn {

Camera::Camera(const CameraOptions& options)
    : position_(options.position)
    , yaw_(options.yaw)
    , pitch_(options.pitch)
    , world_up_(options.world_up)
    , constrain_pitch_(options.constrain_pitch)
    , fov_(options.fov)
    , pitch_constaints_(options.pitch_constaints)
    , fov_constraints_(options.fov_constraints)
{
    update_vectors();
    LOGF("front %f %f %f", front_.x, front_.y, front_.z);
}

void Camera::rotate(float yaw, float pitch)
{
    yaw_ -= yaw;
    pitch_ += pitch;
    if (constrain_pitch_) {
        if (pitch_ > pitch_constaints_[1])
            pitch_ = pitch_constaints_[1];
        if (pitch_ < pitch_constaints_[0])
            pitch_ = pitch_constaints_[0];
    }
    update_vectors();
}

void Camera::look_at(glm::vec3 target)
{
    glm::vec3 new_forward = glm::normalize(target - position_);
    yaw_ = glm::degrees(atan2(new_forward.z, new_forward.x) - M_PI / 2);
    yaw_ = yaw_ < 90 ? 360. - yaw_ : yaw_;
    pitch_ = glm::degrees(asin(new_forward.y));
    update_vectors();
}

void Camera::move(glm::vec3 offset)
{
    position_ += offset;
}

void Camera::move_to(glm::vec3 destination)
{
    position_ = destination;
}

void Camera::zoom(float angle)
{
    fov_ -= angle;
    if (fov_ < fov_constraints_[0])
        fov_ = fov_constraints_[0];
    if (fov_ > fov_constraints_[1])
        fov_ = fov_constraints_[1];
}

glm::mat4 Camera::get_view_matrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

float Camera::fov()
{
    return fov_;
}

glm::vec3 Camera::front()
{
    return front_;
}

glm::vec3 Camera::up() const
{
    return up_;
}

glm::vec3 Camera::position() const
{
    return position_;
}

void Camera::update_vectors()
{
    glm::vec3 front;
    front.x = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

}
