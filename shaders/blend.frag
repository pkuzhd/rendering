#version 330 core
out vec4 FragColor;

in vec3 TexCoord;
in float depth;

uniform float width;
uniform float height;


void main()
{
    FragColor = vec4(TexCoord.xyz, 1.0);
}

