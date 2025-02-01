#version 410 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 texCoord;

void main()
{
    FragColor = ourColor;
}