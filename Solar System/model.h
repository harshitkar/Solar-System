#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2/SOIL2.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

using namespace std;

// Function declarations
GLint TextureFromFile(const char* path, string directory);
std::string correctTexturePath(const std::string& processedPath);
std::string getLastDirectory(const std::string& path);

// Texture map and directory name map
std::map<std::string, std::string> textureNameMap = {
    {"h_TEXTURE_CM.tga", "Earth_TEXTURE_CM.tga"},
    {"e.jpg", "space.jpg"},
    {"3_Sun_diff.jpg", "13913_Sun_diff.jpg"},
    {"ury-texture.jpeg", "mercury-texture.jpeg"},
    {"s-texture.jpg", "venus-texture.jpg"},
    {"-texture.jpg", "mars-texture.jpg"},
    {"ter-texture.jpg", "jupiter-texture.jpg"},
    {"rn_diff.jpg", "Saturn_diff.jpg"},
    {"7_Uranus_planet_diff.JPG", "13907_Uranus_planet_diff.jpg"},
    {"8_Neptune_planet_diff.jpg", "13908_Neptune_planet_diff.jpg"}
};

// Function to correct texture path
std::string correctTexturePath(const std::string& processedPath) {
    if (textureNameMap.find(processedPath) != textureNameMap.end()) {
        std::string correctedPath = textureNameMap[processedPath];
        if (!correctedPath.empty()) {
            return correctedPath; // Return the correct path if defined
        }
    }
    return processedPath; // Return the original path if no correction
}

// Function to get the last directory name from a path
std::string getLastDirectory(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');

    if (lastSlash == std::string::npos) {
        return path; // No directories in the path
    }

    std::string directory = path.substr(lastSlash + 1);
    size_t lastDot = directory.find_last_of('.');

    if (lastDot != std::string::npos) {
        directory = directory.substr(0, lastDot); // Remove the file extension
    }

    return directory;
}

// Function to load a texture from file
GLint TextureFromFile(const char* path, string directory) {
    string filename = string(path);

    filename = correctTexturePath(filename);
    filename = directory + '/' + filename;

    // Check if the last directory is "space"
    if (getLastDirectory(directory) == "space") {
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &nrChannels, SOIL_LOAD_RGB);

    if (!image) {
        std::cerr << "Failed to load texture at " << filename << std::endl;
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    return textureID;
}

// Model class definition
class Model {
public:
    Model(GLchar *path) {
        this->loadModel(path);
    }

    void Draw(Shader shader) {
        for (GLuint i = 0; i < this->meshes.size(); i++) {
            this->meshes[i].Draw(shader);
        }
    }

private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;

    void loadModel(string path) {
        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        this->directory = path.substr(0, path.find_last_of('/'));
        this->processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }

        for (GLuint i = 0; i < node->mNumChildren; i++) {
            this->processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        bool applyTexture = true; // Default to true

        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (GLuint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
        return loadMaterialTextures(mat, type, typeName, this->directory);
    }

    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, string directory) {
        vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            GLboolean skip = false;

            for (GLuint j = 0; j < textures_loaded.size(); j++) {
                if (textures_loaded[j].path == str) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip) {
                // Skip loading texture if the last directory is "space"
                if (getLastDirectory(directory) != "space") {
                    Texture texture;
                    texture.id = TextureFromFile(str.C_Str(), directory);
                    texture.type = typeName;
                    texture.path = str;
                    textures.push_back(texture);

                    this->textures_loaded.push_back(texture);
                }
            }
        }

        return textures;
    }
};