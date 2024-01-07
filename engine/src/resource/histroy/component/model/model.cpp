
// #include "yutrel_pch.hpp"

// #include "model.hpp"

// #include "core/application/application.hpp"
// #include "function/render/renderer.hpp"
// #include "resource/component/animation/animation.hpp"

// #include "assimp/GltfMaterial.h"
// #include "assimp/color4.h"
// #include "glm/detail/qualifier.hpp"
// #include <algorithm>
// #include <assimp/material.h>
// #include <assimp/pbrmaterial.h>
// #include <assimp/types.h>
// #include <glm/fwd.hpp>

// #include <stdint.h>
// #include <string>
// #include <vector>

// namespace Yutrel
// {
//     Ref<Model> Model::Create(std::string const& path, bool is_animate)
//     {
//         return CreateRef<Model>(path, is_animate);
//     }

//     Model::Model(std::string const& path, bool is_animate)
//         : is_animate(is_animate), m_path(path)
//     {
//         LOG_INFO("Loading Model from: {0}", path);

//         unsigned int import_options = aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FindInvalidData | aiProcess_ValidateDataStructure | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights;

//         // for static models, let Assimp pre-transform all vertices for us (will lose the hierarchy)
//         if (!is_animate)
//         {
//             import_options |= aiProcess_PreTransformVertices;
//         }

//         Assimp::Importer importer;
//         importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4); // stick to "4 bones per vertex" rule
//         m_ai_scene = importer.ReadFile(path, import_options);

//         // check for errors
//         if (!m_ai_scene || m_ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_ai_scene->mRootNode) // if is Not Zero
//         {
//             LOG_ERROR("ASSIMP:{0}", importer.GetErrorString());
//             YUTREL_ASSERT(false, "Assimp");
//             return;
//         }

//         // process ASSIMP's root node recursively
//         ProcessNode(m_ai_scene->mRootNode);

//         if (is_animate)
//         {
//             // YUTREL_ASSERT(n_bones == cnt, "Corrupted data: duplicate or missing bones!");
//             YUTREL_ASSERT(m_bone_num <= 150, "Animation can only support up to 150 bones!");
//         }

//         m_ai_scene = nullptr;

//         LOG_TRACE("Generating model loading report...... (for reference)");
//         LOG_TRACE("-----------------------------------------------------");
//         LOG_TRACE("total # of meshes:     {0}", m_meshes.size());
//         LOG_TRACE("total # of vertices:   {0:.2f}k ({1})", m_vertex_num * 0.001f, m_vertex_num);
//         LOG_TRACE("total # of triangles:  {0:.2f}k ({1})", m_face_num * 0.001f, m_face_num);
//         LOG_TRACE("-----------------------------------------------------");
//     }

//     void Model::ProcessNode(aiNode* ai_node)
//     {
//         for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
//         {
//             // 根据索引找到对应的mesh数据
//             aiMesh* mesh = m_ai_scene->mMeshes[ai_node->mMeshes[i]];
//             ProcessMesh(mesh);
//         }
//         for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
//         {
//             ProcessNode(ai_node->mChildren[i]);
//         }
//     }

//     void Model::ProcessMesh(aiMesh* ai_mesh)
//     {
//         std::vector<Vertex> vertices;
//         std::vector<uint32_t> indices;
//         // std::vector<Material*> materials;

//         // 顶点属性
//         for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
//         {
//             Vertex vertex;
//             // SetVertexBoneDataToDefault(vertex);

//             // 顶点位置
//             if (ai_mesh->HasPositions())
//             {
//                 auto ai_position = ai_mesh->mVertices[i];
//                 vertex.position  = glm::vec3(ai_position.x, ai_position.y, ai_position.z);
//             }
//             // 法向量
//             if (ai_mesh->HasNormals())
//             {
//                 auto ai_normal = ai_mesh->mNormals[i];
//                 vertex.normal  = glm::vec3(ai_normal.x, ai_normal.y, ai_normal.z);
//             }
//             // 切线和副切线
//             if (ai_mesh->HasTangentsAndBitangents())
//             {
//                 auto ai_tangent   = ai_mesh->mTangents[i];
//                 vertex.tangent    = glm::vec3(ai_tangent.x, ai_tangent.y, ai_tangent.z);
//                 auto ai_bitangent = ai_mesh->mBitangents[i];
//                 vertex.bitangent  = glm::vec3(ai_bitangent.x, ai_bitangent.y, ai_bitangent.z);
//             }
//             // 纹理坐标
//             if (ai_mesh->HasTextureCoords(0))
//             {
//                 auto ai_uv = ai_mesh->mTextureCoords[0][i];
//                 vertex.uv  = glm::vec2(ai_uv.x, ai_uv.y);
//             }

//             vertices.emplace_back(vertex);
//             m_vertex_num++;
//         }

//         // 索引
//         for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
//         {
//             aiFace ai_face = ai_mesh->mFaces[i];
//             for (unsigned int j = 0; j < ai_face.mNumIndices; j++)
//             {
//                 indices.emplace_back(ai_face.mIndices[j]);
//             }
//             m_face_num++;
//         }

//         // 骨骼
//         if (is_animate)
//         {
//             ExtractBoneWeightForVertices(vertices, ai_mesh);
//         }

//         // Mesh
//         auto mesh = Mesh::Create(vertices, indices);
//         m_meshes.emplace_back(mesh);

//         // 材质
//         aiMaterial* ai_material = m_ai_scene->mMaterials[ai_mesh->mMaterialIndex];
//         ProcessMaterial(ai_material, mesh);
//     }

//     void Model::ProcessMaterial(aiMaterial* ai_material, const Ref<Mesh> mesh)
//     {
//         YUTREL_ASSERT(ai_material != nullptr, "Corrupted assimp data: material is nullptr!");

//         aiString name;
//         if (ai_material->Get(AI_MATKEY_NAME, name) != AI_SUCCESS)
//         {
//             LOG_ERROR("Unable to load mesh's material");
//             return;
//         }

//         std::string matkey{name.C_Str()};

//         // 设置mesh的材质名
//         mesh->SetMaterialName(matkey);
//     }

//     void Model::SetMaterial(const std::string& name, Ref<Material> material)
//     {
//         m_materials.insert_or_assign(name, material);
//     }

//     void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh)
//     {
//         for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
//         {
//             int boneID           = -1;
//             std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
//             if (m_bone_info.find(boneName) == m_bone_info.end())
//             {
//                 BoneInfo newBoneInfo;
//                 newBoneInfo.id        = m_bone_num;
//                 newBoneInfo.offset    = AssimpMat2GLM(mesh->mBones[boneIndex]->mOffsetMatrix);
//                 m_bone_info[boneName] = newBoneInfo;
//                 boneID                = m_bone_num;
//                 m_bone_num++;
//             }
//             else
//             {
//                 boneID = m_bone_info[boneName].id;
//             }

//             YUTREL_ASSERT(boneID != -1, "Invalid bone, cannot find a match in the nodes hierarchy!");
//             auto weights   = mesh->mBones[boneIndex]->mWeights;
//             int numWeights = mesh->mBones[boneIndex]->mNumWeights;

//             for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
//             {
//                 int vertexId = weights[weightIndex].mVertexId;
//                 float weight = weights[weightIndex].mWeight;
//                 YUTREL_ASSERT(vertexId < vertices.size(), "Vertex id out of bound!");
//                 SetVertexBoneData(vertices[vertexId], boneID, weight);
//             }
//         }
//     }

//     void Model::SetVertexBoneDataToDefault(Vertex& vertex)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             vertex.bone_ids[i]     = -1;
//             vertex.bone_weights[i] = 0.0f;
//         }
//     }

//     void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
//     {
//         for (int i = 0; i < 4; i++)
//         {
//             if (vertex.bone_ids[i] < 0)
//             {
//                 vertex.bone_weights[i] = weight;
//                 vertex.bone_ids[i]     = boneID;
//                 break;
//             }
//         }
//     }

//     void Model::AttachMotion(const std::string& filepath)
//     {
//         if (!is_animate)
//         {
//             LOG_ERROR("Cannot attach animation to the model, model must be animated");
//             return;
//         }

//         const unsigned int import_options = 0 | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FindInvalidData | aiProcess_ValidateDataStructure | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights
//             // | aiProcess_PreTransformVertices  // this flag must be disabled to load animation
//             ;

//         Assimp::Importer importer;
//         importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
//         importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, true);

//         LOG_TRACE("Start loading animation from: {0}", filepath);
//         const aiScene* ai_scene = importer.ReadFile(filepath, import_options);

//         // note that we don't need to check `scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE`
//         // here because the file can contain animations ONLY (without vertices, meshes)
//         // in which case the scene will be incomplete but the animation is still valid

//         if (!ai_scene || ai_scene->mRootNode == nullptr)
//         {
//             LOG_ERROR("Failed to import animation: {0}", filepath);
//             LOG_ERROR("Assimp error: {0}", importer.GetErrorString());
//             return;
//         }

//         m_animation = Animation::Create(ai_scene, shared_from_this());
//     }

//     void Model::SetPBRMaterial()
//     {
//         LOG_INFO("Load PBR Material from {0}", m_path);

//         Assimp::Importer importer;
//         importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4); // stick to "4 bones per vertex" rule
//         m_ai_scene = importer.ReadFile(m_path, 0);

//         ProcessPBRNode(m_ai_scene->mRootNode);
//     }

//     void Model::ProcessPBRNode(aiNode* ai_node)
//     {
//         for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
//         {
//             aiMesh* mesh = m_ai_scene->mMeshes[ai_node->mMeshes[i]];
//             ProcessPBRMesh(mesh);
//         }
//         for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
//         {
//             ProcessPBRNode(ai_node->mChildren[i]);
//         }
//     }

//     void Model::ProcessPBRMesh(aiMesh* ai_mesh)
//     {
//         // 材质
//         aiMaterial* ai_material = m_ai_scene->mMaterials[ai_mesh->mMaterialIndex];

//         YUTREL_ASSERT(ai_material != nullptr, "Corrupted assimp data: material is nullptr!");

//         aiString name;
//         if (ai_material->Get(AI_MATKEY_NAME, name) != AI_SUCCESS)
//         {
//             LOG_ERROR("Unable to load mesh's material");
//             return;
//         }

//         std::string matkey{name.C_Str()};
//         auto directory = m_path.substr(0, m_path.find_last_of('\\') + 1);

//         auto material = Material::Create();

//         // base color
//         if (ai_material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
//         {
//             aiString ai_path;
//             ai_material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &ai_path);
//             auto path            = ai_path.C_Str();
//             Ref<Texture> texture = nullptr;
//             if (m_texture_loaded.count(path))
//             {
//                 texture = m_texture_loaded[path];
//             }
//             else
//             {
//                 texture                = Texture::Create(directory + path);
//                 m_texture_loaded[path] = texture;
//             }
//             material->SetUniform("u_albedo_tex_toggle", 1.0f);
//             material->SetTexture(2, texture);
//         }
//         else
//         {
//             material->SetUniform("u_albedo_tex_toggle", 0.0f);
//         }
//         // 始终设置albedo值
//         aiColor4D ai_color;
//         ai_material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, ai_color);
//         material->SetUniform("u_albedo", glm::vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a));
//         aiString alphaMode;
//         if (ai_material->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode) == AI_SUCCESS)
//         {
//             // 如果"alphaMode"为"BLEND"，则物体是半透明的
//             if (alphaMode == aiString("BLEND"))
//             {
//                 material->SetUniform("u_blend_toggle", 1.0f);
//             }
//             else
//             {
//                 material->SetUniform("u_blend_toggle", 0.0f);
//             }
//         }

//         // normal
//         if (ai_material->GetTextureCount(aiTextureType_NORMALS) > 0)
//         {
//             aiString ai_path;
//             ai_material->GetTexture(aiTextureType_NORMALS, 0, &ai_path);
//             auto path            = ai_path.C_Str();
//             Ref<Texture> texture = nullptr;
//             if (m_texture_loaded.count(path))
//             {
//                 texture = m_texture_loaded[path];
//             }
//             else
//             {
//                 texture                = Texture::Create(directory + path);
//                 m_texture_loaded[path] = texture;
//             }
//             material->SetUniform("u_normal_tex_toggle", 1.0f);
//             material->SetTexture(1, texture);
//         }
//         else
//         {
//             material->SetUniform("u_normal_tex_toggle", 0.0f);
//         }

//         // matellic & roughness
//         if (ai_material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
//         {
//             aiString ai_path;
//             ai_material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &ai_path);
//             auto path            = ai_path.C_Str();
//             Ref<Texture> texture = nullptr;
//             if (m_texture_loaded.count(path))
//             {
//                 texture = m_texture_loaded[path];
//             }
//             else
//             {
//                 texture                = Texture::Create(directory + path);
//                 m_texture_loaded[path] = texture;
//             }
//             material->SetUniform("u_metallic_roughness_tex_toggle", 1.0f);
//             material->SetTexture(3, texture);
//         }
//         else
//         {
//             material->SetUniform("u_metallic_roughness_tex_toggle", 0.0f);

//             float value;
//             if (ai_material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, value) == AI_SUCCESS)
//             {
//                 material->SetUniform("u_metallic", value);
//             }
//             else
//             {
//                 material->SetUniform("u_metallic", 0.0f);
//             }

//             if (ai_material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
//             {
//                 material->SetUniform("u_roughness", value);
//             }
//             else
//             {
//                 material->SetUniform("u_roughness", 0.0f);
//             }
//         }

//         // emissive
//         if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, ai_color) == AI_SUCCESS)
//         {
//             material->SetUniform("u_emissive", glm::vec3(ai_color.r, ai_color.g, ai_color.b));
//         }

//         m_materials[matkey] = material;
//     }

//     void Model::Draw()
//     {
//         for (auto& mesh : m_meshes)
//         {
//             Resources resource = Resources(Application::GetInstance().GetWorld());
//             auto renderer      = resource.Get<Ref<Renderer>>();
//             // renderer->DrawIndexed(mesh->m_VAO);
//         }
//     }

//     void Model::Draw(Ref<Shader> shader, bool is_material)
//     {
//         for (auto& mesh : m_meshes)
//         {
//             if (is_material)
//             {
//                 auto material = m_materials[mesh->GetMaterialName()];
//                 material->Bind(shader);
//             }

//             Resources resource = Resources(Application::GetInstance().GetWorld());
//             auto renderer      = resource.Get<Ref<Renderer>>();
//             renderer->DrawIndexed(mesh->m_vertex_array);
//         }
//     }

//     void Model::Draw(Ref<Shader> shader, Ref<CameraController> camera_controller, bool is_material, int blend_mode, Transform tranform)
//     {
//         for (auto& mesh : m_meshes)
//         {
//             // 视锥体裁剪
//             if (!mesh->m_AABB->IsOnFrustum(Frustum(camera_controller), tranform))
//             {
//                 continue;
//             }

//             // 判断是否为透明
//             if (is_material)
//             {
//                 auto material = m_materials[mesh->GetMaterialName()];

//                 if (material != nullptr)
//                 {
//                     if (
//                         (blend_mode == 1 && material->IsBlend()) ||
//                         (blend_mode == 2 && !material->IsBlend()))
//                     {
//                         continue;
//                     }

//                     material->Bind(shader);
//                 }
//             }

//             Resources resource = Resources(Application::GetInstance().GetWorld());
//             auto renderer      = resource.Get<Ref<Renderer>>();
//             renderer->DrawIndexed(mesh->m_vertex_array);
//         }
//     }

// } // namespace Yutrel