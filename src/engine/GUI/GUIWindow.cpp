#include <engine/GUI/GUIWindow.hpp>

GUIWindow::GUIWindow(WindowManager& window_manager, ImVec2 window_pos) {
    m_window_manager = &window_manager;
    m_window = m_window_manager->get_window();
    m_imgui_io = &ImGui::GetIO();
    m_window_pos = window_pos;
}