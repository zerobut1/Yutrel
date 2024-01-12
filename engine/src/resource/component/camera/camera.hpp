#pragma once

#include <glm/glm.hpp>

namespace Yutrel
{
    class Camera
    {
    public:
        Camera() = default;
        Camera(glm::vec3 pos)
            : m_world_position(pos) {}

        void Updata(float delta_time);

        glm::mat4 GetViewMatrix() const;

        glm::mat4 GetRotationMatrix() const;

        // 移动
        void MoveFront();
        void MoveBack();
        void MoveLeft();
        void MoveRight();
        void MoveUp();
        void MoveDown();
        void MoveDirection(float offset_x, float offset_y);

    private:
        // 摄像机局部空间
        glm::vec3 m_direction;
        glm::vec3 m_right;
        float m_pitch{0.0f};
        float m_yaw{0.0f};

        // 世界空间
        glm::vec3 m_world_velocity{0.0f};
        glm::vec3 m_world_position{0.0f, 0.0f, 5.0f};

        // 参数
        float m_mouse_sensitivity{0.005f};
    };
} // namespace Yutrel