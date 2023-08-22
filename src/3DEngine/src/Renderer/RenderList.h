#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shaders/ShaderManager.h"
#include "Material.h"

namespace Engine
{

struct RenderMesh
{
    MeshRef Mesh;
    glm::mat4 Transform;
    int32_t EntityId;
};

using RenderListMap = std::unordered_map<MaterialRef, std::vector<RenderMesh>>;

class RenderList
{
  public:
    RenderList()
    {
        //
        m_RenderList = RenderListMap();
    }

    void AddToRenderList(MeshRef mesh, const glm::mat4 &transform, const int32_t entityId = -1)
    {
        MaterialRef material = mesh->Material;
        if (m_RenderList.find(material) == m_RenderList.end())
        {
            m_RenderList[material] = std::vector<RenderMesh>();
        }

        m_RenderList[material].push_back({std::move(mesh), std::move(transform), entityId});
    }

    void Flush(Shader *shader, bool depthOnly = false)
    {
        shader->Bind();
        const uint32_t entityIdUniformLocation = shader->FindUniformLocation("u_EntityID");
        const uint32_t modelMatrixUniformLocation = shader->FindUniformLocation("u_Model");
        for (auto &i : m_RenderList)
        {
            if (!depthOnly) i.first->Bind(shader);

            for (auto &m : i.second)
            {
                // shader->SetUniformMatrix4fv(modelMatrixUniformLocation, m.transform);
                // shader->SetUniform1i(entityIdUniformLocation, m.entityId + 1);
                m.Mesh->Draw(shader, false);
            }
        }

        m_RenderList.clear();
    }

  private:
    RenderListMap m_RenderList;
};
} // namespace Engine