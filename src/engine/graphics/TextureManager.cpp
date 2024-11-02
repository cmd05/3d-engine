#include <glad/glad.h> 
#include <stb/stb_image.h>

#include <array>
#include <unordered_map>
#include <string>

#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/objects/GraphicsObjects.hpp>

#include <lib/utilities/DebugAssert.hpp>

TextureManager::TextureManager() {

}

// if same texture path already exists return the texture id
// else create new texture and return its id 
unsigned int TextureManager::texture_from_file(std::string file_path, bool gamma_correction, bool vflip_texture) {
    if(vflip_texture)
        stbi_set_flip_vertically_on_load(true);
    
    // check if texture exists
    auto it = m_loaded_textures.begin();
    if((it = m_loaded_textures.find(file_path)) != m_loaded_textures.end())
        return it->second;
    
    // create a new texture
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, num_components;
    unsigned char *img_data = stbi_load(file_path.c_str(), &width, &height, &num_components, 0);
    
    if (img_data) {
        GLenum internal_format, data_format;

        if (num_components == 1) {
            internal_format = GL_RED;
            data_format = GL_RED;
        } else if (num_components == 3) {
            internal_format = gamma_correction ? GL_SRGB : GL_RGB;
            data_format = GL_RGB;
        } else if (num_components == 4) {
            internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
            data_format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, img_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        ASSERT_MESSAGE("Texture data could not be loaded: " << file_path);
    }
    
    stbi_image_free(img_data);

    glBindTexture(GL_TEXTURE_2D, 0); // reset bound texture
    m_loaded_textures[file_path] = texture_id; // add to loaded textures

    return texture_id;
}

cubemaps_interface_type TextureManager::load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps) {
    cubemaps_interface_type loaded_cubemaps;

    for(auto [name, cubemap] : cubemaps)
        loaded_cubemaps[name] = add_cubemap(cubemap);

    return loaded_cubemaps;
}

unsigned int TextureManager::add_cubemap(CubemapFaces faces) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    // order as required by GL_TEXTURE_CUBE_MAP_POSITIVE_X
    cubemap_array<std::string> faces_ordered = {faces.right, faces.left, faces.top, faces.bottom, faces.front, faces.back};

    int width, height, num_components;

    for(std::size_t i = 0; i < faces_ordered.size(); i++) {
        unsigned char* img_data = stbi_load(faces_ordered[i].c_str(), &width, &height, &num_components, 0);
            
        if (img_data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
        else
            ASSERT_MESSAGE("Cubemap texture failed to load at path: " << faces_ordered[i]);
        
        stbi_image_free(img_data);
    }

    // set cubemap properties
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_cubemaps.push_back(texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture_id;
}

void TextureManager::draw_cubemap(unsigned int cubemap_id, const std::unique_ptr<Shader>& cubemap_shader, const CameraWrapper& camera_wrapper) {
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
    
    cubemap_shader->activate();

    // FIX: fix setting texture units and other constant uniforms before render loop (in constructor / rendersystem::init())
    // this can be fixed by having shaders specific to each class. i.e cubemap_shader is initialized in TextureManager
    // we can have a ResourceManager to hold all the shader objects in memory and return pointers (and an id) to them
    glUniform1i(glGetUniformLocation(cubemap_shader->get_id(), "skybox"), 0);
    
    glm::mat4 view = glm::mat4(glm::mat3(camera_wrapper.get_view_matrix())); // remove translation from the view matrix
    cubemap_shader->set_uniform<glm::mat4>("view", view);
    cubemap_shader->set_uniform<glm::mat4>("projection", camera_wrapper.get_projection_matrix());
    
    glBindVertexArray(g_graphics_objects.cube.VAO);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
    
    // draw cubemap
    glDrawArrays(GL_TRIANGLES, 0, g_graphics_objects.cube.num_vertices);
    
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}