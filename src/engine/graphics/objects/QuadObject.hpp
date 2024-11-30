#pragma once

#include <glad/glad.h>

struct QuadObject {
    const unsigned int len_pos = 3;
    const unsigned int len_tex_coords = 2;

    const int attrib_pos_begin = 0;
    const int attrib_tex_coords_begin = 3 * sizeof(float);
    
    const int stride = 5 * sizeof(float);

    float vertices[5 * 4] = {
        // positions            // texture Coords
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
    };

    void init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        // vertex positions (location=0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, len_pos, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(attrib_pos_begin));
        
        // texture coordinates (location=1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, len_tex_coords, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(attrib_tex_coords_begin));
    }

    unsigned int VAO;
    unsigned int VBO;
};