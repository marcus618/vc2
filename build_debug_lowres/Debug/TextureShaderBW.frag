#version 330 core
in vec2 UV;
out vec4 FragColor;

uniform sampler2D myTextureSampler;

void main() {
    vec3 color = texture(myTextureSampler, UV).rgb;
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    FragColor = vec4(vec3(gray), 1.0);
}