#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>

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

void ImGuiLayer::Begin() {}

void ImGuiLayer::End() {}
