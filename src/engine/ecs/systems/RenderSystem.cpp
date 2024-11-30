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
#include <engine/ecs/components/PointLight.hpp>

#include <engine/graphics/Shader.hpp>
#include <engine/graphics/objects/CubeObject.hpp>

#include <engine/config/Events.hpp>

void RenderSystem::gl_init_callback(Event& event) {
    // build default objects and their VBO's
    g_graphics_objects.init();
}

RenderSystem::RenderSystem(Scene& scene, Entity camera, GUIState& gui_state): 
    System(scene),
    m_model_manager(m_texture_manager),
    m_camera_wrapper(scene, camera),
    m_gui_state{&gui_state} {
    // setup opengl properties
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // add window resize listener
    m_scene->add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));

    // initialize shaders
    m_model_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "shader_model_normal.vs", std::string(FS_SHADERS_DIR) + "shader_model_normal.fs");
    m_cubemap_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "cubemap.vs", std::string(FS_SHADERS_DIR) + "cubemap.fs");

    // Initialize state of shader data
    m_shader_uniform_blocks.init();
}

models_interface_type RenderSystem::load_models(std::unordered_map<std::string, std::string> models) {
    return m_model_manager.load_models(models);
}

cubemaps_interface_type RenderSystem::load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps) {
    return m_texture_manager.load_cubemaps(cubemaps);
}

void RenderSystem::draw_cubemap(unsigned int cubemap_id) {
    m_texture_manager.draw_cubemap(cubemap_id, m_cubemap_shader, m_camera_wrapper);
}

void RenderSystem::update(float dt) {
    // clear screen and buffers
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set OpenGL parameters
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // buffer camera data     
    m_shader_uniform_blocks.camera.view_pos = glm::vec4(m_camera_wrapper.get_transform_component().position, 0.0);
    glBindBuffer(GL_UNIFORM_BUFFER, m_shader_uniform_blocks.ubo_camera);
    glBufferSubData(GL_UNIFORM_BUFFER, ShaderUniformBlocks::UBLOCK_OFFSET_BEGIN, sizeof(m_shader_uniform_blocks.camera), &m_shader_uniform_blocks.camera);

    // draw lights
    m_model_shader->activate();
    int i_lights = 0;
    
    for(const auto& entity : SceneView<Components::PointLight, Components::Transform>(*m_scene)) {
        glm::vec3 light_color = m_scene->get_component<Components::PointLight>(entity).light_color;
        auto light_transform = m_scene->get_component<Components::Transform>(entity);

        m_light_renderer.draw_light_cube(light_transform, m_camera_wrapper, light_color);

        m_model_shader->activate();

        // set lights for model shader (tbd: this will be changed to ssbo for lights)
        m_model_shader->set_uniform<glm::vec3>("u_point_lights[" + std::to_string(i_lights) + "].position", light_transform.position);
        m_model_shader->set_uniform<glm::vec3>("u_point_lights[" + std::to_string(i_lights) + "].color", light_color);

        i_lights++;
    }

    // buffer GUI parameters
    m_model_shader->activate();
    m_shader_uniform_blocks.gui_state.ambient_strength = m_gui_state->ambient_strength;
    m_shader_uniform_blocks.gui_state.attenuation = m_gui_state->attenuation;
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_shader_uniform_blocks.ubo_gui_state);
    glBufferSubData(GL_UNIFORM_BUFFER, ShaderUniformBlocks::UBLOCK_OFFSET_BEGIN, sizeof(m_shader_uniform_blocks.gui_state), &m_shader_uniform_blocks.gui_state);

    // buffer uniform block matrices
    GraphicsHelper::MVP mvp;
    mvp.view = m_camera_wrapper.get_view_matrix();
    mvp.projection = m_camera_wrapper.get_projection_matrix();

    m_shader_uniform_blocks.matrices.view = mvp.view;
    m_shader_uniform_blocks.matrices.projection = mvp.projection;

    glBindBuffer(GL_UNIFORM_BUFFER, m_shader_uniform_blocks.ubo_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, ShaderUniformBlocks::UBLOCK_OFFSET_BEGIN, sizeof(m_shader_uniform_blocks.matrices), &m_shader_uniform_blocks.matrices);
    // glBufferSubData(GL_UNIFORM_BUFFER, offsetof(ShaderNormalData::ub_matrices, normal_matrix), sizeof(glm::mat3), &shader_normal_data.matrices.normal_matrix);

    // draw models
    for(const auto& entity : SceneView<Components::Renderable, Components::Model, Components::Transform>(*m_scene)) {
        const auto& transform = m_scene->get_component<Components::Transform>(entity);
        const auto& object_model = m_scene->get_component<Components::Model>(entity);

        m_model_manager.draw_model(m_model_shader, object_model.model_id, transform, mvp);
    }

    glDisable(GL_CULL_FACE);

    // draw cubemaps
    for(const auto& entity : SceneView<Components::Renderable, Components::Cubemap>(*m_scene))
        draw_cubemap(m_scene->get_component<Components::Cubemap>(entity).id);
}

void RenderSystem::set_uniforms_pre_rendering() {
    // call some private function which sets the pre render loop uniforms
}

void RenderSystem::window_size_listener(Event& event) {
    auto window_width = event.get_param<int>(Events::Window::Resized::WIDTH);
    auto window_height = event.get_param<int>(Events::Window::Resized::HEIGHT);
    
    // resize viewport to match new window dimensions
    glViewport(0, 0, window_width, window_height);

    // resize view size for all cameras
    for(auto& entity : SceneView<Components::Camera, Components::Transform>(*m_scene)) {
        CameraWrapper camera_wrapper{*m_scene, entity};
        camera_wrapper.resize_view(window_width, window_height);
    }
}