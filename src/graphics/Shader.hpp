#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(std::string const& vertexPath, std::string const& fragmentPath);

    void Activate();

    template<typename T>
    void SetUniform(const std::string& name, const T& value)
    {
        // if constexpr (std::is_same_v<T, Mat44>)
        // {
        //     glUniformMatrix4fv(glGetUniformLocation(mId, name.c_str()), 1, GL_TRUE, (GLfloat*)value.m);
        // }
        // if constexpr (std::is_same_v<T, Vec3>)
        // {
        //     glUniform3fv(glGetUniformLocation(mId, name.c_str()), 1, (GLfloat*)&value);
        // }
        if constexpr (std::is_same_v<T, glm::mat4>)
        {
            glUniformMatrix4fv(glGetUniformLocation(mId, name.c_str()), 1, GL_FALSE, &value[0][0]);
        }
        if constexpr (std::is_same_v<T, glm::vec3>)
        {
            glUniform3fv(glGetUniformLocation(mId, name.c_str()), 1, &value[0]); 
        }
    }

private:
    GLuint mId;
};
