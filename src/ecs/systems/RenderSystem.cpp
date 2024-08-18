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
    ref_gcoordinator.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::window_size_listener));

    shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "vertex.glsl", std::string(FS_SHADERS_DIR) + "fragment.glsl");

    m_camera = ref_gcoordinator.create_entity();

    ref_gcoordinator.add_component(
        m_camera,
        Transform{
            .position = glm::vec3(0.0f, 0.0f, 500.0f)
        });

    ref_gcoordinator.add_component(
        m_camera,
        Camera{
            .projection_transform = Camera::create_projection_transform(45.0f, 0.1f, 1000.0f, 1920, 1080)
        });

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


    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Vertices
    glGenBuffers(1, &m_vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW); // sizeof(glm::vec3) == 12
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    // Surface normal
    glGenBuffers(1, &m_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void RenderSystem::update(float dt)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT (hicpp-signed-bitwise)

    shader->Activate();
    glBindVertexArray(m_vao);


    auto& cameraTransform = ref_gcoordinator.get_component<Transform>(m_camera);
    auto& camera = ref_gcoordinator.get_component<Camera>(m_camera);

    for (const auto& entity : m_entities)
    {
        const auto& transform = ref_gcoordinator.get_component<Transform>(entity);
        const auto& renderable = ref_gcoordinator.get_component<Renderable>(entity);

        // Mat44 view;
        // view.m[0][3] = -cameraTransform.position.x;
        // view.m[1][3] = -cameraTransform.position.y;
        // view.m[2][3] = -cameraTransform.position.z;

        // glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
        // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        // glm::mat4 view = camera.view_matrix;
        
        // camera.view_matrix should be created just before rendering,
        // as camera position keeps changing
        camera.view_matrix = Camera::create_view_matrix(cameraTransform.position); 

        // Mat44 rotY;

        // float cos_theta_y = cosf(transform.rotation.y);
        // float sin_theta_y = sinf(transform.rotation.y);

        // rotY.m[0][0] = cos_theta_y;
        // rotY.m[2][0] = -sin_theta_y;
        // rotY.m[0][2] = sin_theta_y;
        // rotY.m[2][2] = cos_theta_y;


        // Mat44 rotX;

        // float cosThetaX = cosf(transform.rotation.x);
        // float sinThetaX = sinf(transform.rotation.x);

        // rotX.m[1][1] = cosThetaX;
        // rotX.m[2][1] = sinThetaX;
        // rotX.m[1][2] = -sinThetaX;
        // rotX.m[2][2] = cosThetaX;


        // Mat44 rotZ;

        // float cosThetaZ = cosf(transform.rotation.z);
        // float sinThetaZ = sinf(transform.rotation.z);

        // rotZ.m[0][0] = cosThetaZ;
        // rotZ.m[1][0] = sinThetaZ;
        // rotZ.m[0][1] = -sinThetaZ;
        // rotZ.m[1][1] = cosThetaZ;


        // Mat44 translate;
        // translate.m[0][3] = transform.position.x;
        // translate.m[1][3] = transform.position.y;
        // translate.m[2][3] = transform.position.z;

        // Mat44 scaleMat;
        // scaleMat.m[0][0] = transform.scale.x;
        // scaleMat.m[1][1] = transform.scale.y;
        // scaleMat.m[2][2] = transform.scale.z;
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, transform.position);
        model = glm::rotate(model, (transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, (transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform.scale);

        // Mat44 model = translate * scaleMat * rotY;

        shader->SetUniform<glm::mat4>("uModel", model);
        shader->SetUniform<glm::mat4>("uView", camera.view_matrix);
        shader->SetUniform<glm::mat4>("uProjection", camera.projection_transform);
        shader->SetUniform<glm::vec3>("uColor", renderable.color);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
}

void RenderSystem::window_size_listener(Event& event)
{
    auto windowWidth = event.get_param<unsigned int>(Events::Window::Resized::WIDTH);
    auto windowHeight = event.get_param<unsigned int>(Events::Window::Resized::HEIGHT);

    auto& camera = ref_gcoordinator.get_component<Camera>(m_camera);
    camera.projection_transform = Camera::create_projection_transform(45.0f, 0.1f, 1000.0f, windowWidth, windowHeight);
}
