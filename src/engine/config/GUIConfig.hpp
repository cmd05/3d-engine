#pragma once

#include <imgui/imgui.h>

struct GUIConfig {
    static constexpr auto IMGUI_CONFIG_FLAGS = 
        (ImGuiConfigFlags_NavEnableKeyboard
         | ImGuiConfigFlags_ViewportsEnable 
         | ImGuiConfigFlags_NavEnableGamepad
         | ImGuiConfigFlags_DockingEnable);

};