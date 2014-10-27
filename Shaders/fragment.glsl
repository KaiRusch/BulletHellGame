#version 150

out vec4 outColor;

in vec2 vfUvCoords;

uniform sampler2D tex;

void main()
{
    outColor = texture(tex,vfUvCoords);
}
