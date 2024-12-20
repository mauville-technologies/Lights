#version 460 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 texCoord;

uniform sampler2D inTexture;
uniform sampler2D inTexture2;

void main()
{
//    FragColor = mix(texture(inTexture, texCoord), texture(inTexture2, texCoord), 0.4);
    // Red
    FragColor = vec4(1, 0, 0, 1);
}