#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Buffer.h"
#include "Mesh.h"

class Model
{
  public:
    Model() = default;
    Model(const char *file);

    void Draw(Shader &shader, Camera &camera);

  private:
    std::vector<unsigned int> GetIndices(const aiMesh *mesh);
    std::vector<Vertex> AssembleVertices(const aiMesh *mesh);
    std::vector<Texture> GetTextures();

    void LoadMesh();

  private:
    const char *m_File;
    std::vector<unsigned char> m_Data;
    const aiScene *m_Scene;

    std::vector<Mesh> m_Meshes;
    bool m_HasTexture = false;
    std::vector<Texture> m_TexturesLoaded;
};
