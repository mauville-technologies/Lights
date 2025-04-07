#version 410 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D inTexture;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(inTexture, texCoord).r);
    FragColor = vec4(textColor, 1.0) * sampled;
}