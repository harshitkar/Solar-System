#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Character {
    GLuint TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;   // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
    TextRenderer(const std::string& atlasPath, unsigned int atlasWidth, unsigned int atlasHeight) {
        // Load the texture atlas
        glGenTextures(1, &AtlasTexture);
        glBindTexture(GL_TEXTURE_2D, AtlasTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the texture data (assuming you have a method to load it)
        unsigned char* data = nullptr; // Load your texture data here
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlasWidth, atlasHeight, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Create VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Shader initialization
        const char* vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec4 vertex;
            out vec2 TexCoords;
            uniform mat4 projection;
            void main() {
                gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
                TexCoords = vertex.zw;
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 330 core
            in vec2 TexCoords;
            out vec4 color;
            uniform sampler2D text;
            uniform vec3 textColor;
            void main() {
                vec4 texColor = texture(text, TexCoords);
                color = vec4(textColor, 1.0) * texColor;
            }
        )";

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        ShaderProgram = glCreateProgram();
        glAttachShader(ShaderProgram, vertexShader);
        glAttachShader(ShaderProgram, fragmentShader);
        glLinkProgram(ShaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    ~TextRenderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteTextures(1, &AtlasTexture);
    }

    void LoadCharacters(const std::map<char, Character>& characters) {
        Characters = characters;
    }

    void RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
        glUseProgram(ShaderProgram);
        glUniform3f(glGetUniformLocation(ShaderProgram, "textColor"), color.x, color.y, color.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AtlasTexture);
        glUniform1i(glGetUniformLocation(ShaderProgram, "text"), 0);

        glBindVertexArray(VAO);

        for (const auto& c : text) {
            Character ch = Characters.at(c);

            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;

            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Advance the cursor to the next glyph
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    std::map<char, Character> Characters;
    GLuint VAO, VBO;
    GLuint ShaderProgram;
    GLuint AtlasTexture;
};

#endif // TEXTRENDERER_H
