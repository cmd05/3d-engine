#include <cmath>
#include <string>

#include <glm/glm.hpp>

#include <engine/ecs/systems/RenderSystem.hpp>

#include <engine/ecs/core/Event.hpp>
#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/SceneView.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/Renderable.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>

#include <engine/graphics/Shader.hpp>

RenderSystem::RenderSystem(Scene& scene, Entity camera): 
    System(scene),
    m_model_manager(m_texture_manager, FS_RESOURCES_DIR + std::string(MODEL_BIN_PATH)),
    m_camera_wrapper(scene, camera) {
    // setup opengl properties
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // add window resize listener
    ref_scene.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));
    
    // initialize shaders
    model_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "shader_model.vs", std::string(FS_SHADERS_DIR) + "shader_model.fs");
    cubemap_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "cubemap.vs", std::string(FS_SHADERS_DIR) + "cubemap.fs");

    // initialize members (which depend on graphics objects)
    m_light_renderer = std::make_unique<LightRenderer>();
}

models_interface_type RenderSystem::load_models(std::unordered_map<std::string, std::string> models) {
    return m_model_manager.load_models(models);
}

cubemaps_interface_type RenderSystem::load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps) {
    return m_texture_manager.load_cubemaps(cubemaps);
}

void RenderSystem::init() {}

void RenderSystem::draw_cubemap(unsigned int cubemap_id) {
    m_texture_manager.draw_cubemap(cubemap_id, cubemap_shader, m_camera_wrapper);
}

void RenderSystem::update(float dt)
{
    // clear screen and buffers
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all models
    glEnable(GL_DEPTH_TEST);
    model_shader->activate();

    for(const auto& entity : SceneView<Renderable, Model, Transform>(ref_scene)) {
        const auto& transform = ref_scene.get_component<Transform>(entity);
        const auto& object_model = ref_scene.get_component<Model>(entity);

        m_model_manager.draw_model(model_shader, object_model.model_id, m_camera_wrapper, transform);
    }

    // draw cubemaps
    for(const auto& entity : SceneView<Renderable, Cubemap>(ref_scene))
        draw_cubemap(ref_scene.get_component<Cubemap>(entity).id);
}

void RenderSystem::window_size_listener(Event& event) {
    auto window_width = event.get_param<int>(Events::Window::Resized::WIDTH);
    auto window_height = event.get_param<int>(Events::Window::Resized::HEIGHT);
    
    // resize viewport to match new window dimensions
    glViewport(0, 0, window_width, window_height);

    m_camera_wrapper.resize_view(window_width, window_height); // resize camera aspect ratio
}