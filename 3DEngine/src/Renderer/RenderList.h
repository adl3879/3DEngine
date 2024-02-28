#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>

#include "StaticMesh.h"
#include "Shaders/ShaderManager.h"
#include "Material.h"
#include "Light.h"
#include "AssetManager.h"
#include "Log.h"
#include "SkinnedMesh.h"

namespace Engine
{
struct RenderMesh
{
    StaticMeshRef Mesh;
    glm::mat4 Transform;
    int32_t EntityId;
};

struct SkinnedRenderMesh
{
	SkinnedMeshRef Mesh;
	glm::mat4 Transform;
	int32_t EntityId;
};

using RenderListMap = std::unordered_map<MaterialRef, std::vector<RenderMesh>>;
using SkinnedRenderListMap = std::unordered_map<MaterialRef, std::vector<SkinnedRenderMesh>>;

class RenderList
{
public:
    RenderList() { m_RenderList = RenderListMap(); }

    void AddToRenderList(StaticMeshRef mesh, MaterialRef mat, glm::mat4 &transform, const int32_t entityId = -1)
    {
        if (mat)
        {
            if (m_RenderList.find(mat) == m_RenderList.end())
            {
                m_RenderList[mat] = std::vector<RenderMesh>();
            }
            m_RenderList[mat].push_back({std::move(mesh), transform, entityId});
        }
    }

	void AddToRenderList(SkinnedMeshRef mesh, MaterialRef mat, glm::mat4& transform, const int32_t entityId = -1) 
	{
		if (mat)
		{
			if (m_SkinnedRenderList.find(mat) == m_SkinnedRenderList.end())
			{
				m_SkinnedRenderList[mat] = std::vector<SkinnedRenderMesh>();
			}
			m_SkinnedRenderList[mat].push_back({std::move(mesh), transform, entityId});
		}
	}

    void Flush(Shader *shader, bool depthOnly = false)
    {
        shader->Bind();
        const uint32_t entityIdUniformLocation = shader->FindUniformLocation("entityId");
        const uint32_t modelMatrixUniformLocation = shader->FindUniformLocation("model");
        for (auto &i : m_RenderList)
        {
            if (!depthOnly) i.first->Bind(shader);
            for (auto &m : i.second)
            {
                shader->SetUniformMatrix4fv(modelMatrixUniformLocation, m.Transform);
                shader->SetUniform1i(entityIdUniformLocation, m.EntityId + 1);

                const auto mat = i.first;
                shader->SetUniform1i("hasAlbedoMap", mat->HasMaterialMap(ParameterType::ALBEDO));
                shader->SetUniform1i("hasNormalMap", mat->HasMaterialMap(ParameterType::NORMAL) & mat->GetUseNormalMap());
                shader->SetUniform1i("hasMetallicMap", mat->HasMaterialMap(ParameterType::METALLIC));
                shader->SetUniform1i("hasRoughnessMap", mat->HasMaterialMap(ParameterType::ROUGHNESS));
                shader->SetUniform1i("hasAoMap", mat->HasMaterialMap(ParameterType::AO));

                m.Mesh->Draw(shader, true);
            }
        }
        m_RenderList.clear();

		for (auto& i : m_SkinnedRenderList)
        {
			if (!depthOnly) i.first->Bind(shader);
			for (auto& m : i.second)
			{
				shader->SetUniformMatrix4fv(modelMatrixUniformLocation, m.Transform);
				shader->SetUniform1i(entityIdUniformLocation, m.EntityId + 1);

				const auto mat = i.first;
				shader->SetUniform1i("hasAlbedoMap", mat->HasMaterialMap(ParameterType::ALBEDO));
				shader->SetUniform1i("hasNormalMap", mat->HasMaterialMap(ParameterType::NORMAL) & mat->GetUseNormalMap());
				shader->SetUniform1i("hasMetallicMap", mat->HasMaterialMap(ParameterType::METALLIC));
				shader->SetUniform1i("hasRoughnessMap", mat->HasMaterialMap(ParameterType::ROUGHNESS));
				shader->SetUniform1i("hasAoMap", mat->HasMaterialMap(ParameterType::AO));

				m.Mesh->Draw(shader, true);
			}
		}
		m_SkinnedRenderList.clear();
    }

private:
    RenderListMap m_RenderList;
	SkinnedRenderListMap m_SkinnedRenderList;
};
} // namespace Engine
