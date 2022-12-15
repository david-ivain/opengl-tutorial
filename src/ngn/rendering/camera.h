#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <array>

namespace ngn {

struct CameraOptions {
    /**
     * @brief Direction of up in the world.
     */
    glm::vec3 world_up { 0, 1, 0 };
    glm::vec3 position { 0, 0, 0 };
    float yaw { 0 };
    float pitch { 0 };
    /**
     * @brief Do we need to constain the pitch value.
     */
    bool constrain_pitch { true };
    /**
     * @brief Min and max constraints for the pitch. Only if {{constrain_pitch}} is set to true.
     */
    std::array<float, 2> pitch_constaints { -89.9, 89.9 };
    float fov { 45 };
    /**
     * @brief Min and max constraints for the field of view.
     */
    std::array<float, 2> fov_constraints { 1, 90 };
};

class Camera {
public:
    Camera(const CameraOptions&);
    ~Camera() = default;

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;

    void rotate(float yaw, float pitch);
    void look_at(glm::vec3 target);
    void move(glm::vec3);
    void move_to(glm::vec3);
    void zoom(float angle);

    glm::mat4 get_view_matrix();

    float fov();
    glm::vec3 front();
    glm::vec3 up() const;
    glm::vec3 position() const;

private:
    /**
     * @brief Updates the camera's vectors based on current yaw and pitch.
     */
    void update_vectors();

    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;
    glm::vec3 world_up_;

    float yaw_;
    float pitch_;
    bool constrain_pitch_;
    std::array<float, 2> pitch_constaints_;
    float fov_;
    std::array<float, 2> fov_constraints_;
};

}
