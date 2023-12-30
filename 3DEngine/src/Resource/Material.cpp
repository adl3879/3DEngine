#include "Material.h"

#include "AssetManager.h"
#include "Texture.h"
#include "MaterialSerializer.h"
#include "ThumbnailManager.h"

#include "Log.h"

namespace Engine
{
Material::Material()
{
    std::fill(m_TextureHandles.begin(), m_TextureHandles.end(), 0);
    std::fill(m_TexturePaths.begin(), m_TexturePaths.end(), "");
}

void Material::Init(const std::string &name, AssetHandle albedo, AssetHandle normal, AssetHandle metallic,
                    AssetHandle roughness, AssetHandle ao)
{
    Name = name;

    m_TextureHandles[ALBEDO] = albedo;
    m_TextureHandles[METALLIC] = metallic;
    m_TextureHandles[NORMAL] = normal;
    m_TextureHandles[ROUGHNESS] = roughness;
    m_TextureHandles[AO] = ao;
}

void Material::Init(const std::string &name, const std::string &albedo, const std::string &normal,
                    const std::string &metallic, const std::string &roughness, const std::string &ao)
{
    Name = name;

    m_TexturePaths[ALBEDO] = albedo;
    m_TexturePaths[METALLIC] = metallic;
    m_TexturePaths[NORMAL] = normal;
    m_TexturePaths[ROUGHNESS] = roughness;
    m_TexturePaths[AO] = ao;
}

void Material::Init(std::string_view name, const glm::vec3 &albedo, const glm::vec3 &normal, float metallic,
                    float roughness, float ao, float alpha)
{
    Name = name;

    m_MaterialParam.Albedo = albedo;
    m_MaterialParam.AO = ao;
    m_MaterialParam.Normal = normal;
    m_MaterialParam.Metallic = metallic;
    m_MaterialParam.Roughness = roughness;
}

void Material::Bind(Shader *shader) noexcept
{
    shader->Bind();

    BindTextures();

    shader->SetUniform1i("irradianceMap", 0);
    shader->SetUniform1i("prefilterMap", 1);
    shader->SetUniform1i("brdfLUT", 2);

    shader->SetUniform1i("albedoMap", 3);
    shader->SetUniform1i("normalMap", 4);
    shader->SetUniform1i("metallicMap", 5);
    shader->SetUniform1i("roughnessMap", 6);
    shader->SetUniform1i("aoMap", 7);

    shader->SetUniform3f("albedoParam", m_MaterialParam.Albedo);
    shader->SetUniform1f("metallicParam", m_MaterialParam.Metallic);
    shader->SetUniform1f("aoParam", m_MaterialParam.AO);
    shader->SetUniform1f("roughnessParam", m_MaterialParam.Roughness);
    shader->SetUniform1f("emissiveParam", m_MaterialParam.Emissive);
}

void Material::Unbind() const noexcept {}

bool Material::Reset(ParameterType type)
{
    if (IsDefault) return false;

    MaterialRef mat = std::make_shared<Material>();
    MaterialSerializer serializer(mat);
    auto filePath = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;
    serializer.Deserialize(filePath);

    if (type == ParameterType::ALBEDO)
        m_MaterialParam.Albedo = mat->GetMaterialData().Albedo;
    else if (type == ParameterType::AO)
        m_MaterialParam.AO = mat->GetMaterialData().AO;
    else if (type == ParameterType::METALLIC)
        m_MaterialParam.Metallic = mat->GetMaterialData().Metallic;
    else if (type == ParameterType::NORMAL)
    {
        m_MaterialParam.Normal = mat->GetMaterialData().Normal;
        m_UseNormalMap = mat->GetUseNormalMap();
    }
    else if (type == ParameterType::ROUGHNESS)
        m_MaterialParam.Roughness = mat->GetMaterialData().Roughness;

    return true;
}

int Material::HasMaterialMap(ParameterType type) const
{
    return AssetManager::IsAssetHandleValid(m_TextureHandles[type]);
}

void Material::SetTexture(ParameterType type, AssetHandle textureHandle) { m_TextureHandles[type] = textureHandle; }

void Material::SetTexturePath(ParameterType type, const std::string &path) { m_TexturePaths[type] = path; }

void Material::SetMaterialParam(ParameterType type, std::any param)
{
    if (type == ParameterType::ALBEDO)
        m_MaterialParam.Albedo = std::any_cast<glm::vec3>(param);
    else if (type == ParameterType::AO)
        m_MaterialParam.AO = std::any_cast<float>(param);
    else if (type == ParameterType::METALLIC)
        m_MaterialParam.Metallic = std::any_cast<float>(param);
    else if (type == ParameterType::NORMAL)
        m_MaterialParam.Normal = std::any_cast<glm::vec3>(param);
    else if (type == ParameterType::ROUGHNESS)
        m_MaterialParam.Roughness = std::any_cast<float>(param);
}

std::string Material::GetTexturePath(ParameterType type) const
{
    if (!m_TexturePaths[type].empty())
        return m_TexturePaths[type];
    else
        return "No Texture";
}

void Material::BindTextures()
{
    for (int i = 0; i < m_TextureHandles.size(); i++)
    {
        if (!AssetManager::IsAssetHandleValid(m_TextureHandles[i]))
        {
            // only runs once if there is a valid texture
            auto texture = AssetManager::GetAsset<Texture2D>(TexturesDirectory / m_TexturePaths[i]);
            if (texture)
            {
                texture->Bind(i + 3);
                m_TextureHandles[i] = texture->Handle;

                // serialize the material
                MaterialSerializer serializer(std::make_shared<Material>(*this));
                auto filePath = Project::GetAssetDirectory() / AssetManager::GetRegistry()[Handle].FilePath;
                serializer.Serialize(filePath);

                //                AssetManager::UnloadAsset(Handle);
                //                ThumbnailManager::Get().MarkThumbnailAsDirty(Handle);
            }
        }
        else
            AssetManager::GetAsset<Texture2D>(m_TextureHandles[i])->Bind(i + 3);
    }
}

Texture2DRef Material::GetTexture(ParameterType type) const
{
    return AssetManager::GetAsset<Texture2D>(m_TextureHandles[type]);
}
} // namespace Engine
