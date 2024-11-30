#include <fstream>
#include <functional>
#include <cassert>
#include <utility>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <engine/graphics/ModelManager.hpp>

#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/ModelProcessor.hpp>
#include <engine/graphics/MeshProcessor.hpp>

#include <lib/utilities/DebugAssert.hpp>

#include <engine/shaders/interface/ShaderUniformBlocks.hpp>
#include <engine/shaders/interface/ShaderDataTypes.hpp>

#include <numeric>
#include <algorithm>

ModelManager::ModelManager(TextureManager& tex_manager): m_tex_manager{&tex_manager} {
    // check if mapper file exists in bin_dir else create
    if(!std::ifstream(m_mapper_file) && !std::ofstream(m_mapper_file))
        ASSERT_MESSAGE("Could not create mapper file");

    load_mapper_data();
}

std::unordered_map<std::string, std::size_t> ModelManager::load_models(mapper_data_map_type models) {
    std::unordered_map<std::string, std::size_t> model_id_map;

    for(auto& [model_name, model_path] : models)
        model_id_map[model_name] = load_model(model_path);
    
    return model_id_map;
}

ModelManager::MeshDrawData ModelManager::setup_mesh(std::string model_path, Mesh& mesh) {
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

    // vertex positions (location = 0)
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));

    // vertex normals (location = 1)
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, normal));
    
    // vertex texture coords (location = 2)
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tex_coords));
    
    // vertex tangent (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, tangent));
    
    // vertex bitangent (location = 4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, bitangent));

    glBindVertexArray(0);

    // Textures
    // store count of available textures
    mesh_draw_data.textures_available = mesh.m_textures_available;

    // load textures
    std::string model_dir = model_path.substr(0, model_path.find_last_of('/'));
    
    for(MeshTexture& mesh_texture : mesh.m_textures) {
        std::string tex_path =  model_dir + '/' + mesh_texture.path;
    
        bool gamma_correct_required = m_tex_manager->gamma_correct_required(mesh_texture.type);

        unsigned int texture_id = m_tex_manager->texture_from_file(tex_path, gamma_correct_required);
        mesh_draw_data.textures.push_back(std::make_pair(texture_id, mesh_texture.type));
    }

    return mesh_draw_data;
}

void ModelManager::add_model(std::string model_path, std::size_t model_hash, ModelData& model_data) {
    ModelDrawData model_draw_data;
    
    for(Mesh& mesh : model_data.meshes)
        model_draw_data.meshes.push_back(setup_mesh(model_path, mesh));

    m_models[model_hash] = model_draw_data; // update list of loaded models
}

std::size_t ModelManager::load_model(std::string model_path) {
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

void ModelManager::load_mapper_data() {
    std::ifstream ifs {m_mapper_file};
    std::string model_path, model_bin_path;

    while(ifs >> model_path >> model_bin_path)
        m_mapper_data[model_path] = model_bin_path;
}

/// Drawing Logic

void ModelManager::draw_mesh(const std::unique_ptr<Shader>& shader, MeshDrawData& mesh_draw_data) {
    shader->activate();

    // bind appropriate textures
    // count for texture names as texture_diffuse1, texture_diffuse2 ...
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    unsigned int normal_nr = 1;
    unsigned int ambient_nr = 1;

    // DEBUGGING:
    // if(mesh_draw_data.textures_available.specular)
    //     ASSERT_MESSAGE("has spec");
    // if(mesh_draw_data.textures_available.diffuse)
    //     ASSERT_MESSAGE("has diff");
    // if(mesh_draw_data.textures_available.normal)
    //     ASSERT_MESSAGE("has normal");

    // set mesh uniforms
    shader->set_uniform<int>("u_mesh_textures_available.ambient", mesh_draw_data.textures_available.ambient);
    shader->set_uniform<int>("u_mesh_textures_available.diffuse", mesh_draw_data.textures_available.diffuse);
    shader->set_uniform<int>("u_mesh_textures_available.specular", mesh_draw_data.textures_available.specular);
    shader->set_uniform<int>("u_mesh_textures_available.normal", mesh_draw_data.textures_available.normal);

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
        shader->set_uniform<int>(tex_name.c_str(), i);
        glBindTexture(GL_TEXTURE_2D, mesh_draw_data.textures[i].first);
    }

    // draw mesh
    glBindVertexArray(mesh_draw_data.VAO); // automatically binds respective VBO and EBO
    glDrawElements(GL_TRIANGLES, mesh_draw_data.num_indices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0); // reset bound VAO
    glActiveTexture(GL_TEXTURE0); // reset active texture
}

void ModelManager::draw_model(const std::unique_ptr<Shader>& model_shader, std::size_t model_id,
    const Components::Transform& transform, const GraphicsHelper::MVP& mvp) {    
    // retrieve model data
    auto it = m_models.begin();
    assert((it = m_models.find(model_id)) != m_models.end() && "Model with given ID does not exist");
    ModelDrawData& model_draw_data = it->second;
    
    model_shader->activate();

    ShaderDataTypes::MeshMatrices mesh_matrices;
    mesh_matrices.model_matrix = GraphicsHelper::create_model_matrix(transform);
    mesh_matrices.mvp_matrix = mvp.projection * mvp.view * mesh_matrices.model_matrix;
    mesh_matrices.normal_matrix = glm::inverseTranspose(glm::mat3(mesh_matrices.model_matrix));
    
    model_shader->set_uniform<glm::mat4>("u_mesh_matrices.model_matrix", mesh_matrices.model_matrix);
    model_shader->set_uniform<glm::mat4>("u_mesh_matrices.mvp_matrix", mesh_matrices.mvp_matrix);
    model_shader->set_uniform<glm::mat3>("u_mesh_matrices.normal_matrix", mesh_matrices.normal_matrix);

    // draw meshes
    for(MeshDrawData& mesh_draw_data : model_draw_data.meshes)
        draw_mesh(model_shader, mesh_draw_data);
}

std::size_t ModelManager::hasher(const std::string& model_path) {
    return std::hash<std::string>{}(model_path);
}

/// ------------------------ MODEL SERIALIZATION AND DESERIALIZATION ---------------------------------------------------

// returns model hash id
mapper_data_map_type::iterator ModelManager::create_model_dump(const std::string& model_path, std::size_t model_hash) {
    std::string file_name = model_path.substr(model_path.find_last_of("/\\") + 1);
    std::string bin_name = std::to_string(model_hash) + '_' + file_name; // prefix model_hash so that names do not collide
    std::string model_bin_path = m_bin_dir + '/' + bin_name + ".bin";
    
    std::ofstream ofs {model_bin_path, std::ios::binary};
    
    // Get model data
    ModelProcessor model_processor{model_path};

    /// Dump Data
    // static_assert(std::is_pod_v<std::string> == true);

    /// A. Total vertices and indices counts
    ofs.write(reinterpret_cast<byte_ptr>(&model_processor.m_num_vertices), sizeof(model_processor.m_num_vertices));
    ofs.write(reinterpret_cast<byte_ptr>(&model_processor.m_num_indices), sizeof(model_processor.m_num_indices));

    /// B. Dump meshes
    
    // 1. dump count of meshes
    std::size_t num_meshes = model_processor.m_meshes.size();
    ofs.write(reinterpret_cast<byte_ptr>(&num_meshes), sizeof(num_meshes));

    // 2. dump per mesh vertices count
    for(Mesh& mesh : model_processor.m_meshes) {
        std::size_t vec_sz = mesh.m_vertices.size();
        ofs.write(reinterpret_cast<byte_ptr>(&vec_sz), sizeof(vec_sz));
    }

    // 3. dump per mesh vertices
    for(Mesh& mesh : model_processor.m_meshes) {
        std::size_t vec_sz = mesh.m_vertices.size();
        ofs.write(reinterpret_cast<byte_ptr>(&mesh.m_vertices[0]), vec_sz * sizeof(mesh.m_vertices[0]));
    }

    // 4. dump per mesh indices count
    for(Mesh& mesh : model_processor.m_meshes) {
        std::size_t vec_sz = mesh.m_indices.size();
        ofs.write(reinterpret_cast<byte_ptr>(&vec_sz), sizeof(vec_sz));
    }

    // 5. dump per mesh indices
    for(Mesh& mesh : model_processor.m_meshes) {
        std::size_t vec_sz = mesh.m_indices.size();
        ofs.write(reinterpret_cast<byte_ptr>(&mesh.m_indices[0]), vec_sz * sizeof(mesh.m_indices[0]));
    }

    // 6. write `MeshTexturesAvailable` data per mesh
    for(Mesh& mesh : model_processor.m_meshes) {
        ofs.write(reinterpret_cast<byte_ptr>(&mesh.m_textures_available), sizeof(mesh.m_textures_available));
    }

    // 7. write size of `m_textures` vector per mesh
    for(Mesh& mesh : model_processor.m_meshes) {
        std::size_t vec_sz = mesh.m_textures.size();
        ofs.write(reinterpret_cast<byte_ptr>(&vec_sz), sizeof(vec_sz));
    }

    // 8. write textures types per texture per mesh
    for(Mesh& mesh : model_processor.m_meshes) {
        for(MeshTexture& mesh_texture : mesh.m_textures)
            ofs.write(reinterpret_cast<byte_ptr>(&mesh_texture.type), sizeof(mesh_texture.type));
    }

    // 9. write textures path length per texture per mesh
    for(Mesh& mesh : model_processor.m_meshes) {
        for(MeshTexture& mesh_texture : mesh.m_textures) {
            std::size_t len_str = mesh_texture.path.size();
            ofs.write(reinterpret_cast<byte_ptr>(&len_str), sizeof(len_str));
        }
    }

    // 10. write texture paths per texture per mesh
    for(Mesh& mesh : model_processor.m_meshes) {
        for(MeshTexture& mesh_texture : mesh.m_textures) {
            std::size_t len_str = mesh_texture.path.size();
            ofs.write(reinterpret_cast<byte_ptr>(&mesh_texture.path[0]), len_str * sizeof(mesh_texture.path[0]));
        }
    }
    
    /// C. Dump additional information
    
    // 1. Dump model path
    std::size_t len_str = model_processor.m_model_path.size();
    ofs.write(reinterpret_cast<byte_ptr>(&len_str), sizeof(len_str));
    ofs.write(reinterpret_cast<byte_ptr>(&model_processor.m_model_path[0]), len_str * sizeof(model_processor.m_model_path[0]));

    /// Write to mapper file
    std::ofstream mapper_ofs {m_mapper_file, std::ios::app};
    mapper_ofs << model_path << '\t' << model_bin_path << std::endl;
    
    // Update loaded mapper data in memory
    auto pair = m_mapper_data.insert({model_path, model_bin_path});

    return pair.first;
}

ModelManager::ModelData ModelManager::load_model_data(std::string model_bin_path) {        
    // Create ModelData and initialize input stream 
    ModelData model_data;
    std::ifstream ifs {model_bin_path, std::ios::binary};

    /// A. Total vertices and indices counts
    ifs.read(reinterpret_cast<byte_ptr>(&model_data.num_vertices), sizeof(model_data.num_vertices));
    ifs.read(reinterpret_cast<byte_ptr>(&model_data.num_indices), sizeof(model_data.num_indices));
    
    /// B. Load meshes

    // 1. load count of meshes
    std::size_t num_meshes;
    ifs.read(reinterpret_cast<byte_ptr>(&num_meshes), sizeof(num_meshes));

    // 2. load per mesh vertices count
    std::vector<std::size_t> mesh_vertex_counts;
    mesh_vertex_counts.resize(num_meshes);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_vertex_counts.data()), num_meshes * sizeof(std::size_t));

    // 3. load per mesh vertices
    std::vector<MeshVertex> mesh_vertices;
    mesh_vertices.resize(model_data.num_vertices);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_vertices.data()), model_data.num_vertices * sizeof(MeshVertex));

    // 4. load per mesh indices count
    std::vector<std::size_t> mesh_index_counts;
    mesh_index_counts.resize(num_meshes);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_index_counts.data()), num_meshes * sizeof(std::size_t));

    // 5. load per mesh indices
    std::vector<unsigned int> mesh_indices;
    mesh_indices.resize(model_data.num_indices);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_indices.data()), model_data.num_indices * sizeof(unsigned int));

    // 6. load `MeshTexturesAvailable` data per mesh
    std::vector<MeshTexturesAvailable> mesh_textures_available;
    mesh_textures_available.resize(num_meshes);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_textures_available.data()), num_meshes * sizeof(MeshTexturesAvailable));

    // 7. load size of `m_textures` vector per mesh
    std::vector<std::size_t> mesh_textures_counts;
    mesh_textures_counts.resize(num_meshes);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_textures_counts.data()), num_meshes * sizeof(std::size_t));

    // 8. load texture types per texture per mesh
    std::size_t sum_mesh_texture_counts = std::accumulate(mesh_textures_counts.begin(), mesh_textures_counts.end(), (std::size_t) 0);
    
    std::vector<MeshTextureType> mesh_texture_types;
    mesh_texture_types.resize(sum_mesh_texture_counts);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_texture_types.data()), sum_mesh_texture_counts * sizeof(MeshTextureType));

    // 9. load texture path lengths per texture per mesh
    // TBD: we can optimize here, for not repeating path names and 
    // instead having some sort of indicator to existing loaded path name in the vector
    std::vector<std::size_t> mesh_texture_path_lengths;
    mesh_texture_path_lengths.resize(sum_mesh_texture_counts);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_texture_path_lengths.data()), sum_mesh_texture_counts * sizeof(std::size_t));

    // 10. load texture path per texture per mesh
    std::size_t sum_mesh_texture_path_lengths = std::accumulate(mesh_texture_path_lengths.begin(), mesh_texture_path_lengths.end(), (std::size_t) 0);

    std::vector<char> mesh_texture_paths;
    mesh_texture_paths.resize(sum_mesh_texture_path_lengths);
    ifs.read(reinterpret_cast<byte_ptr>(mesh_texture_paths.data()), sum_mesh_texture_path_lengths * sizeof(char));

    /// C. Load additional data

    // 1. load model path
    std::size_t len_model_path;
    ifs.read(reinterpret_cast<byte_ptr>(&len_model_path), sizeof(len_model_path));

    // std::string model_path;
    model_data.model_path.resize(len_model_path);
    ifs.read(reinterpret_cast<byte_ptr>(model_data.model_path.data()), len_model_path * sizeof(char));

    /// Load meshes into `model_data`
    model_data.meshes.resize(num_meshes);

    // keep iterators for vectors
    auto next_vertex_it = mesh_vertices.begin();
    auto next_index_it = mesh_indices.begin();
    auto next_texture_type_it = mesh_texture_types.begin();
    auto next_texture_path_len_it = mesh_texture_path_lengths.begin();
    auto next_texture_path_char_it = mesh_texture_paths.begin();

    for(std::size_t i = 0; i < num_meshes; i++) {
        Mesh& current_mesh = model_data.meshes[i];
        
        std::size_t num_vertices = mesh_vertex_counts[i];
        std::size_t num_indices = mesh_index_counts[i];
        
        // move vertices
        current_mesh.m_vertices.resize(num_vertices);
        std::move(next_vertex_it, next_vertex_it + num_vertices, current_mesh.m_vertices.begin());
        next_vertex_it += num_vertices;

        // move indices
        current_mesh.m_indices.resize(num_indices);
        std::move(next_index_it, next_index_it + num_indices, current_mesh.m_indices.begin());
        next_index_it += num_indices;

        // copy `MeshTexturesAvailable`
        current_mesh.m_textures_available = std::move(mesh_textures_available[i]);

        // load data to `m_textures`
        std::size_t num_textures = mesh_textures_counts[i];
        current_mesh.m_textures.resize(num_textures);
    
        /// TBD: can this O(n^2) be optimized? profile and decide
        /// use single loop instead
        for (std::size_t j = 0; j < num_textures; j++) {
            current_mesh.m_textures[j].type = *next_texture_type_it;
            next_texture_type_it++;

            std::size_t str_len = *next_texture_path_len_it;
            current_mesh.m_textures[j].path.resize(str_len);
            next_texture_path_len_it++;
            
            std::move(next_texture_path_char_it, next_texture_path_char_it + str_len, current_mesh.m_textures[j].path.begin());
            next_texture_path_char_it += str_len;
        }
    }

    return model_data;
}

/// --------------------------------------------------------------------------------------------------------------------
