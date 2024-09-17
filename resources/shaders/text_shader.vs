#version 330 core

layout (location = 0) in vec4 vertex; // Position and texture coordinates

out vec2 TexCoords; // Output texture coordinates to fragment shader

uniform mat4 model;       // Model matrix for positioning and scaling
uniform mat4 projection;  // Projection matrix (usually orthographic for 2D rendering)

void main()
{
    // Transform the vertex position using the model and projection matrices
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw; // Pass texture coordinates to the fragment shader
}
