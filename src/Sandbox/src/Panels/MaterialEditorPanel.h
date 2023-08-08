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
    static void OpenMaterialEditor(AssetHandle handle);

  private:
    Texture2DRef m_CheckerboardTexture;

    static AssetHandle s_MaterialHandle;
    static bool s_ShowMaterialEditor;
};
} // namespace Engine