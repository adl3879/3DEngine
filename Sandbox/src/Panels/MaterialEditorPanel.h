#pragma once

#include "Asset.h"
#include "TextureImporter.h"

namespace Engine
{
class MaterialEditorPanel
{
  public:
    MaterialEditorPanel();
    ~MaterialEditorPanel() = default;

    void OnImGuiRender();

  public:
    static void OpenMaterialEditor(AssetHandle handle, bool isDefaultMaterial = false);

  private:
    Texture2DRef m_CheckerboardTexture;

    static AssetHandle s_MaterialHandle;
    static bool s_ShowMaterialEditor;
    static bool s_IsDefaultMaterial;
};
} // namespace Engine
