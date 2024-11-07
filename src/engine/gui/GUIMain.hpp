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
    GUIMain(GUIState& gui_state, const char* glsl_version);
    void bind_window_manager(WindowManager& window_manager);
    void init(std::string gui_name);

    void new_frame();
    void update();
    void render();
    void update_platform_windows();
    void shutdown();
    const ImGuiIO& get_io() { return *m_imgui_io; }

    bool io_want_capture_keyboard() { return ImGui::GetIO().WantCaptureKeyboard; }
    bool io_want_capture_mouse() { return m_imgui_io->WantCaptureMouse; }
private:
    std::string m_glsl_version;
    std::string m_gui_name;

    WindowManager* m_window_manager;
    GUIState* m_gui_state;
    GLFWwindow* m_window;
    ImGuiIO* m_imgui_io;

    // list of gui windows
    std::unique_ptr<CameraControlWindow> m_camera_control_window;
    std::unique_ptr<LightControlWindow> m_light_control_window;

    void init_gui();
    void init_windows();
    bool viewports_enabled() const { return m_imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable; }
};