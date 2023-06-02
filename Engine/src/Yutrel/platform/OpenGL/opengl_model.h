#pragma once

#include "Yutrel/function/render/model.h"
#include "Yutrel/platform/OpenGL/opengl_mesh.h"

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
        OpenGLModel(std::string const &path);

        virtual void Draw() override;

    private:
        void loadModel(std::string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> texture_loaded;
    };
} // namespace Yutrel
