/**
 * 变换
 */
#pragma once

#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/glm.hpp>

namespace Yutrel
{
    enum class Space : char
    {
        Local = 1 << 0,
        World = 1 << 1
    };

    class Transform
    {
    public:
        Transform();

        void Translate(const glm::vec3& vector, Space space = Space::World);
        void Translate(float x, float y, float z, Space space = Space::World);

        void Rotate(const glm::vec3& axis, float angle, Space space);
        void Rotate(const glm::vec3& eulers, Space space);
        void Rotate(float euler_x, float euler_y, float euler_z, Space space);
        void Rotate(float quat_x, float quat_y, float quat_z, float quat_w, Space space);

        void Scale(float scale);
        void Scale(const glm::vec3& scale);
        void Scale(float scale_x, float scale_y, float scale_z);

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetTransform(const glm::mat4& transform);

        // converts a vector from local to world space or vice versa
        glm::vec3 Local2World(const glm::vec3& v) const;
        glm::vec3 World2Local(const glm::vec3& v) const;

        // returns the local tranform matrix that converts world space to local space
        glm::mat4 GetLocalTransform() const;
        glm::mat4 GetLocalTransform(const glm::vec3& forward, const glm::vec3& up) const;

    public:
        glm::vec3 position;
        glm::quat rotation;
        glm::mat4 transform;

        float euler_x, euler_y, euler_z;
        float scale_x, scale_y, scale_z;

        glm::vec3 up;
        glm::vec3 forward;
        glm::vec3 right;

    private:
        void RecalculateBasis(void);
        void RecalculateEuler(void);
    };

} // namespace Yutrel