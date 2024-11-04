#include <engine/gui/GUIWindow.hpp>

GUIWindow::GUIWindow(WindowManager& window_manager, GUIState& gui_state, ImVec2 window_pos) {
    m_window_manager = &window_manager;
    m_gui_state = &gui_state;

    m_window = m_window_manager->get_window();
    m_imgui_io = &ImGui::GetIO();
    m_window_pos = window_pos;
}