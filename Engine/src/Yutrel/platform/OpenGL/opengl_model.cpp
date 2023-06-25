#include "yutrel_pch.h"

#include "opengl_model.h"

#include "Yutrel/function/global/global_context.h"
#include "Yutrel/platform/OpenGL/opengl_texture.h"

#include "assimp/material.h"

namespace Yutrel
{
    OpenGLModel::OpenGLModel(std::string const &path)
    {
        loadModel(path);
    }

    void OpenGLModel::loadModel(std::string const &path)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            LOG_ERROR("ASSIMP:{0}", importer.GetErrorString());
            assert(false);
            return;
        }
        // retrieve the directory path of the filepath
        m_directory = path.substr(0, path.find_last_of('/') + 1);

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    void OpenGLModel::Render()
    {
        for (auto &mesh : m_meshes)
            mesh->Render();
    }

    void OpenGLModel::processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // 根据索引找到对应的mesh数据
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.emplace_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    std::shared_ptr<Mesh> OpenGLModel::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
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
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.emplace_back(face.mIndices[j]);
            }
        }

        // 这一部分后面要更改
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. diffuse maps
        std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh::Create(vertices, indices, textures);
    }

    std::vector<std::shared_ptr<Texture>> OpenGLModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
    {
        std::vector<std::shared_ptr<Texture>> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString aistr;
            mat->GetTexture(type, i, &aistr);
            std::string str = aistr.C_Str();

            str = m_directory + str;

            if (texture_loaded.count(str))
            {
                textures.emplace_back(texture_loaded[str]);
            }
            else
            {
                std::shared_ptr<Texture2D> texture = Texture2D::Create(str);
                texture_loaded[str]                = texture;
                textures.emplace_back(texture);
            }
        }
        return textures;
    }
} // namespace Yutrel