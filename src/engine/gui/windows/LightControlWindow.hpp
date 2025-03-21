#pragma once

#include <engine/gui/GUIWindow.hpp>

class LightControlWindow : public GUIWindow {
public:
    // constructor is from GUIWindow
    using GUIWindow::GUIWindow; // Inherit GUIWindow's constructor

    void update() {
        // Always center this window when appearing
        // ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.1f, 0.5f));
        ImGui::SetNextWindowPos(ImVec2(10, 260), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(m_window_length, m_window_height));
        
        // Position 0th light
        ImGui::SliderInt3("light[0].position", m_gui_state->light0_pos, -100.0f, 100.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        
        ImGui::SliderFloat("Exposure", &m_gui_state->exposure, 0.0f, 3.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Ambient Strength", &m_gui_state->ambient_strength, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        // ImGui::ColorEdit3("Exp", (float*)&clear_color); // Edit 3 floats representing a color
        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / this->m_imgui_io->Framerate, this->m_imgui_io->Framerate);
        ImGui::Checkbox("Attenuation", &m_gui_state->attenuation);
        ImGui::Checkbox("HDR Enabled", &m_gui_state->hdr_enabled);
        
        ImGui::End();
    }
private:
    unsigned int m_window_length = 300;
    unsigned int m_window_height = 150;
};