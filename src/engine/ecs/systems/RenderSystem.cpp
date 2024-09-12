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
    m_camera_manager(scene, camera) {
    // setup opengl properties
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // add window resize listener
    ref_scene.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));
    
    // initialize shaders
    model_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "shader_model.vs", std::string(FS_SHADERS_DIR) + "shader_model.fs");
    cubemap_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "cubemap.vs", std::string(FS_SHADERS_DIR) + "cubemap.fs");
}

models_interface_type RenderSystem::load_models(std::unordered_map<std::string, std::string> models) {
    return m_model_manager.load_models(models);
}

cubemaps_interface_type RenderSystem::load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps) {
    return m_texture_manager.load_cubemaps(cubemaps);
}

void RenderSystem::init() {}

void RenderSystem::draw_cubemap(unsigned int cubemap_id) {
    m_texture_manager.draw_cubemap(cubemap_id, *cubemap_shader.get(), m_camera_manager.get_view_matrix(), m_camera_manager.get_projection_matrix());
}

void RenderSystem::update(float dt)
{
    // clear screen and buffers
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT (hicpp-signed-bitwise)

    // auto& camera_transform = ref_scene.get_component<Transform>(m_camera);
    // auto& camera = ref_scene.get_component<Camera>(m_camera);

    // // loop through all entities in RenderSystem
    // for (const auto& entity : SceneView<Renderable, Transform>(ref_scene))
    // {
    //     const auto& transform = ref_scene.get_component<Transform>(entity);
    //     const auto& renderable = ref_scene.get_component<Renderable>(entity);
        
    //     glm::mat4 model = glm::mat4(1.0);

    //     model = glm::translate(model, transform.position);
    //     model = glm::rotate(model, (transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    //     model = glm::rotate(model, (transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    //     model = glm::rotate(model, (transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    //     model = glm::scale(model, transform.scale);
        
    //     // camera.view_matrix should be created just before rendering,
    //     // as camera position keeps changing
    //     camera.view_matrix = Camera::create_view_matrix(camera_transform.position); 

    //     // set uniforms
    //     shader->set_uniform<glm::mat4>("uModel", model);
    //     shader->set_uniform<glm::mat4>("uView", camera.view_matrix);
    //     shader->set_uniform<glm::mat4>("uProjection", camera.projection_transform);
    //     shader->set_uniform<glm::vec3>("uColor", renderable.color);

    //     // draw cube to screen
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }

    // // // unbind VAO
    // glBindVertexArray(0);

    // draw cubemap
    // for(const auto& entity : SceneView<Renderable, Cubemap>(ref_scene)) {
    //     auto& cubemap = ref_scene.get_component<Cubemap>(entity);
    //     m_texture_manager.draw_cubemap(cubemap.id);
    // }

    /// draw all models

    glEnable(GL_DEPTH_TEST);
    model_shader->activate();

    for(const auto& entity : SceneView<Renderable, Model, Transform>(ref_scene)) {
        auto& transform = ref_scene.get_component<Transform>(entity);
        auto& object_model = ref_scene.get_component<Model>(entity);
        // const auto& renderable = ref_scene.get_component<Renderable>(entity);
        
        glm::mat4 model = glm::mat4(1.0);

        model = glm::translate(model, transform.position);
        model = glm::rotate(model, (transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform.scale);

        // camera.view_matrix should be created just before rendering,
        // as camera position keeps changing
        // camera.view_matrix = Camera::create_view_matrix(camera_transform.position); 

        // set uniforms
        model_shader->set_uniform<glm::mat4>("model", model);
        model_shader->set_uniform<glm::mat4>("view", m_camera_manager.get_view_matrix());
        model_shader->set_uniform<glm::mat4>("projection", m_camera_manager.get_projection_matrix());
        // shader->set_uniform<glm::vec3>("uColor", renderable.color);

        m_model_manager.draw_model(model_shader, object_model.model_id);
        // object_model.model.Draw(model_shader); // modelloader::draw itself must activate shader before drawing
    }

    // draw cubemaps
    for(const auto& entity : SceneView<Renderable, Cubemap>(ref_scene)) {
        draw_cubemap(ref_scene.get_component<Cubemap>(entity).id);
    }
}

void RenderSystem::window_size_listener(Event& event)
{
    auto window_width = event.get_param<unsigned int>(Events::Window::Resized::WIDTH);
    auto window_height = event.get_param<unsigned int>(Events::Window::Resized::HEIGHT);

    m_camera_manager.resize_view(window_width, window_height);
}