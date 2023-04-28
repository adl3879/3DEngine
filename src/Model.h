#pragma once

#include <json/json.h>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"

using json = nlohmann::json;

class Model
{
  public:
    Model() = default;
    Model(const char *file);

    void Draw(Shader &shader, Camera &camera);

  private:
    std::string ReadFile(const char *file);
    std::vector<unsigned char> GetData();

    std::vector<float> GetFloats(json accessor);
    std::vector<unsigned int> GetIndices(json accessor);
    std::vector<Texture> GetTextures();
    std::vector<Vertex> AssembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals,
                                         std::vector<glm::vec2> texUVs);
    void LoadMesh(unsigned int indMesh);

    std::vector<glm::vec2> GroupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> GroupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> GroupFloatsVec4(std::vector<float> floatVec);

    void TraverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4{1.0f});

  private:
    const char *m_File;
    std::vector<unsigned char> m_Data;
    json m_JSON;

    std::vector<Mesh> m_Meshes;
    std::vector<glm::vec3> m_TranslationsMeshes;
    std::vector<glm::quat> m_RotationsMeshes;
    std::vector<glm::vec3> m_ScalesMeshes;
    std::vector<glm::mat4> m_MatricesMeshes;

    std::vector<std::string> m_LoadedTexName;
    std::vector<Texture> m_LoadedTex;
};
