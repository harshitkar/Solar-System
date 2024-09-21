#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLM/glm.hpp>
#include <GL/glew.h>

class Shader {
public:
    GLuint Program;
    std::string vertexSource;
    std::string fragmentSource;

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        vShaderFile.exceptions(std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::badbit);
        try {
            // Load shader code from files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexSource = vShaderStream.str();
            fragmentSource = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
            return;
        }

        const GLchar* vShaderCode = vertexSource.c_str();
        const GLchar* fShaderCode = fragmentSource.c_str();
        GLuint vertex, fragment;
        GLint success;
        GLchar infoLog[512];

        // Compile Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Compile Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        // Link Shader Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // Clean up
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Use() {
        glUseProgram(this->Program);
    }

    void PrintSource() {
        std::cout << "Vertex Shader Source:\n" << vertexSource << std::endl;
        std::cout << "Fragment Shader Source:\n" << fragmentSource << std::endl;
    }

    // Utility functions to set uniform values with debugging
    void SetUniform1f(const std::string& name, float value) {
        GLint location = glGetUniformLocation(Program, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::SHADER::UNIFORM::NOT_FOUND: " << name << std::endl;
        }
        glUniform1f(location, value);
    }

    void SetUniform1i(const std::string& name, int value) {
        GLint location = glGetUniformLocation(Program, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::SHADER::UNIFORM::NOT_FOUND: " << name << std::endl;
        }
        glUniform1i(location, value);
    }

    void SetUniform3f(const std::string& name, float v1, float v2, float v3) {
        GLint location = glGetUniformLocation(Program, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::SHADER::UNIFORM::NOT_FOUND: " << name << std::endl;
        }
        glUniform3f(location, v1, v2, v3);
    }

    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
        GLint location = glGetUniformLocation(Program, name.c_str());
        if (location == -1) {
            std::cerr << "WARNING::SHADER::UNIFORM::NOT_FOUND: " << name << std::endl;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    }
};

#endif
