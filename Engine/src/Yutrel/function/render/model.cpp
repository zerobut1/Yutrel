
#include "yutrel_pch.hpp"

#include "model.hpp"

#include "Yutrel/function/render/renderer.hpp"

#include <assimp/material.h>
#include <assimp/types.h>
#include <glm/fwd.hpp>

#include <string>
#include <vector>

namespace Yutrel
{
    Model* Model::Create(std::string const& path)
    {
        return new Model(path);
    }

    Model::Model(std::string const& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
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
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // 根据索引找到对应的mesh数据
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.emplace_back(ProcessMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        std::vector<Material*> materials;

        // 顶点属性
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            // 顶点位置
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

            if (mesh->HasTangentsAndBitangents())
            {
                vertices.emplace_back(mesh->mTangents[i].x);
                vertices.emplace_back(mesh->mTangents[i].y);
                vertices.emplace_back(mesh->mTangents[i].z);
                vertices.emplace_back(mesh->mBitangents[i].x);
                vertices.emplace_back(mesh->mBitangents[i].y);
                vertices.emplace_back(mesh->mBitangents[i].z);
            }
            else
            {
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
                vertices.emplace_back(0.0f);
            }

            // 纹理坐标
            if (mesh->HasTextureCoords(0))
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

        // 材质
        aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];

        materials.emplace_back(LoadMaterial(ai_material, "Ambient", aiTextureType_AMBIENT));
        materials.emplace_back(LoadMaterial(ai_material, "Diffuse", aiTextureType_DIFFUSE));
        materials.emplace_back(LoadMaterial(ai_material, "Specular", aiTextureType_SPECULAR));
        materials.emplace_back(LoadMaterial(ai_material, "Normals", aiTextureType_HEIGHT));//Assimp加载obj用HEIGHT读取Normal 很神秘

        return Mesh::Create(vertices, indices, materials);
    }

    Material* Model::LoadMaterial(aiMaterial* mat, std::string type, aiTextureType ai_type)
    {
        auto material  = new Material();
        material->type = type;
        aiColor3D ai_color;

        if (mat->GetTextureCount(ai_type) == 0)
        {
            material->is_texture = false;
            switch (ai_type)
            {
            case aiTextureType_AMBIENT:
                mat->Get(AI_MATKEY_COLOR_AMBIENT, ai_color);
                material->color = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
                break;
            case aiTextureType_DIFFUSE:
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color);
                material->color = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
                break;
            case aiTextureType_SPECULAR:
                mat->Get(AI_MATKEY_COLOR_SPECULAR, ai_color);
                material->color = glm::vec3(ai_color.r, ai_color.g, ai_color.b);
                break;
            default:
                break;
            }

            material->textures.emplace_back("");
        }
        else
        {
            material->is_texture = true;
            material->textures   = LoadMaterialTextures(mat, ai_type);
        }

        return material;
    }

    std::vector<std::string> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
    {
        std::vector<std::string> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString aistr;
            mat->GetTexture(type, i, &aistr);
            std::string str = aistr.C_Str();

            str = m_directory + str;

            if (!m_texture_loaded.count(str))
            {
                Texture2D* texture    = Texture2D::Create(str);
                m_texture_loaded[str] = texture;
            }
            textures.emplace_back(str);
        }
        return textures;
    }

    void Model::Draw()
    {
        for (auto& mesh : m_meshes)
        {
            Resources resource = Resources(Application::Get().GetWorld());
            auto renderer      = resource.Get<Renderer*>();
            renderer->DrawIndexed(mesh->m_VAO);
        }
    }

    void Model::Draw(Shader* shader)
    {
        for (auto& mesh : m_meshes)
        {
            // mesh->Draw(shader);
            auto materials = mesh->m_materials;
            for (unsigned int i = 0; i < materials.size(); i++)
            {
                shader->setFloat("u_" + materials[i]->type + "TexToggle", materials[i]->is_texture);
                shader->setFloat3("u_" + materials[i]->type + "Color", materials[i]->color);
                if (m_texture_loaded.count(materials[i]->textures[0]))
                {
                    m_texture_loaded[materials[i]->textures[0]]->Bind(i);
                }
            }

            Resources resource = Resources(Application::Get().GetWorld());
            auto renderer      = resource.Get<Renderer*>();
            renderer->DrawIndexed(mesh->m_VAO);
        }
    }

} // namespace Yutrel