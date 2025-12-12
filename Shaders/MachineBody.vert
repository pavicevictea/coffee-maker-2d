#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec4 uPosScale; 

void main()
{
    vec2 scaleVec = uPosScale.zw;
    vec2 scaledPos = aPos * scaleVec; 
    vec2 finalPos = scaledPos + uPosScale.xy;
    
    gl_Position = vec4(finalPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}