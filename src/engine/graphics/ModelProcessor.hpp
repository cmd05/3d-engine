#pragma once

#include <string>
#include <vector>

#include <glad/glad.h> 

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/graphics/MeshProcessor.hpp>

void copy_assimp_vec3(glm::vec3& to, aiVector3D& from);

class ModelManager;

class ModelProcessor {
    friend class ModelManager;

    ModelProcessor();
    ModelProcessor(const std::string& path);
    
    void clear(); // clear data so that same ModelProcessor object can be reused for different model
private:
    std::vector<MeshTexture> m_textures_loaded;	// optimization to store loaded textures

    // initialize to zero by default
    unsigned int m_num_vertices = 0;
    unsigned int m_num_indices = 0;

    std::vector<Mesh> m_meshes;

    std::string m_model_path;
private:
    void load_model(const std::string& path);
    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<MeshTexture> load_material_textures(aiMaterial* mat, aiTextureType ai_tex_type, MeshTextureType mesh_tex_type);
};