#version 330 core

in vec2 TexCoords; // Input texture coordinates from vertex shader

out vec4 color; // Output color of the fragment

uniform sampler2D text; // Texture atlas containing the font
uniform vec3 textColor; // Color of the text

void main()
{
    // Sample the texture atlas to get the color from the texture
    vec4 sampled = texture(text, TexCoords);
    
    // Apply the text color to the sampled color
    color = vec4(textColor, 1.0) * sampled;
}
