#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 Xw = model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f);
    vec4 Xv = view * Xw;
    gl_Position = projection * Xv;
    TexCoord = aTexCoord;
}

