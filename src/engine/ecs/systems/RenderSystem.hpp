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
    void init_framebuffer_size(int win_framebuffer_width, int win_framebuffer_height);
    // void init();

    void update(float dt);

    void set_uniforms_pre_rendering();
    void set_camera(Entity camera) { m_camera_wrapper = CameraWrapper{*m_scene, camera}; }

    models_interface_type load_models(std::unordered_map<std::string, std::string> models);
    cubemaps_interface_type load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps);
    void draw_cubemap(unsigned int cubemap_id);

    void buffer_camera_data();
    void buffer_gui_data();
    void buffer_matrices();
    
    // void set_dir_lights();
    // void set_point_lights();

    void render_point_lights();
    void render_dir_lights();
    void render_models(const std::unique_ptr<Shader>& shader);
    void render_cubemaps();

    // callback methods for when opengl context has been created
    static void gl_init_callback(Event& event);
private:
    std::unique_ptr<Shader> m_model_shader;
    std::unique_ptr<Shader> m_cubemap_shader;
    std::unique_ptr<Shader> m_hdr_shader;
    std::unique_ptr<Shader> m_shadow_depth_shader;

    CameraWrapper m_camera_wrapper;
    ModelManager m_model_manager;
    TextureManager m_texture_manager;
    LightRenderer m_light_renderer;

    GUIState* const m_gui_state;

    ShaderUniformBlocks m_shader_uniform_blocks;

    unsigned int m_win_framebuffer_width, m_win_framebuffer_height;

    GLuint m_hdr_fbo;
    GLuint m_hdr_color_buffer;
    GLuint m_hdr_render_buffer;

    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    GLuint m_shadow_depth_map_fbo;
    GLuint m_depth_map_tex;
private:
    std::unique_ptr<Shader> make_shader(std::string const& vertex_path, std::string const& fragment_path, std::string geometry_path = "");
    void window_size_listener(Event& event);

    void init_hdr_fbo();
    void resize_hdr_attachments();
    void init_shadow_mapping();
};