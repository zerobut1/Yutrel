#pragma once

#include "Yutrel/function/render/model.hpp"
#include "Yutrel/platform/OpenGL/opengl_mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stb_image.h>

namespace Yutrel
{
    class OpenGLModel : public Model
    {
    public:
        OpenGLModel(std::string const& path);

        virtual void Draw() override;

    private:
        void loadModel(std::string const& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh* processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    private:
        std::unordered_map<std::string, Texture2D*> texture_loaded;
    };
} // namespace Yutrel
