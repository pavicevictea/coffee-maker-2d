#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 uPos;
uniform float uScaleX;
uniform float uScaleY;
uniform float uLevel;

void main()
{
    vec2 scaledPos = aPos;
    scaledPos.x *= uScaleX;
    scaledPos.y *= uScaleY * uLevel;
    
    vec2 finalPos = scaledPos + uPos;
    
    gl_Position = vec4(finalPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}