// #include "bone.hpp"
// #include "glm/ext/matrix_float4x4.hpp"
// #include "glm/ext/quaternion_common.hpp"
// #include "yutrel_pch.hpp"
// #include <stdint.h>

// namespace Yutrel
// {
//     Ref<Bone> Bone::Create(const std::string& name, uint32_t ID, const aiNodeAnim* channel)
//     {
//         return CreateRef<Bone>(name, ID, channel);
//     }

//     Bone::Bone(const std::string& name, uint32_t ID, const aiNodeAnim* channel)
//         : m_name(name),
//           m_id(ID),
//           m_local_transform(1.0f)
//     {
//         m_position_num = channel->mNumPositionKeys;

//         for (int positionIndex = 0; positionIndex < m_position_num; ++positionIndex)
//         {
//             aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
//             float timeStamp       = channel->mPositionKeys[positionIndex].mTime;
//             KeyPosition data;
//             data.position  = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
//             data.timeStamp = timeStamp;
//             m_positions.emplace_back(data);
//         }

//         m_rotation_num = channel->mNumRotationKeys;
//         for (int rotationIndex = 0; rotationIndex < m_rotation_num; ++rotationIndex)
//         {
//             aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
//             float timeStamp            = channel->mRotationKeys[rotationIndex].mTime;
//             KeyRotation data;
//             data.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
//             data.timeStamp   = timeStamp;
//             m_rotations.emplace_back(data);
//         }

//         m_scale_num = channel->mNumScalingKeys;
//         for (int keyIndex = 0; keyIndex < m_scale_num; ++keyIndex)
//         {
//             aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
//             float timeStamp  = channel->mScalingKeys[keyIndex].mTime;
//             KeyScale data;
//             data.scale     = glm::vec3(scale.x, scale.y, scale.z);
//             data.timeStamp = timeStamp;
//             m_scales.emplace_back(data);
//         }
//     }

//     void Bone::Update(float animationTime)
//     {
//         glm::mat4 translation = InterpolatePosition(animationTime);
//         glm::mat4 rotation    = InterpolateRotation(animationTime);
//         glm::mat4 scale       = InterpolateScaling(animationTime);
//         m_local_transform     = translation * rotation * scale;
//     }

//     uint32_t Bone::GetPositionIndex(float animationTime)
//     {
//         for (int index = 0; index < m_position_num - 1; ++index)
//         {
//             if (animationTime < m_positions[index + 1].timeStamp)
//                 return index;
//         }
//         YUTREL_ASSERT(false, "Fail to get position index!");
//         return NULL;
//     }

//     uint32_t Bone::GetRotationIndex(float animationTime)
//     {
//         for (int index = 0; index < m_rotation_num - 1; ++index)
//         {
//             if (animationTime < m_rotations[index + 1].timeStamp)
//                 return index;
//         }
//         YUTREL_ASSERT(false, "Fail to get rotation index!");
//         return NULL;
//     }

//     uint32_t Bone::GetScaleIndex(float animationTime)
//     {
//         for (int index = 0; index < m_scale_num - 1; ++index)
//         {
//             if (animationTime < m_scales[index + 1].timeStamp)
//                 return index;
//         }
//         YUTREL_ASSERT(false, "Fail to get scale index!");
//         return NULL;
//     }

//     glm::mat4 Bone::InterpolatePosition(float animationTime)
//     {
//         if (1 == m_position_num)
//             return glm::translate(glm::mat4(1.0f), m_positions[0].position);

//         int p0Index             = GetPositionIndex(animationTime);
//         int p1Index             = p0Index + 1;
//         float scaleFactor       = GetScaleFactor(m_positions[p0Index].timeStamp,
//                                            m_positions[p1Index].timeStamp,
//                                            animationTime);
//         glm::vec3 finalPosition = glm::mix(m_positions[p0Index].position, m_positions[p1Index].position, scaleFactor);
//         return glm::translate(glm::mat4(1.0f), finalPosition);
//     }

//     glm::mat4 Bone::InterpolateRotation(float animationTime)
//     {
//         if (1 == m_rotation_num)
//         {
//             auto rotation = glm::normalize(m_rotations[0].orientation);
//             return glm::mat4(rotation);
//         }

//         int p0Index             = GetRotationIndex(animationTime);
//         int p1Index             = p0Index + 1;
//         float scaleFactor       = GetScaleFactor(m_rotations[p0Index].timeStamp,
//                                            m_rotations[p1Index].timeStamp,
//                                            animationTime);
//         glm::quat finalRotation = glm::slerp(m_rotations[p0Index].orientation, m_rotations[p1Index].orientation, scaleFactor);
//         finalRotation           = glm::normalize(finalRotation);
//         return glm::mat4(finalRotation);
//     }

//     glm::mat4 Bone::InterpolateScaling(float animationTime)
//     {
//         if (1 == m_scale_num)
//             return glm::scale(glm::mat4(1.0f), m_scales[0].scale);

//         int p0Index          = GetScaleIndex(animationTime);
//         int p1Index          = p0Index + 1;
//         float scaleFactor    = GetScaleFactor(m_scales[p0Index].timeStamp,
//                                            m_scales[p1Index].timeStamp,
//                                            animationTime);
//         glm::vec3 finalScale = glm::mix(m_scales[p0Index].scale, m_scales[p1Index].scale, scaleFactor);
//         return glm::scale(glm::mat4(1.0f), finalScale);
//     }

//     float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
//     {
//         float scaleFactor  = 0.0f;
//         float midWayLength = animationTime - lastTimeStamp;
//         float framesDiff   = nextTimeStamp - lastTimeStamp;
//         scaleFactor        = midWayLength / framesDiff;
//         return scaleFactor;
//     }

// } // namespace Yutrel