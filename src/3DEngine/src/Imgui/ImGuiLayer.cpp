#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Engine
{
ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnUpdate(float deltaTime)
{
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    auto windowState = InputManager::Instance().GetWindowState();
    io.DisplaySize = ImVec2(windowState.Width, windowState.Height);
    io.FontGlobalScale = 1.8f;

    float time = (float)glfwGetTime();
    io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
    m_Time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    static bool show = true;
    ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::OnKeyPressed(InputKey key, bool isRepeat) {}

void ImGuiLayer::OnKeyReleased(InputKey key) {}

void ImGuiLayer::OnMouseButtonPressed(MouseButton button)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(button)] = true;
}

void ImGuiLayer::OnMouseButtonReleased(MouseButton button)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(button)] = false;
}

void ImGuiLayer::OnMouseMoved(double xPos, double yPos, double xOffset, double yOffset)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(xPos, yPos);
}

void ImGuiLayer::OnMouseScrolled(double xOffset, double yOffset)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += (float)xOffset;
    io.MouseWheel += (float)yOffset;
}

void ImGuiLayer::OnWindowResize(int width, int height)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void ImGuiLayer::Begin() {}

void ImGuiLayer::End() {}
} // namespace Engine