#pragma once

#include <engine/GUI/GUIWindow.hpp>

class CameraControlWindow : public GUIWindow {
public:
    // constructor is from GUIWindow
    using GUIWindow::GUIWindow; // Inherit GUIWindow's constructor

    void update() {
        // Always center this window when appearing
        // ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.1f, 0.1f));
        ImGui::SetNextWindowPos(ImVec2(10, 60), ImGuiCond_Once); // TODO: fix these magic values for window size and placements
        
        ImGui::Begin("Camera Control");                          // Create a window called "Hello, world!" and append into it.
        

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImVec4 clear_color;
        float f;

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / this->m_imgui_io->Framerate, this->m_imgui_io->Framerate);
        ImGui::End();
    }
// private:
//     unsigned int m_window_length = 200;
//     unsigned int m_window_length = 150;
};