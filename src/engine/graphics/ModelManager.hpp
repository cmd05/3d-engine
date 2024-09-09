#pragma once

#include <fstream>
#include <functional>
#include <cassert>
#include <utility>
#include <memory>

#include <engine/graphics/Shader.hpp>
#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/ModelProcessor.hpp>
#include <engine/graphics/MeshProcessor.hpp>

#define BIN_MAP_FNAME   "bin_mapper.map"

using mapper_data_map_type = std::unordered_map<std::string, std::string>;
using models_interface_type = std::unordered_map<std::string, std::size_t>;

class ModelManager {
public:
    ModelManager(TextureManager& tex_manager, std::string bin_dir): ref_tex_manager(tex_manager) {
        // check if mapper file exists in bin_dir else create
        std::string mapper_file = bin_dir + '/' + BIN_MAP_FNAME;

        if(!std::ifstream(mapper_file) && !std::ofstream(mapper_file))
            assert("Could not create mapper file");

        m_bin_dir = bin_dir;
        m_mapper_file = mapper_file;

        load_mapper_data();
    }

    struct ModelData {
        // keep total number of vertices and indices for debugging purposes
        unsigned int num_vertices; 
        unsigned int num_indices;
        
        std::vector<Mesh> meshes;

        bool gamma_correction;
        std::string model_path;
    };

    struct MeshDrawData {
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        unsigned int num_vertices;
        unsigned int num_indices;

        // texture id, type
        std::vector<std::pair<unsigned int, MeshTextureType>> textures;
    };

    struct ModelDrawData {
        std::vector<MeshDrawData> meshes;
    };

    // returns model hash id
    mapper_data_map_type::iterator create_model_dump(const std::string& model_path, std::size_t model_hash) {
        std::string file_name = model_path.substr(model_path.find_last_of("/\\") + 1);
        std::string bin_name = std::to_string(model_hash) + '_' + file_name; // prefix model_hash so that names do not collide
        std::string model_bin_path = m_bin_dir + '/' + bin_name + ".bin";
        
        std::ofstream ofs {model_bin_path, std::ios::binary};
        // get model data
        ModelProcessor model_processor{model_path};

        /// Dump Data
        // static_assert(std::is_pod_v<std::string> == true);

        // dump counts
        ofs.write(reinterpret_cast<char*>(&model_processor.m_num_vertices), sizeof(model_processor.m_num_vertices));
        ofs.write(reinterpret_cast<char*>(&model_processor.m_num_indices), sizeof(model_processor.m_num_indices));

        // dump meshes
        
        std::size_t meshes_sz = model_processor.m_meshes.size();
        ofs.write(reinterpret_cast<char*>(&meshes_sz), sizeof(meshes_sz));
        
        for(Mesh& mesh : model_processor.m_meshes) {
            // vertices
            std::size_t vec_sz = mesh.m_vertices.size();
            ofs.write(reinterpret_cast<char*>(&vec_sz), sizeof(std::size_t));
            ofs.write(reinterpret_cast<char*>(&mesh.m_vertices[0]), vec_sz * sizeof(mesh.m_vertices[0]));
            
            // indices
            vec_sz = mesh.m_indices.size();
            ofs.write(reinterpret_cast<char*>(&vec_sz), sizeof(std::size_t));
            ofs.write(reinterpret_cast<char*>(&mesh.m_indices[0]), vec_sz * sizeof(mesh.m_indices[0]));
            
            // textures
            // write size of `m_textures` vector
            vec_sz = mesh.m_textures.size();
            ofs.write(reinterpret_cast<char*>(&vec_sz), sizeof(vec_sz));
            
            // write data for textures
            for(MeshTexture& mesh_texture : mesh.m_textures) {
                // write texture type
                ofs.write(reinterpret_cast<char*>(&mesh_texture.type), sizeof(MeshTextureType));

                // write texture path        
                std::size_t str_sz = mesh_texture.path.size();
                ofs.write(reinterpret_cast<char*>(&str_sz), sizeof(str_sz));
                ofs.write(reinterpret_cast<char*>(&mesh_texture.path[0]), str_sz * sizeof(mesh_texture.path[0]));
            }
        }
        
        // dump additional information
        ofs.write(reinterpret_cast<char*>(&model_processor.m_gamma_correction), sizeof(model_processor.m_gamma_correction));
        
        // dump model path
        std::size_t str_sz = model_processor.m_model_path.size();
        ofs.write(reinterpret_cast<char*>(&str_sz), sizeof(str_sz));
        ofs.write(reinterpret_cast<char*>(&model_processor.m_model_path[0]), str_sz * sizeof(model_processor.m_model_path[0]));
    
        std::ofstream mapper_ofs {m_mapper_file, std::ios::app};
        mapper_ofs << model_path << '\t' << model_bin_path << std::endl;
        
        // update mapper data in unordered_map
        auto pair = m_mapper_data.insert({model_path, model_bin_path});

        return pair.first;
    }

    std::unordered_map<std::string, std::size_t> load_models(mapper_data_map_type models) {
        std::unordered_map<std::string, std::size_t> model_id_map;

        for(auto& [model_name, model_path] : models)
            model_id_map[model_name] = load_model(model_path);
        
        return model_id_map;
    }

    ModelData load_model_data(std::string model_bin_path) {        
        ModelData model_data;
        std::ifstream ifs {model_bin_path, std::ios::binary};

        // read counts
        ifs.read(reinterpret_cast<char*>(&model_data.num_vertices), sizeof(model_data.num_vertices));
        ifs.read(reinterpret_cast<char*>(&model_data.num_indices), sizeof(model_data.num_indices));
        
        // meshes vector size
        std::size_t meshes_sz;
        ifs.read(reinterpret_cast<char*>(&meshes_sz), sizeof(meshes_sz));
        model_data.meshes.resize(meshes_sz);
        
        // read meshes
        for(Mesh& mesh : model_data.meshes) {
            std::size_t vec_sz;
            
            // vertices
            ifs.read(reinterpret_cast<char*>(&vec_sz), sizeof(vec_sz));
            mesh.m_vertices.resize(vec_sz);
            ifs.read(reinterpret_cast<char*>(mesh.m_vertices.data()), vec_sz * sizeof(MeshVertex));
            
            // indices
            ifs.read(reinterpret_cast<char*>(&vec_sz), sizeof(vec_sz));
            mesh.m_indices.resize(vec_sz);
            ifs.read(reinterpret_cast<char*>(mesh.m_indices.data()), vec_sz * sizeof(unsigned int));

            // textures
            ifs.read(reinterpret_cast<char*>(&vec_sz), sizeof(vec_sz));
            mesh.m_textures.resize(vec_sz);
            
            for(MeshTexture& mesh_texture : mesh.m_textures) {
                // texture type
                ifs.read(reinterpret_cast<char*>(&mesh_texture.type), sizeof(MeshTextureType));
                
                // texture path
                std::size_t str_sz;
                ifs.read(reinterpret_cast<char*>(&str_sz), sizeof(str_sz));
                mesh_texture.path.resize(str_sz);
                ifs.read(reinterpret_cast<char*>(mesh_texture.path.data()), str_sz * sizeof(char));
            }
        }

        // read additional information
        ifs.read(reinterpret_cast<char*>(&model_data.gamma_correction), sizeof(model_data.gamma_correction));

        // read texture path
        std::size_t str_sz;
        ifs.read(reinterpret_cast<char*>(&str_sz), sizeof(str_sz));
        model_data.model_path.resize(str_sz);
        ifs.read(reinterpret_cast<char*>(model_data.model_path.data()), str_sz * sizeof(char));

        return model_data;
    }

    MeshDrawData setup_mesh(std::string model_path, Mesh& mesh, bool gamma_correction) {
        MeshDrawData mesh_draw_data;

        // set counts
        mesh_draw_data.num_vertices = mesh.m_vertices.size();
        mesh_draw_data.num_indices = mesh.m_indices.size();

        // create buffers/arrays
        glGenVertexArrays(1, &mesh_draw_data.VAO);
        glGenBuffers(1, &mesh_draw_data.VBO);
        glGenBuffers(1, &mesh_draw_data.EBO);

        glBindVertexArray(mesh_draw_data.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh_draw_data.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.m_vertices.size() * sizeof(MeshVertex), &mesh.m_vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_draw_data.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.m_indices.size() * sizeof(unsigned int), &mesh.m_indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));

        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
        
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tex_coords));
        
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
        
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));

        glBindVertexArray(0);

        // load textures
        std::string model_dir = model_path.substr(0, model_path.find_last_of('/'));
        
        for(MeshTexture& mesh_texture : mesh.m_textures) {
            std::string tex_path =  model_dir + '/' + mesh_texture.path;
            unsigned int texture_id = ref_tex_manager.texture_from_file(tex_path, gamma_correction);
            mesh_draw_data.textures.push_back(std::make_pair(texture_id, mesh_texture.type));
        }

        return mesh_draw_data;
    }

    void add_model(std::string model_path, std::size_t model_hash, ModelData& model_data) {
        ModelDrawData model_draw_data;
     
        for(Mesh& mesh : model_data.meshes)
            model_draw_data.meshes.push_back(setup_mesh(model_path, mesh, model_data.gamma_correction));
    
        m_models[model_hash] = model_draw_data; // update list of loaded models
    }

    std::size_t load_model(std::string model_path) {
        // 1. check mapper file if binary object exists for given model path
            // if not create a dump of the model
        // 2. load data from the binary dump
        // 3. convert the data to `ModelData`
        // 4. add this model to `m_models`
        
        std::size_t model_hash = hasher(model_path);

        // don't load model if already loaded
        if(m_models.find(model_hash) != m_models.end()) 
            return model_hash;
        
        auto it = m_mapper_data.begin();
        if((it = m_mapper_data.find(model_path)) == m_mapper_data.end())
            it = create_model_dump(model_path, model_hash); // create dump if it doesn't exist
        
        // retrieve model data
        ModelData model_data = load_model_data(it->second);

        // add model to m_models
        add_model(model_path, model_hash, model_data);

        return model_hash;
    }

    void load_mapper_data() {
        std::ifstream ifs {m_mapper_file};
        std::string model_path, model_bin_path;

        while(ifs >> model_path >> model_bin_path)
            m_mapper_data[model_path] = model_bin_path;
    }

    /// Drawing Logic
    void draw_mesh(std::unique_ptr<Shader>& shader, MeshDrawData& mesh_draw_data) {
        shader->activate();

        // bind appropriate textures
        unsigned int diffuse_nr = 1;
        unsigned int specular_nr = 1;
        unsigned int normal_nr = 1;
        unsigned int ambient_nr = 1;

        for(unsigned int i = 0; i < mesh_draw_data.textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string tex_name;
            MeshTextureType type = mesh_draw_data.textures[i].second;

            if(type == MeshTextureType::DIFFUSE)
                tex_name = "texture_diffuse" + std::to_string(diffuse_nr++);
            if(type == MeshTextureType::SPECULAR)
                tex_name = "texture_specular" + std::to_string(specular_nr++);
            if(type == MeshTextureType::NORMAL)
                tex_name = "texture_normal" + std::to_string(normal_nr++);
            if(type == MeshTextureType::AMBIENT)
                tex_name = "texture_ambient" + std::to_string(ambient_nr++);

            // set value of texture sampler as the texture unit
            glUniform1i(glGetUniformLocation(shader->get_id(), tex_name.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, mesh_draw_data.textures[i].first);
        }

        // draw mesh
        glBindVertexArray(mesh_draw_data.VAO); // automatically binds respective VBO and EBO
        glDrawElements(GL_TRIANGLES, mesh_draw_data.num_indices, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0); // reset bound VAO
        glActiveTexture(GL_TEXTURE0); // reset active texture
    }

    void draw_model(std::unique_ptr<Shader>& shader, std::size_t model_id) {
        auto it = m_models.begin();
        assert((it = m_models.find(model_id)) != m_models.end() && "Model with given ID does not exist");

        ModelDrawData& model_draw_data = it->second;

        for(MeshDrawData& mesh_draw_data : model_draw_data.meshes)
            draw_mesh(shader, mesh_draw_data);
    }

private:
    std::size_t hasher(const std::string& model_path) {
        return std::hash<std::string>{}(model_path);
    }

    std::unordered_map<std::size_t, ModelDrawData> m_models;
    mapper_data_map_type m_mapper_data;
    
    TextureManager& ref_tex_manager;

    std::string m_mapper_file;
    std::string m_bin_dir;
};