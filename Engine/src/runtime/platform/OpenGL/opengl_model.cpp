#include "opengl_model.h"

#include "assimp/material.h"
#include "runtime/function/global/global_context.h"
#include "runtime/platform/OpenGL/opengl_texture.h"

#include <memory>
#include <string>

namespace Yutrel
{
    OpenGLModel::OpenGLModel(std::string const &path)
    {
        loadModel(path);
    }

    void OpenGLModel::loadModel(std::string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            LOG_ERROR("ASSIMP:{0}", importer.GetErrorString());
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    void OpenGLModel::Draw()
    {
        for (auto &mesh : meshes)
            mesh->Draw();
    }

    void OpenGLModel::processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            // 根据索引找到对应的mesh数据
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.emplace_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    std::shared_ptr<Mesh> OpenGLModel::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<float> vertices(0);
        std::vector<uint32_t> indices(0);
        std::vector<std::shared_ptr<Texture>> textures;

        // 顶点属性
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // 坐标
            vertices.emplace_back(mesh->mVertices[i].x);
            vertices.emplace_back(mesh->mVertices[i].y);
            vertices.emplace_back(mesh->mVertices[i].z);

            // 法向量
            if (mesh->HasNormals())
            {
                vertices.emplace_back(mesh->mNormals[i].x);
                vertices.emplace_back(mesh->mNormals[i].y);
                vertices.emplace_back(mesh->mNormals[i].z);
            }

            // 纹理坐标
            if (mesh->mTextureCoords[0])
            {
                vertices.emplace_back(mesh->mTextureCoords[0][i].x);
                vertices.emplace_back(mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
            }
        }

        // 索引
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        // process materials
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        /*
        // 2. specular maps
        std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        */

        // return a mesh object created from the extracted mesh data
        return Mesh::create(vertices, indices, textures);
    }

    std::vector<std::shared_ptr<Texture>> OpenGLModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
    {
        std::vector<std::shared_ptr<Texture>> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString aistr;
            mat->GetTexture(type, i, &aistr);
            std::string str = aistr.C_Str();

            str = "../Engine/asset/object/nanosuit/" + str;

            if (texture_loaded.count(str))
            {
                textures.emplace_back(texture_loaded[str]);
            }
            else
            {
                std::shared_ptr<Texture2D> texture = Texture2D::create(str);
                texture_loaded[str]                = texture;
                textures.emplace_back(texture);
            }
        }
        return textures;
    }
} // namespace Yutrel