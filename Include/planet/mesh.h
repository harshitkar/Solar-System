#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    
    Mesh( vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->setupMesh( );
    }
    
    void Draw(Shader shader) {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        for (GLuint i = 0; i < this->textures.size(); i++) {

            if (i >= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
                std::cerr << "Error: Exceeded maximum texture units!" << std::endl;
                break;
            }

            glActiveTexture(GL_TEXTURE0 + i);

            stringstream ss;
            string number;
            string name = this->textures[i].type;

            if (name == "texture_diffuse") {
                ss << diffuseNr++;
            }
            else if (name == "texture_specular") {
                ss << specularNr++;
            }

            number = ss.str();

            if (this->textures[i].id > 0) {
                glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
                glBindTexture(GL_TEXTURE_2D, this->textures[i].id);

                // Check for OpenGL errors
                GLenum error = glGetError();
                if (error != GL_NO_ERROR) {
                    std::cerr << "OpenGL Error during texture binding: " << error << std::endl;
                }
            }
            else {
                std::cerr << "Warning: Texture [" << name + number << "] is missing or has an invalid ID." << std::endl;
            }
        }

        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

        glBindVertexArray(this->VAO);

        if (this->indices.size() > 0) {
            glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        }
        else {
            std::cerr << "Error: No indices available for drawing." << std::endl;
        }

        glBindVertexArray(0);

        // Reset textures for non-space objects
        for (GLuint i = 0; i < this->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Check for OpenGL errors after drawing
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "OpenGL Error during mesh draw: " << error << std::endl;
            }
        }
    }


    
private:
    GLuint VAO, VBO, EBO;
    
    void setupMesh() {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

        // Vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

        // Vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);  // Unbind VAO
    }
};
