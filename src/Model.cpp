#include "Model.h"

#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char *file) : m_File(file)
{
    Assimp::Importer importer;
    m_Scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    LoadMesh();
}

void Model::Draw(Shader &shader, Camera &camera)
{
    for (unsigned int i = 0; i < m_Meshes.size(); i++)
    {
        m_Meshes[i].Mesh::Draw(shader, camera);
    }
}

std::vector<unsigned int> Model::GetIndices(const aiMesh *mesh)
{
    std::vector<unsigned int> indices;

    for (size_t j = 0; j < mesh->mNumFaces; ++j)
    {
        const aiFace &face = mesh->mFaces[j];

        for (size_t k = 0; k < face.mNumIndices; ++k)
            indices.push_back(face.mIndices[k]);
    }

    return indices;
}

std::vector<Texture> Model::GetTextures()
{
    std::vector<Texture> textures;
    std::string fileStr = std::string(m_File);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

    for (size_t i = 0; i < m_Scene->mNumMaterials; ++i)
    {
        const aiMaterial *material = m_Scene->mMaterials[i];
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;
            auto success = material->GetTexture(aiTextureType_DIFFUSE, i, &path);

            m_HasTexture = true;
            bool skip = false;
            for (size_t j = 0; j < m_TexturesLoaded.size(); j++)
            {
                std::string texturePath = fileDirectory + path.C_Str();
                if (std::strcmp(m_TexturesLoaded[j].GetPath().data(), texturePath.c_str()) == 0)
                {
                    textures.push_back(m_TexturesLoaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip && success == AI_SUCCESS)
            {
                Texture texture(fileDirectory + path.C_Str(), "diffuse", i);
                textures.push_back(texture);
            }
        }
        else if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString path;
            auto success = material->GetTexture(aiTextureType_SPECULAR, i, &path);

            m_HasTexture = true;
            bool skip = false;

            for (size_t j = 0; j < m_TexturesLoaded.size(); j++)
            {
                std::string texturePath = fileDirectory + path.C_Str();
                if (std::strcmp(m_TexturesLoaded[j].GetPath().data(), texturePath.c_str()) == 0)
                {
                    textures.push_back(m_TexturesLoaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip && success == AI_SUCCESS)
            {
                Texture texture(fileDirectory + path.C_Str(), "specular", i);
                textures.push_back(texture);
            }
        }
    }

    return textures;
}

std::vector<Vertex> Model::AssembleVertices(const aiMesh *mesh)
{
    std::vector<Vertex> vertices;

    for (size_t j = 0; j < mesh->mNumVertices; ++j)
    {
        auto vertex = Vertex{
            .Position = glm::vec3{mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z},
            .Normal = glm::vec3{mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z},
            .Color = glm::vec3{1.0f, 1.0f, 1.0f},
            .Texture = m_HasTexture ? glm::vec2{mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y}
                                    : glm::vec2{0.0f, 0.0f},
        };
        vertices.push_back(vertex);
    }

    return vertices;
}

void Model::LoadMesh()
{
    for (size_t i = 0; i < m_Scene->mNumMeshes; ++i)
    {
        const aiMesh *mesh = m_Scene->mMeshes[i];

        auto textures = GetTextures();
        auto vertices = AssembleVertices(mesh);
        auto indices = GetIndices(mesh);

        m_Meshes.push_back(Mesh{vertices, indices, textures});
    }
}
