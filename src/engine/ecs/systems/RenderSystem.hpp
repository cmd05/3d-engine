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

#include <engine/shaders/interface/ShaderUniformBlocks.hpp>

class RenderSystem : public System {
public:
    RenderSystem(Scene& scene, Entity camera, GUIState& gui_state);
    // void init();

    void update(float dt);

    void set_uniforms_pre_rendering();
    void set_camera(Entity camera) { m_camera_wrapper = CameraWrapper{*m_scene, camera}; }

    models_interface_type load_models(std::unordered_map<std::string, std::string> models);
    cubemaps_interface_type load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps);
    void draw_cubemap(unsigned int cubemap_id);

    // callback methods for when opengl context has been created
    static void gl_init_callback(Event& event);
private:
    std::unique_ptr<Shader> m_model_shader;
    std::unique_ptr<Shader> m_cubemap_shader;
    
    CameraWrapper m_camera_wrapper;
    ModelManager m_model_manager;
    TextureManager m_texture_manager;
    LightRenderer m_light_renderer;

    GUIState* const m_gui_state;

    ShaderUniformBlocks m_shader_uniform_blocks;
private:
    void window_size_listener(Event& event);
};