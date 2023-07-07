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

<<<<<<< HEAD
<<<<<<< HEAD
        virtual void Render() = 0;
=======
        // std::vector<Mesh*> GetMeshes() const { return m_meshes; }
>>>>>>> b8bc66d (修改model和mesh)
=======
        // std::vector<Mesh*> GetMeshes() const { return m_meshes; }
>>>>>>> 41d5f2d913b1cd99d802037860fdc7eb579aecd9

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