#pragma once

#include <engine/graphics/Shader.hpp>
#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/MeshProcessor.hpp>
#include <engine/graphics/CameraWrapper.hpp>

#include <engine/ecs/components/Transform.hpp>

constexpr const char* BIN_MAP_FNAME = "bin_mapper.map";

using mapper_data_map_type = std::unordered_map<std::string, std::string>;
using models_interface_type = std::unordered_map<std::string, std::size_t>;

class ModelManager {
public:
    struct ModelData {
        // keep total number of vertices and indices for debugging purposes
        unsigned int num_vertices; 
        unsigned int num_indices;
        
        std::vector<Mesh> meshes;

        std::string model_path;
        bool gamma_correction; // TODO: remove from here and ModelProcessor
    };

    struct MeshDrawData {
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        unsigned int num_vertices;
        unsigned int num_indices;

        std::vector<std::pair<unsigned int, MeshTextureType>> textures; // texture id, type
        MeshTexturesAvailable textures_available;
    };

    struct ModelDrawData {
        std::vector<MeshDrawData> meshes;
    };

    ModelManager(TextureManager& tex_manager, std::string bin_dir);

    void load_mapper_data();

    mapper_data_map_type::iterator create_model_dump(const std::string& model_path, std::size_t model_hash);
    std::unordered_map<std::string, std::size_t> load_models(mapper_data_map_type models);
    std::size_t load_model(std::string model_path);
    ModelData load_model_data(std::string model_bin_path);
    void add_model(std::string model_path, std::size_t model_hash, ModelData& model_data);

    MeshDrawData setup_mesh(std::string model_path, Mesh& mesh, bool gamma_correction);

    
    void draw_mesh(const std::unique_ptr<Shader>& shader, MeshDrawData& mesh_draw_data);
    void draw_model(const std::unique_ptr<Shader>& model_shader, std::size_t model_id, const CameraWrapper& camera_wrapper, const Transform& transform);
private:
    using byte_ptr = char*;

    std::unordered_map<std::size_t, ModelDrawData> m_models;
    mapper_data_map_type m_mapper_data;
    
    TextureManager* const m_tex_manager;

    std::string m_mapper_file;
    std::string m_bin_dir;
private:
    std::size_t hasher(const std::string& model_path);
};