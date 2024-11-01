#include <engine/graphics/LightRenderer.hpp>

LightRenderer::LightRenderer() {
// TODO: make resource manager which loads shaders using make_unique
    // resource manager will then own the Shader objects in memory. (to decide)
    m_light_cube_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "light_cube.vs", std::string(FS_SHADERS_DIR) + "light_cube.fs");
}

void LightRenderer::draw_light_cube(Transform& transform, const CameraWrapper& camera_wrapper, glm::vec3 light_color) {
    GraphicsHelper::set_mvp(m_light_cube_shader, transform, camera_wrapper);

    m_light_cube_shader->activate();
    m_light_cube_shader->set_uniform("lightColor", light_color);
    // m_light_cube_shader->set_uniform("lightPosition", transform.position);

    glBindVertexArray(g_graphics_objects.cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, g_graphics_objects.cube.num_vertices);
}