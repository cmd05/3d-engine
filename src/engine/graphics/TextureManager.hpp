#pragma once

#include <unordered_map>
#include <array>
#include <vector>
#include <string>

#include <engine/graphics/Shader.hpp>

template<typename T>
using cubemap_array = std::array<T, 6>;

using cubemaps_interface_type = std::unordered_map<std::string, unsigned int>;

struct CubemapFaces {
    std::string right, left, top, bottom, front, back;
};

class TextureManager {
public:
    TextureManager();
    unsigned int texture_from_file(std::string file_path, bool gamma = false, bool vflip_texture = false);

    cubemaps_interface_type load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps);
    unsigned int add_cubemap(CubemapFaces faces);
    // FIX: dirty workaround right now for matrices. take Camera& as a reference, for camera class
    void draw_cubemap(unsigned int cubemap_id, Shader& cubemap_shader, glm::mat4 view_matrix, glm::mat4 projection_matrix);
private:
    // full texture path, texture id
    std::unordered_map<std::string, unsigned int> m_loaded_textures;
    
    std::vector<unsigned int> m_cubemaps;
    unsigned int m_cubemap_vao;
};