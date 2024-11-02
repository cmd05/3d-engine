#pragma once

#include <engine/window/WindowManager.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// interface (abstract class)
class GUIWindow {
public:
    GUIWindow(WindowManager& window_manager, ImVec2 window_pos);
    virtual void update() = 0;
protected:
    WindowManager* m_window_manager;
    GLFWwindow* m_window;
    ImGuiIO* m_imgui_io;
    ImVec2 m_window_pos;
};