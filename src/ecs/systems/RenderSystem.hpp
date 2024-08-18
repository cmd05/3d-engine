#pragma once

#include <memory>

#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

#include <graphics/Shader.hpp>

class RenderSystem : public System
{
public:
    RenderSystem(Coordinator& coordinator): System(coordinator) {}

    void Init();

    void Update(float dt);

private:
    void WindowSizeListener(Event& event);

    std::unique_ptr<Shader> shader;

    Entity mCamera;

    GLuint mVao{};
    GLuint mVboVertices{};
    GLuint mVboNormals{};
};
