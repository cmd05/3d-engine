#include <glad/glad.h> 
#include <stb/stb_image.h>

#include <array>
#include <unordered_map>
#include <string>

#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/objects/GraphicsObjects.hpp>

#include <lib/utilities/DebugAssert.hpp>

#include <fstream>

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

    // unsigned char *img_data = stbi_load(file_path.c_str(), &width, &height, &num_components, 0);
    
    // check if file at same path with ".tex_bin" exists
    unsigned char* img_data = nullptr;

    std::string bin_path = file_path + ".tex_bin";
    bool bin_exists = (std::ifstream{bin_path, std::ios::binary}).good();

    // stbi_load: 8-bits-per-channel interface
    if(!bin_exists)
        dump_texture_bin(bin_path, img_data, width, height, num_components);
    else
        load_texture_bin(bin_path, img_data, width, height, num_components);

    if(img_data) {
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

    // stbi_image_free(img_data);

    if(!bin_exists)
        stbi_image_free(img_data);
    else
        free_tex_bytes(img_data);

    glBindTexture(GL_TEXTURE_2D, 0); // reset bound texture
    m_loaded_textures[file_path] = texture_id; // add to loaded textures

    return texture_id;
}

void TextureManager::load_texture_bin(std::string bin_path, unsigned char*& img_data, int& width, int& height, int& num_components) {
    std::ifstream ifs_bin {bin_path, std::ios::binary};
    
    // 1. read width
    ifs_bin.read(reinterpret_cast<byte_ptr>(&width), sizeof(width));

    // 2. read height
    ifs_bin.read(reinterpret_cast<byte_ptr>(&height), sizeof(height));

    // 3. read number of components
    ifs_bin.read(reinterpret_cast<byte_ptr>(&num_components), sizeof(num_components));

    std::size_t num_bytes = width * height * num_components;

    // 4. read image data
    alloc_tex_bytes(img_data, num_bytes);
    ifs_bin.read(reinterpret_cast<byte_ptr>(img_data), num_bytes * sizeof(unsigned char));
}

void TextureManager::dump_texture_bin(std::string bin_path, unsigned char*& img_data, int& width, int& height, int& num_components) {
    // create binary file
    std::ofstream ofs_bin{bin_path, std::ios::binary};
    img_data = stbi_load(bin_path.c_str(), &width, &height, &num_components, 0);

    // stbi_load: 8-bits-per-channel interface
    std::size_t num_bytes = width * height * num_components;

    // 1. dump width
    ofs_bin.write(reinterpret_cast<byte_ptr>(&width), sizeof(width));

    // 2. dump height
    ofs_bin.write(reinterpret_cast<byte_ptr>(&height), sizeof(height));

    // 3. dump number of components
    ofs_bin.write(reinterpret_cast<byte_ptr>(&num_components), sizeof(num_components));
    
    // 4. dump image data
    ofs_bin.write(reinterpret_cast<byte_ptr>(img_data), num_bytes * sizeof(unsigned char));
}

void TextureManager::alloc_tex_bytes(unsigned char*& img_data, int num_bytes) {
    img_data = new unsigned char[num_bytes];
}

void TextureManager::free_tex_bytes(unsigned char*& img_data) {
    delete[] img_data;
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
        // TODO: do same binary loading as texture_from_file() for add_cubemap()
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

    // TBD: fix setting texture units and other constant uniforms before render loop (in constructor / rendersystem::init())
    // this can be fixed by having shaders specific to each class. i.e cubemap_shader is initialized in TextureManager
    // we can have a ResourceManager to hold all the shader objects in memory and return pointers (and an id) to them
    cubemap_shader->set_uniform<int>("skybox", 0);

    glm::mat4 view = glm::mat4(glm::mat3(camera_wrapper.get_view_matrix())); // remove translation from the view matrix
    glm::mat4 projection = camera_wrapper.get_projection_matrix();
    glm::mat4 vp_matrix = projection * view;

    cubemap_shader->set_uniform<glm::mat4>("u_VP_matrix", vp_matrix);

    glBindVertexArray(g_graphics_objects.cube.VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);

    // draw cubemap
    glDrawArrays(GL_TRIANGLES, 0, g_graphics_objects.cube.num_vertices);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

bool TextureManager::gamma_correct_required(MeshTextureType tex_type) const {
    // return `true` if texture type is not in `m_no_gamma_correct`
    return std::find(m_no_gamma_correct.begin(), m_no_gamma_correct.end(), tex_type) == m_no_gamma_correct.end();
}