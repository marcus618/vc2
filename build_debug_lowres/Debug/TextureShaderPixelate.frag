in vec2 UV;

out vec4 FragColor;

uniform sampler2D myTextureSampler;


void main() {
    vec2 pixelatedUV = floor(UV * (textureSize(myTextureSampler, 0) / 10)) * 10 / textureSize(myTextureSampler, 0);
    FragColor = texture(myTextureSampler, pixelatedUV);
}