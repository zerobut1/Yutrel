// #include "yutrel_pch.hpp"

// #include "animation.hpp"
// #include "resource/component/animation/bone.hpp"

// namespace Yutrel
// {

//     Ref<Animation> Animation::Create(const aiScene* ai_scene, Ref<Model> model)
//     {
//         return CreateRef<Animation>(ai_scene, model);
//     }

//     Animation::Animation(const aiScene* ai_scene, Ref<Model> model)
//     {
//         auto ai_animation                 = ai_scene->mAnimations[0];
//         m_duration                        = ai_animation->mDuration;
//         m_ticks_per_second                = ai_animation->mTicksPerSecond;
//         aiMatrix4x4 global_transformation = ai_scene->mRootNode->mTransformation;
//         global_transformation             = global_transformation.Inverse();
//         ReadHierarchyData(m_root, ai_scene->mRootNode);
//         ReadMissingBones(ai_animation, *model);
//     }

//     Ref<Bone> Animation::FindBone(const std::string& name)
//     {
//         auto iter = std::find_if(m_bones.begin(), m_bones.end(), [&](const Ref<Bone> Bone)
//                                  { return Bone->GetBoneName() == name; });
//         if (iter == m_bones.end())
//             return nullptr;
//         else
//             return *iter;
//     }

//     void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
//     {
//         YUTREL_ASSERT(src, "Miss root node!");

//         dest.name           = src->mName.data;
//         dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
//         dest.childrenCount  = src->mNumChildren;

//         for (int i = 0; i < src->mNumChildren; i++)
//         {
//             AssimpNodeData newData;
//             ReadHierarchyData(newData, src->mChildren[i]);
//             dest.children.push_back(newData);
//         }
//     }

//     void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
//     {
//         int size = animation->mNumChannels;

//         auto& boneInfoMap   = model.GetBoneInfoMap(); // getting m_BoneInfoMap from Model class
//         uint32_t& boneCount = model.GetBoneCount();   // getting the m_BoneCounter from Model class

//         // reading channels(bones engaged in an animation and their keyframes)
//         for (int i = 0; i < size; i++)
//         {
//             auto channel         = animation->mChannels[i];
//             std::string boneName = channel->mNodeName.data;

//             if (boneInfoMap.find(boneName) == boneInfoMap.end())
//             {
//                 boneInfoMap[boneName].id = boneCount;
//                 boneCount++;
//             }
//             m_bones.push_back(Bone::Create(channel->mNodeName.data,
//                                            boneInfoMap[channel->mNodeName.data].id,
//                                            channel));
//         }

//         m_bone_info = boneInfoMap;
//     }

//     glm::mat4 Animation ::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
//     {
//         glm::mat4 to;
//         // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
//         to[0][0] = from.a1;
//         to[1][0] = from.a2;
//         to[2][0] = from.a3;
//         to[3][0] = from.a4;
//         to[0][1] = from.b1;
//         to[1][1] = from.b2;
//         to[2][1] = from.b3;
//         to[3][1] = from.b4;
//         to[0][2] = from.c1;
//         to[1][2] = from.c2;
//         to[2][2] = from.c3;
//         to[3][2] = from.c4;
//         to[0][3] = from.d1;
//         to[1][3] = from.d2;
//         to[2][3] = from.d3;
//         to[3][3] = from.d4;
//         return to;
//     }

// } // namespace Yutrel