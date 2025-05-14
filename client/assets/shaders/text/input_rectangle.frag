#version 410 core
out vec4 FragColor;
in vec3 fragLocalPosition;

uniform vec4 backgroundColor;
uniform vec4 rectBounds;
uniform vec4 borderThickness;
uniform vec4 borderColor;

void main()
{
    // Calculate distances to the edges
    float leftDist = abs(fragLocalPosition.x - rectBounds.x);
    float rightDist = abs(fragLocalPosition.x - rectBounds.z);
    float topDist = abs(fragLocalPosition.y - rectBounds.y);
    float bottomDist = abs(fragLocalPosition.y - rectBounds.w);

    // Check if the fragment is within the border region
    bool isInBorder = (leftDist < borderThickness.x || rightDist < borderThickness.z ||
                        topDist < borderThickness.y || bottomDist < borderThickness.w);

    if (isInBorder) {
        FragColor = borderColor; // Render the border
    } else {
        FragColor = backgroundColor; // Render the background
    }
}