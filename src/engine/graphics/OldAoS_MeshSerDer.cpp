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
        // read count of different available textures
        // `MeshTexturesAvailable` is POD
        ifs.read(reinterpret_cast<char*>(&mesh.m_textures_available), sizeof(mesh.m_textures_available));

        // resize `m_textures`
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
        // write count of different available textures
        ofs.write(reinterpret_cast<char*>(&mesh.m_textures_available), sizeof(mesh.m_textures_available));

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
