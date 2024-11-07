#include <fstream>
#include <functional>
#include <cassert>
#include <utility>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <engine/graphics/ModelManager.hpp>

#include <engine/graphics/lib/GraphicsHelper.hpp>
#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/ModelProcessor.hpp>
#include <engine/graphics/MeshProcessor.hpp>

#include <iostream> // DEBUG: _
#include <lib/utilities/DebugAssert.hpp>

ModelManager::ModelManager(TextureManager& tex_manager, std::string bin_dir): ref_tex_manager(tex_manager) {
    // check if mapper file exists in bin_dir else create
    std::string mapper_file = bin_dir + '/' + BIN_MAP_FNAME;

    if(!std::ifstream(mapper_file) && !std::ofstream(mapper_file))
        ASSERT_MESSAGE("Could not create mapper file");

    m_bin_dir = bin_dir;
    m_mapper_file = mapper_file;

    load_mapper_data();
}

// returns model hash id
mapper_data_map_type::iterator ModelManager::create_model_dump(const std::string& model_path, std::size_t model_hash) {
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

std::unordered_map<std::string, std::size_t> ModelManager::load_models(mapper_data_map_type models) {
    std::unordered_map<std::string, std::size_t> model_id_map;

    for(auto& [model_name, model_path] : models)
        model_id_map[model_name] = load_model(model_path);
    
    return model_id_map;
}

ModelManager::ModelData ModelManager::load_model_data(std::string model_bin_path) {        
    ModelData model_data;
    std::ifstream ifs {model_bin_path, std::ios::binary};

    // read counts
    ifs.read(reinterpret_cast<char*>(&model_data.num_vertices), sizeof(model_data.num_vertices));
    ifs.read(reinterpret_cast<char*>(&model_data.num_indices), sizeof(model_data.num_indices));
    
    // meshes vector size
    std::size_t meshes_sz;
    ifs.read(reinterpret_cast<char*>(&meshes_sz), sizeof(meshes_sz));
    model_data.meshes.resize(meshes_sz); // use resize instead of reserve so that we can loop over the meshes
    
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

ModelManager::MeshDrawData ModelManager::setup_mesh(std::string model_path, Mesh& mesh, bool gamma_correction) {
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

    // load textures
    std::string model_dir = model_path.substr(0, model_path.find_last_of('/'));
    
    for(MeshTexture& mesh_texture : mesh.m_textures) {
        std::string tex_path =  model_dir + '/' + mesh_texture.path;
    
        bool gamma_correct_texture = gamma_correction;
        if(mesh_texture.type == MeshTextureType::NORMAL)
            gamma_correct_texture = false;

        unsigned int texture_id = ref_tex_manager.texture_from_file(tex_path, gamma_correct_texture);
        mesh_draw_data.textures.push_back(std::make_pair(texture_id, mesh_texture.type));
    }

    return mesh_draw_data;
}

void ModelManager::add_model(std::string model_path, std::size_t model_hash, ModelData& model_data) {
    ModelDrawData model_draw_data;
    
    for(Mesh& mesh : model_data.meshes)
        model_draw_data.meshes.push_back(setup_mesh(model_path, mesh, model_data.gamma_correction));

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
    unsigned int diffuse_nr = 1;
    unsigned int specular_nr = 1;
    unsigned int normal_nr = 1;
    unsigned int ambient_nr = 1;

    // TODO: store this information in MeshTexturesAvailable
    
    // bool has_amb_tex = std::find_if(mesh_draw_data.textures.begin(), mesh_draw_data.textures.end(),
    //     [](const auto& pair) { return pair.second == MeshTextureType::AMBIENT; }) != mesh_draw_data.textures.end();
    // bool has_diff_tex = std::find_if(mesh_draw_data.textures.begin(), mesh_draw_data.textures.end(),
    //     [](const auto& pair) { return pair.second == MeshTextureType::DIFFUSE; }) != mesh_draw_data.textures.end();
    // bool has_normal_tex = std::find_if(mesh_draw_data.textures.begin(), mesh_draw_data.textures.end(),
    //     [](const auto& pair) { return pair.second == MeshTextureType::NORMAL; }) != mesh_draw_data.textures.end();
    bool has_spec_tex = std::find_if(mesh_draw_data.textures.begin(), mesh_draw_data.textures.end(),
        [](const auto& pair) { return pair.second == MeshTextureType::SPECULAR; }) != mesh_draw_data.textures.end();

    // if(has_spec_tex)
    //     ASSERT_MESSAGE("has spec");
    // if(has_diff_tex)
    //     ASSERT_MESSAGE("has diff");
    // if(has_normal_tex)
    //     ASSERT_MESSAGE("has normal");


    // std::cout << has_diff_tex << " " << has_diff_tex << " " << has_spec_tex << '\n';
    glUniform1i(glGetUniformLocation(shader->get_id(), "specular_tex_exists"), has_spec_tex);

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


void ModelManager::draw_model(const std::unique_ptr<Shader>& model_shader, std::size_t model_id, const CameraWrapper& camera_wrapper,
    const Transform& transform) {    
    // retrieve model data
    auto it = m_models.begin();
    assert((it = m_models.find(model_id)) != m_models.end() && "Model with given ID does not exist");
    ModelDrawData& model_draw_data = it->second;
    
    model_shader->activate();
    
    // set uniforms
    model_shader->set_uniform<glm::mat4>("model", GraphicsHelper::create_model_matrix(transform));
    model_shader->set_uniform<glm::mat4>("view", camera_wrapper.get_view_matrix());
    model_shader->set_uniform<glm::mat4>("projection", camera_wrapper.get_projection_matrix());
    
    // draw meshes
    for(MeshDrawData& mesh_draw_data : model_draw_data.meshes)
        draw_mesh(model_shader, mesh_draw_data);
}

std::size_t ModelManager::hasher(const std::string& model_path) {
    return std::hash<std::string>{}(model_path);
}