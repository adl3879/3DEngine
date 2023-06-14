#include "ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "Application.h"

namespace Engine
{
ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::OnAttach()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    auto windowState = InputManager::Instance().GetWindowState();
    io.DisplaySize = ImVec2(windowState.Width, windowState.Height);

    float fontSize = 24.0f;
    io.Fonts->AddFontFromFileTTF(
        "/home/adeleye/Source/3DEngine/src/Sandbox/res/fonts/IBM_Plex_Sans/IBMPlexSans-Bold.ttf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        "/home/adeleye/Source/3DEngine/src/Sandbox/res/fonts/IBM_Plex_Sans/IBMPlexSans-Regular.ttf", fontSize);

    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    auto nativeWindow = Application::GetWindow()->GetNativeWindow();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(nativeWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
} // namespace Engine