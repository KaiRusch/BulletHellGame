#version 150

in vec2 pvPosition;
in vec2 pvUvCoords;

out vec2 vfUvCoords;

uniform mat4 mvp;

void main()
{
    vfUvCoords = pvUvCoords;
    gl_Position = mvp*vec4(pvPosition,0.0,1.0);
}


