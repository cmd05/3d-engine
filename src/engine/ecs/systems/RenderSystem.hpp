#pragma once

#include <memory>

#include <engine/ecs/core/System.hpp>
#include <engine/ecs/core/Event.hpp>

#include <engine/graphics/ModelManager.hpp>
#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/CameraWrapper.hpp>
#include <engine/graphics/Shader.hpp>

#include <engine/graphics/objects/GraphicsObjects.hpp>
#include <engine/graphics/LightRenderer.hpp>

#include <engine/gui/GUIState.hpp>

constexpr const char* MODEL_BIN_PATH = "models/bin";

class RenderSystem : public System
{
public:
    RenderSystem(Scene& scene, Entity camera, GUIState& gui_state);

    void init();
    void update(float dt);

    void set_uniforms_pre_rendering();

    models_interface_type load_models(std::unordered_map<std::string, std::string> models);

    cubemaps_interface_type load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps);
    void draw_cubemap(unsigned int cubemap_id);
private:
    void window_size_listener(Event& event);

    // FIX: prefix shader names by m_
    std::unique_ptr<Shader> model_shader;
    std::unique_ptr<Shader> cubemap_shader;
    
    CameraWrapper m_camera_wrapper;

    ModelManager m_model_manager;
    TextureManager m_texture_manager;
    std::unique_ptr<LightRenderer> m_light_renderer;
    
    GUIState* m_gui_state;
};