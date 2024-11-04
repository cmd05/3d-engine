#pragma once

#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <engine/window/WindowManager.hpp>

#include <engine/gui/GUIState.hpp>
#include <engine/gui/windows/CameraControlWindow.hpp>
#include <engine/gui/windows/LightControlWindow.hpp>

constexpr auto IMGUI_CONFIG_FLAGS = (ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable);

class GUIMain {
public:
    GUIMain(WindowManager& window_manager, GUIState& gui_state, const char* glsl_version);

    void new_frame();
    void update();
    void render();
    void update_platform_windows();
    void shutdown();
private:
    WindowManager* m_window_manager;
    GUIState* m_gui_state;
    GLFWwindow* m_window;
    ImGuiIO* m_imgui_io;

    // list of gui windows
    std::unique_ptr<CameraControlWindow> m_camera_control_window;
    std::unique_ptr<LightControlWindow> m_light_control_window;

    void init_gui(const char* glsl_version);
    void init_windows();
    bool viewports_enabled() const { return m_imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable; }
};