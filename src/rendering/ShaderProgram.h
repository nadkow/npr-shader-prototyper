#ifndef OPENGLGP_SHADER_H
#define OPENGLGP_SHADER_H

#include <string>
#include <iostream>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <stdio.h>
#include <fstream>

class ShaderProgram
{
public:
    unsigned int ID;

    ShaderProgram() = default;

    ShaderProgram(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        //	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            spdlog::error("SHADER::FILE_NOT_SUCCESFULLY_READ");
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);

        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        glDetachShader(ID, vertex);
        glDetachShader(ID, fragment);
        glDeleteShader(vertex);
        glDeleteShader(fragment);

    }

    ShaderProgram(const char* fragmentPath) : ShaderProgram("res/shaders/flat.vert", fragmentPath) {}

    uint32_t recompile(const char* path) {
        uint32_t newShaderHandle = ShaderProgram(path).ID;

        // Return old shader if compilation failed
        if (newShaderHandle == -1)
            return ID;

        glDeleteProgram(ID);
        std::cout << "recompiled successfully\n";
        ID = newShaderHandle;
        return newShaderHandle;
    }

    void use()
    {
        glUseProgram(ID);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setVec2(const std::string &name, const glm::vec2 &vec) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y);
    }

    void setVec3(const std::string &name, const glm::vec3 &vec) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
    }

    void setVec3(const std::string &name, const glm::vec4 &vec) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
    }

    void setVec4(const std::string &name, const glm::vec4 &vec) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
    }

    void set1f(const std::string &name, const float f) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), f);
    }

    void setInt(const std::string &name, const int i) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), i);
    }

private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#endif //OPENGLGP_SHADER_H
