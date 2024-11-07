#include <cmath>
#include <string>

#include <glm/glm.hpp>

#include <engine/ecs/systems/RenderSystem.hpp>

#include <engine/ecs/core/Event.hpp>
#include <engine/ecs/core/Scene.hpp>
#include <engine/ecs/core/SceneView.hpp>

#include <engine/ecs/components/Cubemap.hpp>
#include <engine/ecs/components/Camera.hpp>
#include <engine/ecs/components/RenderableComponent.hpp>
#include <engine/ecs/components/Transform.hpp>
#include <engine/ecs/components/Model.hpp>
#include <engine/ecs/components/PointLightComponent.hpp>

#include <engine/graphics/Shader.hpp>
#include <engine/graphics/objects/CubeObject.hpp>

RenderSystem::RenderSystem(Scene& scene, Entity camera, GUIState& gui_state): 
    System(scene),
    m_model_manager(m_texture_manager, FS_RESOURCES_DIR + std::string(MODEL_BIN_PATH)), m_camera_wrapper(scene, camera) {
    // setup opengl properties
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // add window resize listener
    ref_scene.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));
    
    // build default objects and their VBO's
    // this should happen only after opengl context is setup
    g_graphics_objects.init();

    // initialize shaders
    m_model_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "shader_model_normal.vs", std::string(FS_SHADERS_DIR) + "shader_model_normal.fs");
    m_cubemap_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "cubemap.vs", std::string(FS_SHADERS_DIR) + "cubemap.fs");

    // store GUIState pointer
    m_gui_state = &gui_state;

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
    m_texture_manager.draw_cubemap(cubemap_id, m_cubemap_shader, m_camera_wrapper);
}

void RenderSystem::update(float dt)
{
    // clear screen and buffers
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set OpenGL parameters
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_model_shader->activate();
    m_model_shader->set_uniform<glm::vec3>("viewPos", m_camera_wrapper.get_transform_component().position);
    
    int i_lights = 0;
    for(const auto& entity : SceneView<PointLightComponent, Transform>(ref_scene)) {
        glm::vec3 light_color = ref_scene.get_component<PointLightComponent>(entity).light_color;
        Transform light_transform = ref_scene.get_component<Transform>(entity);

        m_light_renderer->draw_light_cube(light_transform, m_camera_wrapper, light_color);

        m_model_shader->activate();

        // TODO: fix all uniforms to use layout= instead of glGetUniform. This will require adding a new method to Shader.cpp
        // provide shader.set_uniform_locations(std::unordered_map<std::string, int (location=)>). set in code / loaded by json file
        m_model_shader->set_uniform<glm::vec3>("pointLights[" + std::to_string(i_lights) + "].position", light_transform.position);
        m_model_shader->set_uniform<glm::vec3>("pointLights[" + std::to_string(i_lights) + "].color", light_color);

        i_lights++;
    }

    // experiment with lighting

    m_model_shader->activate();
    m_model_shader->set_uniform<float>("u_ambient_strength", m_gui_state->ambient_strength);

    // ---

    // draw models
    for(const auto& entity : SceneView<RenderableComponent, Model, Transform>(ref_scene)) {
        const auto& transform = ref_scene.get_component<Transform>(entity);
        const auto& object_model = ref_scene.get_component<Model>(entity);

        m_model_manager.draw_model(m_model_shader, object_model.model_id, m_camera_wrapper, transform);
    }

    glDisable(GL_CULL_FACE);

    // draw cubemaps
    for(const auto& entity : SceneView<RenderableComponent, Cubemap>(ref_scene))
        draw_cubemap(ref_scene.get_component<Cubemap>(entity).id);
}

void RenderSystem::set_uniforms_pre_rendering() {
    // call some private function which sets the pre render loop uniforms
    
    // i=0=>use GL_TEXTURE0, i=1=>GL_TEXTURE1 ...
    // right now set in TextureManager::draw_cubemap. do this when multiple cubemaps are required
    m_cubemap_shader->activate();
}

void RenderSystem::window_size_listener(Event& event) {
    auto window_width = event.get_param<int>(Events::Window::Resized::WIDTH);
    auto window_height = event.get_param<int>(Events::Window::Resized::HEIGHT);
    
    // resize viewport to match new window dimensions
    glViewport(0, 0, window_width, window_height);

    // resize view size for all cameras
    for(auto& entity : SceneView<Camera, Transform>(ref_scene)) {
        CameraWrapper camera_wrapper{ref_scene, entity};
        camera_wrapper.resize_view(window_width, window_height);
    }
}