#pragma once

#include "Yutrel/function/render/mesh.hpp"

#include "Yutrel/function/render/shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Yutrel
{

    struct Material
    {
        bool is_texture;
        glm::vec3 color = glm::vec3(1.0f);
        std::vector<std::string> textures;
        std::string type;
    };

    // 模型类
    class Model
    {
    public:
        static Model* Create(std::string const& path);

        Model(std::string const& path);
        virtual ~Model() = default;

        // std::vector<Mesh*> GetMeshes() const { return m_meshes; }

        void Draw();
        void Draw(Shader* shader);

    private:
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
        Material* LoadMaterial(aiMaterial* mat, std::string type, aiTextureType ai_type);
        std::vector<std::string> LoadMaterialTextures(aiMaterial* mat,
                                                      aiTextureType type);

    private:
        std::vector<Mesh*> m_meshes;
        std::unordered_map<std::string, Texture2D*> m_texture_loaded;
        std::string m_directory;
    };
} // namespace Yutrel