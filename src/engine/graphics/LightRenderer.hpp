#pragma once

#include <glm/glm.hpp>

#include <engine/graphics/objects/GraphicsObjects.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/lib/GraphicsHelper.hpp>

#include <engine/graphics/CameraWrapper.hpp>
#include <engine/graphics/ModelManager.hpp>

#include <memory>

class LightRenderer {
public:
    LightRenderer();

    void draw_light_cube(Transform& transform, const CameraWrapper& camera_wrapper, glm::vec3 light_color);

    // TBD: use the modelmanager from RenderSystem and create model_light_shader.vs,fs
    // model lights should have a color only according to their textures and won't be affected by
    // other light sources
    void render_model_light(ModelManager& model_manager);
private:
    std::unique_ptr<Shader> m_light_cube_shader; // it can be used for other shapes too
};