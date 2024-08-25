#pragma once

#include <memory>

#include <engine/core/System.hpp>
#include <engine/core/Event.hpp>

#include <graphics/Shader.hpp>

class RenderSystem : public System
{
public:
    RenderSystem(Scene& scene): System(scene) {}

    void init(Entity camera);
    void update(float dt);
private:
    void window_size_listener(Event& event);

    std::unique_ptr<Shader> shader;

    Entity m_camera;

    GLuint m_vao{};
    GLuint m_vbo_vertices{};
    GLuint m_vbo_normals{};
};
