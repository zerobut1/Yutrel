#include "yutrel_pch.hpp"

#include "animator.hpp"

namespace Yutrel
{
    Ref<Animator> Animator::Create(Yutrel::Ref<Model> model)
    {
        return CreateRef<Animator>(model);
    }

    Animator::Animator(Yutrel::Ref<Model> model)
    {
        m_cur_time      = 0.0;
        m_cur_animation = model->GetAnimation();

        m_final_bone_matrices.reserve(100);

        for (int i = 0; i < 100; i++)
        {
            m_final_bone_matrices.emplace_back(glm::mat4(1.0f));
        }
    }

    void Animator::UpdateAnimation(float delta_time)
    {
        m_delta_time = delta_time;
        if (m_cur_animation)
        {
            m_cur_time += m_cur_animation->GetTicksPerSecond() * delta_time;
            m_cur_time = fmod(m_cur_time, m_cur_animation->GetDuration());
            CalculateBoneTransform(&m_cur_animation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::PlayAnimation(Ref<Animation> pAnimation)
    {
        m_cur_animation = pAnimation;
        m_cur_time      = 0.0f;
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName    = node->name;
        glm::mat4 nodeTransform = node->transformation;

        auto Bone = m_cur_animation->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_cur_time);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_cur_animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index                    = boneInfoMap[nodeName].id;
            glm::mat4 offset             = boneInfoMap[nodeName].offset;
            m_final_bone_matrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

} // namespace Yutrel