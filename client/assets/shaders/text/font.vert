#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragLocalPosition;
void main()
{
    fragLocalPosition = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}