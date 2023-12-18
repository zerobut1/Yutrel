#pragma once

#include "core/macro.hpp"

#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "spdlog/fmt/bundled/core.h"
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{

    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    class Bone
    {
    public:
        static Ref<Bone> Create(const std::string& name, uint32_t ID, const aiNodeAnim* channel);

    public:
        Bone(const std::string& name, uint32_t ID, const aiNodeAnim* channel);
        virtual ~Bone() = default;

        void Update(float animationTime);

        glm::mat4 GetLocalTransform() { return m_local_transform; }
        std::string GetBoneName() const { return m_name; }
        uint32_t GetBoneID() { return m_id; }

    private:
        uint32_t GetPositionIndex(float animationTime);
        uint32_t GetRotationIndex(float animationTime);
        uint32_t GetScaleIndex(float animationTime);

        glm::mat4 InterpolatePosition(float animationTime);
        glm::mat4 InterpolateRotation(float animationTime);
        glm::mat4 InterpolateScaling(float animationTime);

        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

    private:
        std::vector<KeyPosition> m_positions;
        std::vector<KeyRotation> m_rotations;
        std::vector<KeyScale> m_scales;
        uint32_t m_position_num;
        uint32_t m_rotation_num;
        uint32_t m_scale_num;

        glm::mat4 m_local_transform;
        std::string m_name;
        uint32_t m_id;
    };

} // namespace Yutrel