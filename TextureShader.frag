#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D myTextureSampler;

void main() j{
    FragColor = texture(myTextureSampler, UV); 
}
