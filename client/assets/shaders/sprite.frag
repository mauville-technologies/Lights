#version 410 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 texCoord;

uniform sampler2D inTexture;

void main()
{
    FragColor = texture(inTexture, texCoord);
}