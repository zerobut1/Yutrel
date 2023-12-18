/**
 * 模型类
 * 一个模型包括他的所有mesh以及对应的材质
 */
#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "resource/component/camera/camera_controller.hpp"
#include "resource/component/model/material.hpp"
#include "resource/component/model/mesh.hpp"

#include "resource/asset/shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/glm.hpp>

#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Yutrel
{
    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        uint32_t id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;
    };

    class Animation;

    // 模型类
    class Model : public std::enable_shared_from_this<Model>
    {
    public:
        static Ref<Model> Create(std::string const& path, bool is_animate);

        Model(std::string const& path, bool is_animate);
        virtual ~Model() = default;

        void Draw();
        void Draw(Ref<Shader> shader, bool is_material = false);
        void Draw(Ref<Shader> shader, Ref<CameraController> camera, bool is_material, int blend_mode, Transform tranform);

        void SetMaterial(const std::string& name, Ref<Material> material);
        void SetPBRMaterial();

        void AttachMotion(const std::string& path);

        std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() { return m_bone_info; }
        uint32_t& GetBoneCount() { return m_bone_num; }
        Ref<Animation> GetAnimation() { return m_animation; }

    private:
        void ProcessNode(aiNode* ai_node);
        void ProcessMesh(aiMesh* ai_mesh);
        void ProcessMaterial(aiMaterial* ai_material, const Ref<Mesh> mesh);

        void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
        void SetVertexBoneDataToDefault(Vertex& vertex);
        void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

        void ProcessPBRNode(aiNode* ai_node);
        void ProcessPBRMesh(aiMesh* ai_mesh);

        static inline glm::mat4 AssimpMat2GLM(const aiMatrix4x4& m)
        {
            return glm::transpose(glm::make_mat4(&m.a1)); // aiMatrix4x4 is in row-major order so we transpose
        }

    private:
        const aiScene* m_ai_scene = nullptr;
        std::string m_path;

        uint32_t m_vertex_num = 0;
        uint32_t m_face_num   = 0;

        std::vector<Ref<Mesh>> m_meshes;
        std::unordered_map<std::string, Ref<Material>> m_materials;

        bool is_animate     = false;
        uint32_t m_bone_num = 0;
        std::unordered_map<std::string, BoneInfo> m_bone_info;
        Ref<Animation> m_animation;

        std::unordered_map<std::string, Ref<Texture>> m_texture_loaded;
    };
} // namespace Yutrel