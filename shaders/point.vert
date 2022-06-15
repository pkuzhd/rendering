#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;

out vec3 TexCoord;
out float depth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform float width;
uniform float height;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f);
    depth = -(view * model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f)).z;
    TexCoord = aTexCoord;
}

