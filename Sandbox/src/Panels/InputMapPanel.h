#pragma once

namespace Engine
{
class InputMapPanel
{
  public:
    InputMapPanel() = default;
    ~InputMapPanel() = default;

    void OnImGuiRender();

    void SetOpen(bool isOpen) { m_IsOpen = isOpen; }

  private:
    bool m_IsOpen = false;
};
} // namespace Engine