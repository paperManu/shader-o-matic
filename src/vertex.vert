#version 150 core

in vec4 vVertex;
in vec2 vTexCoord;

uniform mat4 vMVP;
uniform vec2 vMouse;

smooth out vec2 finalTexCoord;

void main(void)
{
    gl_Position.xyz = (vMVP*vVertex).xyz;

    finalTexCoord = vTexCoord;
}
