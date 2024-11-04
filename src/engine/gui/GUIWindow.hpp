#pragma once

#include <engine/window/WindowManager.hpp>
#include <engine/gui/GUIState.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// interface (abstract class)
class GUIWindow {
public:
    GUIWindow(WindowManager& window_manager, GUIState& gui_state, ImVec2 window_pos);
    virtual void update() = 0;
protected:
    WindowManager* m_window_manager;
    GLFWwindow* m_window;
    GUIState* m_gui_state;
    ImGuiIO* m_imgui_io;

    ImVec2 m_window_pos;
};