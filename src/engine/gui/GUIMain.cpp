#include <engine/gui/GUIMain.hpp>

GUIMain::GUIMain(GUIState& gui_state, const char* glsl_version) {
    m_gui_state = &gui_state;
    m_glsl_version = glsl_version;
}

void GUIMain::bind_window_manager(WindowManager &window_manager) {
    m_window_manager = &window_manager;
    m_window = window_manager.get_window();
}

void GUIMain::init(std::string gui_name) {
    m_gui_name = gui_name;

    init_gui();
    init_windows();
}

void GUIMain::init_gui() {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    m_imgui_io = &ImGui::GetIO();
    m_imgui_io->ConfigFlags |= IMGUI_CONFIG_FLAGS;
    
    ImGuiStyle& style = ImGui::GetStyle();
    
    if (viewports_enabled()) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(m_glsl_version.c_str());
    ImGui::StyleColorsDark();
}

void GUIMain::init_windows() {
    m_camera_control_window = std::make_unique<CameraControlWindow>(*m_window_manager, *m_gui_state, ImVec2{0, 0});
    m_light_control_window = std::make_unique<LightControlWindow>(*m_window_manager, *m_gui_state, ImVec2{0, 0});
}

void GUIMain::new_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIMain::update() {
    m_camera_control_window->update();
    m_light_control_window->update();
    // bool show_demo_window = true;
    // ImGui::ShowDemoWindow(&show_demo_window);


    // part of GUILayer
    // ImGui::Begin("Conan Logo");                          // Create a window called "Conan Logo" and append into it.
    // render_conan_logo();  // draw conan logo if user didn't override update
    // ImGui::End();
}

void GUIMain::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIMain::update_platform_windows() {
    if (viewports_enabled()) {
        GLFWwindow* backup_current_context = m_window_manager->get_current_context();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        m_window_manager->set_current_context(backup_current_context);
    }
}

void GUIMain::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}