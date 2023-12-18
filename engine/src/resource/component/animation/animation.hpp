/**
 * 动画类
 * 读取动画
 */
#pragma once

#include "resource/component/animation/bone.hpp"
#include "resource/component/model/model.hpp"

#include "glm/ext/matrix_float4x4.hpp"
#include <assimp/scene.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Yutrel
{
    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation
    {
    public:
        static Ref<Animation> Create(const aiScene* ai_scene, Ref<Model> model);

    public:
        Animation(const aiScene* ai_scene, Ref<Model> model);
        ~Animation() = default;

        Ref<Bone> FindBone(const std::string& name);

        inline float GetTicksPerSecond() { return m_ticks_per_second; }
        inline float GetDuration() { return m_duration; }
        inline const AssimpNodeData& GetRootNode() { return m_root; }
        inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
        {
            return m_bone_info;
        }

    private:
        void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
        void ReadMissingBones(const aiAnimation* animation, Model& model);

        glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

    private:
        float m_duration;
        int m_ticks_per_second;
        std::vector<Ref<Bone>> m_bones;
        AssimpNodeData m_root;
        std::unordered_map<std::string, BoneInfo> m_bone_info;
    };

} // namespace Yutrel
