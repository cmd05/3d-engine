#include <engine/config/WindowConfig.hpp>
#include <engine/config/Events.hpp>
#include <application/Application.hpp>
#include <engine/ecs/core/Types.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Gravity.hpp>
#include <engine/ecs/components/Player.hpp>
#include <engine/ecs/components/Renderable.hpp>
#include <engine/ecs/components/RigidBody.hpp>
#include <engine/ecs/components/Thrust.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>
#include <engine/ecs/components/PointLight.hpp>
#include <engine/ecs/components/DirectionalLight.hpp>

#include <fstream>
#include <nlohmann/json.hpp>

Application::Application() {
    register_callbacks();

    m_input_handler = std::make_unique<InputHandler>(m_main_scene);
    m_window_manager = std::make_unique<WindowManager>(m_main_scene, *m_input_handler);
    m_gui_state = std::make_unique<GUIState>();
    m_gui_main = std::make_unique<GUIMain>(*m_gui_state, m_glsl_version);

    int window_width = WindowConfig::DEFAULT_SCR_WIDTH;
    int window_height = WindowConfig::DEFAULT_SCR_HEIGHT;

    m_window_manager->bind_gui(*m_gui_main);
    m_window_manager->init("3D Engine", window_width, window_height, 0, 0);
    
    m_gui_main->bind_window_manager(*m_window_manager);
    m_gui_main->init("3D Engine Editor");

    register_ecs_components();
    register_ecs_systems();
}

Application::~Application() {
    m_window_manager->shutdown();
}

void Application::init() {
    load_models();

    init_lights();
    init_models();
    m_render_system->set_uniforms_pre_rendering();
}

void Application::load_models() {
    try {
        std::unordered_map<std::string, std::string> models;

        std::ifstream models_ifs {std::string(FS_RESOURCES_DIR) + "models.json"};
        nlohmann::json j_models;
        models_ifs >> j_models;

        if (j_models.contains("models") && j_models["models"].is_array()) {
            for (const auto& obj : j_models["models"]) {
                if (!obj.empty()) {
                    auto it = obj.begin();
                    std::string key = it.key();
                    std::string value = it.value();
                    
                    models[key] = std::string(FS_RESOURCES_DIR) + value;
                }
            }
        }

        m_models_map = m_render_system->load_models(models);
    } catch(...) {
        ASSERT_MESSAGE("Error importing models. Check debug");
    }
}

void Application::init_lights() {
    // constexpr int NR_POINT_LIGHTS = 1;
    // for(int i = 0; i < NR_POINT_LIGHTS; i++) {
    //     auto light_entity = m_main_scene.create_entity();

    //     m_main_scene.add_components(light_entity,
    //         Components::Transform {
    //             .position = glm::vec3(10.0f, 50.0f, 10.0f),
    //             .scale = glm::vec3(5.0f)
    //         },

    //         Components::PointLight {
    //             .light_color = glm::vec3(1.0f)
    //         }
    //     );
    // }

    constexpr int NR_DIR_LIGHTS = 1;
    for(int i = 0; i < NR_DIR_LIGHTS; i++) {
        auto light_entity = m_main_scene.create_entity();

        m_main_scene.add_components(light_entity,
            Components::DirectionalLight {
                .direction = glm::vec3(-1.0f, 1.0f, 0.0f),
                .light_color = glm::vec3(1.0f)
            },
            Components::Transform {
                // .position = {1000.0f, 1000.0f, 0.0f}
                .position = glm::vec3(10.0f, 50.0f, 10.0f)
            }
        );
    }
}

void Application::init_models() {
    for(auto& [_, v] : m_models_map) {
        auto model_entity = m_main_scene.create_entity();

        m_main_scene.add_components(model_entity,
            Components::Transform {
                .position = glm::vec3(0.0f),
                .scale = glm::vec3(0.2f)
            },
            Components::Renderable{},
            Components::Model{.model_id = v }
        );
    }
}

void Application::run() {
    float last_frame, current_frame;
    last_frame = current_frame = m_window_manager->get_time(); // windowManager.get_time() is not exactly zero at this call
    float dt = 0.0f;

    while (!m_quit) {
        m_window_manager->process_events(); // first process window events

        m_camera_control_system->update(dt);
        m_render_system->update(dt);

        m_gui_main->new_frame();
        m_gui_main->update();
        m_gui_main->render();
        m_gui_main->update_platform_windows();

        m_window_manager->update(); // window updation happens after windows have been processed

        update_frame_times(m_window_manager->get_time(), current_frame, last_frame, dt);
    }
}

void Application::register_callbacks() {
    m_main_scene.add_event_listener(METHOD_LISTENER(Events::Window::QUIT, Application::quit_handler));
    
    // initialize program state (ex: g_graphics_objects) once OpenGL has been initialized
    m_main_scene.add_event_listener(FUNCTION_LISTENER(Events::Window::GL_INIT, RenderSystem::gl_init_callback));
}

void Application::init_camera(int width, int height) {
    m_camera_entity = m_main_scene.create_entity();

    m_main_scene.add_components(
        m_camera_entity,
        Components::Transform {.position = GraphicsConfig::Camera::DEFAULT_CAM_POS},
        Components::Camera(width, height)
    );

    Components::Camera& camera_component = m_main_scene.get_component<Components::Camera>(m_camera_entity);
    camera_component.set_cam_front(glm::vec3(-1.0f, 0.0f, 0.0f));
}

void Application::register_ecs_systems() {
    m_physics_system = &m_main_scene.register_system<PhysicsSystem>();
    m_camera_control_system = &m_main_scene.register_system<CameraControlSystem>(*m_input_handler);
 
    // set render system
    int win_framebuffer_width, win_framebuffer_height;
    m_window_manager->get_framebuffer_size(win_framebuffer_width, win_framebuffer_height);
    ENGINE_LOG(win_framebuffer_width << " " << win_framebuffer_height);

    init_camera(win_framebuffer_width, win_framebuffer_height);

    m_render_system = &m_main_scene.register_system<RenderSystem>(m_camera_entity, *m_gui_state);
    m_render_system->init_framebuffer_size(win_framebuffer_width, win_framebuffer_height);
}

void Application::register_ecs_components() {
    m_main_scene.register_component<
        Components::Camera,
        Components::Gravity,
        Components::Renderable,
        Components::RigidBody,
        Components::Transform,
        Components::Model,
        Components::Cubemap,
        Components::PointLight,
        Components::DirectionalLight,
        
        Components::Player,
        Components::Thrust
    >();
}

void Application::update_frame_times(float new_time, float &curr_time, float &last_time, float &dt) {
    curr_time = new_time;
    dt = curr_time - last_time;
    last_time = curr_time;
}

void Application::quit_handler(Event &event) {
    ENGINE_LOG("[quit_handler]");
    m_quit = true;
}
