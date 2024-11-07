#pragma once

#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <memory>

#include <engine/graphics/CameraWrapper.hpp>
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
    unsigned int texture_from_file(std::string file_path, bool gamma_correction = false, bool vflip_texture = false);

    cubemaps_interface_type load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps);
    unsigned int add_cubemap(CubemapFaces faces);
    void draw_cubemap(unsigned int cubemap_id, const std::unique_ptr<Shader>& cubemap_shader, const CameraWrapper& camera_wrapper);
private:
    std::unordered_map<std::string, unsigned int> m_loaded_textures; // absolute texture path, texture id
    std::vector<unsigned int> m_cubemaps;
};