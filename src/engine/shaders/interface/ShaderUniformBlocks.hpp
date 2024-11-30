#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

constexpr unsigned int UBLOCK_OFFSET_BEGIN = 0;

struct ShaderUniformBlocks {
	void init() {
        /// Matrices
        glGenBuffers(1, &ubo_matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
        // GL_DYNAMIC_DRAW as MVP and model matrix keep updating
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), &matrices, GL_DYNAMIC_DRAW); 
        glBindBufferBase(GL_UNIFORM_BUFFER, bp_matrices, ubo_matrices);
        
        /// GUI State
        glGenBuffers(1, &ubo_gui_state);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo_gui_state);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(GUIState), &gui_state, GL_DYNAMIC_DRAW); 
        glBindBufferBase(GL_UNIFORM_BUFFER, bp_gui_state, ubo_gui_state);
    }

	// binding=0
	struct Matrices {
		glm::mat4 view;
		glm::mat4 projection;
	};

	// binding=1
	struct Camera {
		// avoid using vec3
		glm::vec4 view_pos;
	};

	// binding=2

	// binding=3
	struct GUIState {
		float ambient_strength;
		int attenuation; // <bool>
	};

public:
    Matrices matrices;
    unsigned int bp_matrices = 0;
    unsigned int ubo_matrices;

    Camera camera;
    unsigned int bp_camera = 1;
    unsigned int ubo_camera;
    
    GUIState gui_state;
    unsigned int bp_gui_state = 3;
    unsigned int ubo_gui_state;

    // ub_lights lights;
    // unsigned int bp_lights;
    // unsigned int ubo_lights;

    // ub_texture_params texture_params;
    // unsigned int bp_texture_params;
    // unsigned int ubo_texture_params;

    // ub_light_params light_params;
    // unsigned int bp_light_params;
    // unsigned int ubo_light_params;
};