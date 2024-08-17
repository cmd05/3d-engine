#include <cmath>
#include <string>

#include <ecs/systems/RenderSystem.hpp>

#include <ecs/components/Camera.hpp>
#include <ecs/components/Renderable.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/core/Event.hpp>
#include <ecs/core/Coordinator.hpp>

#include <graphics/Shader.hpp>

#include <glm/glm.hpp>
#include <lib/math/Mat44.hpp>

extern Coordinator gCoordinator;

void RenderSystem::Init()
{
    gCoordinator.add_event_listener(METHOD_LISTENER(Events::Window::RESIZED, RenderSystem::WindowSizeListener));

    shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "vertex.glsl", std::string(FS_SHADERS_DIR) + "fragment.glsl");

    mCamera = gCoordinator.create_entity();

    gCoordinator.add_component(
        mCamera,
        Transform{
            .position = glm::vec3(0.0f, 0.0f, 500.0f)
        });

    gCoordinator.add_component(
        mCamera,
        Camera{
            .projectionTransform = Camera::MakeProjectionTransform(45.0f, 0.1f, 1000.0f, 1920, 1080)
        });

    std::vector<Vec3> vertices =
        {
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(0.5f, -0.5f, -0.5f),
            Vec3(0.5f, 0.5f, -0.5f),
            Vec3(0.5f, 0.5f, -0.5f),
            Vec3(-0.5f, 0.5f, -0.5f),
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(-0.5f, -0.5f, 0.5),
            Vec3(0.5f, -0.5f, 0.5),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(-0.5f, 0.5f, 0.5),
            Vec3(-0.5f, -0.5f, 0.5),
            Vec3(-0.5f, 0.5f, 0.5f),
            Vec3(-0.5f, 0.5f, -0.5f),
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(-0.5f, -0.5f, 0.5f),
            Vec3(-0.5f, 0.5f, 0.5f),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(0.5f, 0.5f, -0.5),
            Vec3(0.5f, -0.5f, -0.5),
            Vec3(0.5f, -0.5f, -0.5),
            Vec3(0.5f, -0.5f, 0.5),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(0.5f, -0.5f, -0.5f),
            Vec3(0.5f, -0.5f, 0.5f),
            Vec3(0.5f, -0.5f, 0.5f),
            Vec3(-0.5f, -0.5f, 0.5f),
            Vec3(-0.5f, -0.5f, -0.5f),
            Vec3(-0.5f, 0.5f, -0.5),
            Vec3(0.5f, 0.5f, -0.5),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(0.5f, 0.5f, 0.5),
            Vec3(-0.5f, 0.5f, 0.5),
            Vec3(-0.5f, 0.5f, -0.5)
        };

    std::vector<Vec3> normals =
        {
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, -1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(0.0f, 0.0f, 1.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(-1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(1.0f, 0.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, -1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0f),
            Vec3(0.0f, 1.0f, 0.0)
        };


    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // Vertices
    glGenBuffers(1, &mVboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, mVboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)nullptr);
    glEnableVertexAttribArray(0);

    // Surface normal
    glGenBuffers(1, &mVboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, mVboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)nullptr);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void RenderSystem::Update(float dt)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // NOLINT (hicpp-signed-bitwise)

    shader->Activate();
    glBindVertexArray(mVao);


    auto& cameraTransform = gCoordinator.get_component<Transform>(mCamera);
    auto& camera = gCoordinator.get_component<Camera>(mCamera);

    for (const auto& entity : m_entities)
    {
        const auto& transform = gCoordinator.get_component<Transform>(entity);
        const auto& renderable = gCoordinator.get_component<Renderable>(entity);

        // Mat44 view;
        // view.m[0][3] = -cameraTransform.position.x;
        // view.m[1][3] = -cameraTransform.position.y;
        // view.m[2][3] = -cameraTransform.position.z;

        glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraTransform.position, cameraTransform.position + camFront, up);

        Mat44 rotY;

        float cos_theta_y = cosf(transform.rotation.y);
        float sin_theta_y = sinf(transform.rotation.y);

        rotY.m[0][0] = cos_theta_y;
        rotY.m[2][0] = -sin_theta_y;
        rotY.m[0][2] = sin_theta_y;
        rotY.m[2][2] = cos_theta_y;


        Mat44 rotX;

        float cosThetaX = cosf(transform.rotation.x);
        float sinThetaX = sinf(transform.rotation.x);

        rotX.m[1][1] = cosThetaX;
        rotX.m[2][1] = sinThetaX;
        rotX.m[1][2] = -sinThetaX;
        rotX.m[2][2] = cosThetaX;


        Mat44 rotZ;

        float cosThetaZ = cosf(transform.rotation.z);
        float sinThetaZ = sinf(transform.rotation.z);

        rotZ.m[0][0] = cosThetaZ;
        rotZ.m[1][0] = sinThetaZ;
        rotZ.m[0][1] = -sinThetaZ;
        rotZ.m[1][1] = cosThetaZ;


        Mat44 translate;
        translate.m[0][3] = transform.position.x;
        translate.m[1][3] = transform.position.y;
        translate.m[2][3] = transform.position.z;

        Mat44 scaleMat;
        scaleMat.m[0][0] = transform.scale.x;
        scaleMat.m[1][1] = transform.scale.y;
        scaleMat.m[2][2] = transform.scale.z;

        Mat44 model = translate * scaleMat * rotY;

        glm::mat4 projection = camera.projectionTransform;

        shader->SetUniform<Mat44>("uModel", model);
        shader->SetUniform<glm::mat4>("uView", view);
        shader->SetUniform<glm::mat4>("uProjection", projection);
        shader->SetUniform<Vec3>("uColor", renderable.color);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
}

void RenderSystem::WindowSizeListener(Event& event)
{
    auto windowWidth = event.get_param<unsigned int>(Events::Window::Resized::WIDTH);
    auto windowHeight = event.get_param<unsigned int>(Events::Window::Resized::HEIGHT);

    auto& camera = gCoordinator.get_component<Camera>(mCamera);
    camera.projectionTransform = Camera::MakeProjectionTransform(45.0f, 0.1f, 1000.0f, windowWidth, windowHeight);
}

