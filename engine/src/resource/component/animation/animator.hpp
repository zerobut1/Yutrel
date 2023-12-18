/**
 * 动画的驱动
 */
#pragma once

#include "resource/component/animation/animation.hpp"
#include "resource/component/animation/bone.hpp"
#include "resource/component/model/model.hpp"

namespace Yutrel
{
    class Animator
    {
    public:
        static Ref<Animator> Create(Yutrel::Ref<Model> model);

    public:
        Animator(Yutrel::Ref<Model> model);
        virtual ~Animator() = default;

        void UpdateAnimation(float delta_time);
        void PlayAnimation(Ref<Animation> pAnimation);
        void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

        std::vector<glm::mat4> GetFinalBoneMatrices()
        {
            return m_final_bone_matrices;
        }

    private:
        std::vector<glm::mat4> m_final_bone_matrices;
        Ref<Animation> m_cur_animation;
        float m_cur_time;
        float m_delta_time;
    };

} // namespace Yutrel