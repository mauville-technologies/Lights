#version 410 core
out vec4 FragColor;

in vec3 fragLocalPosition;
in vec2 texCoord;

uniform sampler2D inTexture;
uniform vec3 textColor;
uniform vec4 rectBounds;

void main()
{
    // x and y are the left and top of the rectangle
    // z and w are the right and bottom of the rectangle
    if (rectBounds != vec4(0.0) && (fragLocalPosition.x < rectBounds.x || fragLocalPosition.y < rectBounds.y ||
    fragLocalPosition.x > rectBounds.z || fragLocalPosition.y > rectBounds.w)) {
        discard;
    }

    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(inTexture, texCoord).r);
    FragColor = vec4(textColor, 1.0) * sampled;
}