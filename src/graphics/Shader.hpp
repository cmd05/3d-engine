#pragma once

#include <glad/glad.h>
#include <string>

class Mat44;
class Vec3;


class Shader
{
public:
    Shader(std::string const& vertexPath, std::string const& fragmentPath);

    void Activate();

    template<typename T>
    void SetUniform(const std::string& name, const T& value)
    {
        if constexpr (std::is_same_v<T, Mat44>)
        {
            glUniformMatrix4fv(glGetUniformLocation(mId, name.c_str()), 1, GL_TRUE, (GLfloat*)value.m);
        }
        else if constexpr (std::is_same_v<T, Vec3>)
        {
            glUniform3fv(glGetUniformLocation(mId, name.c_str()), 1, (GLfloat*)&value);
        }
    }

private:
    GLuint mId;
};
