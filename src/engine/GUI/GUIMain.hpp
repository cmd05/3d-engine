#pragma once

#include <memory>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <engine/window/WindowManager.hpp>

#include <engine/GUI/windows/CameraControlWindow.hpp>
#include <engine/GUI/windows/LightControlWindow.hpp>

constexpr auto IMGUI_CONFIG_FLAGS = (ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable);

class GUIMain {
public:
    GUIMain(WindowManager& window_manager, const char* glsl_version);

    void new_frame();
    void update();
    void render();
    void update_platform_windows();
    void shutdown();
protected:
    ImGuiIO* m_imgui_io;
private:
    WindowManager* m_window_manager;
    GLFWwindow* m_window;

    // list of gui windows
    std::unique_ptr<CameraControlWindow> m_camera_control_window;
    std::unique_ptr<LightControlWindow> m_light_control_window;

    void init_gui(const char* glsl_version);
    void init_windows();
    bool viewports_enabled() const { return m_imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable; }
};