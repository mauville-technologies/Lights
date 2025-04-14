#version 410 core
out vec4 FragColor;

uniform vec4 backgroundColor;
uniform vec4 rectBounds;

void main()
{
    FragColor = backgroundColor;
}