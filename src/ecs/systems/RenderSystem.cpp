#include <cmath>
#include <string>

#include <glm/glm.hpp>

#include <ecs/systems/RenderSystem.hpp>

#include <ecs/components/Camera.hpp>
#include <ecs/components/Renderable.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/core/Event.hpp>
#include <ecs/core/Coordinator.hpp>

#include <graphics/Shader.hpp>

void RenderSystem::init()
{
    // add window resize listener
    ref_gcoordinator.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));
    // create shader uniqueptr
    shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "vertex.vs", std::string(FS_SHADERS_DIR) + "fragment.fs");

    // create camera entity
    m_camera = ref_gcoordinator.create_entity();

    ref_gcoordinator.add_component(
        m_camera,
        Transform {
            .position = glm::vec3(0.0f, 0.0f, 500.0f)
        }
    );

    ref_gcoordinator.add_component(
        m_camera,
        Camera {
            .projection_transform = Camera::create_projection_transform(45.0f, 0.1f, 1000.0f, 1920, 1080)
        }
    );

    // save cube data in m_vao and vbo's
    std::vector<glm::vec3> vertices =
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5),
        glm::vec3(0.5f, -0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(-0.5f, 0.5f, 0.5),
        glm::vec3(-0.5f, -0.5f, 0.5),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, -0.5),
        glm::vec3(0.5f, -0.5f, -0.5),
        glm::vec3(0.5f, -0.5f, -0.5),
        glm::vec3(0.5f, -0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5),
        glm::vec3(0.5f, 0.5f, -0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(0.5f, 0.5f, 0.5),
        glm::vec3(-0.5f, 0.5f, 0.5),
        glm::vec3(-0.5f, 0.5f, -0.5)
    };

    std::vector<glm::vec3> normals =
    {
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0)
    };

    // use m_vao
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // vertices
    glGenBuffers(1, &m_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW); // sizeof(glm::vec3) == 12
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    // surface normal
    glGenBuffers(1, &m_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void RenderSystem::update(float dt)
{
    // clear screen and buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT (hicpp-signed-bitwise)

    shader->activate();
    glBindVertexArray(m_vao);

    auto& camera_transform = ref_gcoordinator.get_component<Transform>(m_camera);
    auto& camera = ref_gcoordinator.get_component<Camera>(m_camera);

    // loop through all entities in RenderSystem
    for (const auto& entity : m_entities)
    {
        const auto& transform = ref_gcoordinator.get_component<Transform>(entity);
        const auto& renderable = ref_gcoordinator.get_component<Renderable>(entity);
        
        glm::mat4 model = glm::mat4(1.0);

        model = glm::translate(model, transform.position);
        model = glm::rotate(model, (transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform.scale);
        
        // camera.view_matrix should be created just before rendering,
        // as camera position keeps changing
        camera.view_matrix = Camera::create_view_matrix(camera_transform.position); 

        // set uniforms
        shader->set_uniform<glm::mat4>("uModel", model);
        shader->set_uniform<glm::mat4>("uView", camera.view_matrix);
        shader->set_uniform<glm::mat4>("uProjection", camera.projection_transform);
        shader->set_uniform<glm::vec3>("uColor", renderable.color);

        // draw cube to screen
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // unbind VAO
    glBindVertexArray(0);
}

void RenderSystem::window_size_listener(Event& event)
{
    auto window_width = event.get_param<unsigned int>(Events::Window::Resized::WIDTH);
    auto window_height = event.get_param<unsigned int>(Events::Window::Resized::HEIGHT);

    auto& camera = ref_gcoordinator.get_component<Camera>(m_camera);
    camera.projection_transform = Camera::create_projection_transform(45.0f, 0.1f, 1000.0f, window_width, window_height);
}
