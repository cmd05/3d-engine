#pragma once

#include <memory>

#include <ecs/core/System.hpp>
#include <ecs/core/Event.hpp>

#include <graphics/Shader.hpp>

// class Event;


class RenderSystem : public System
{
public:
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
