#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 K_inv;
uniform mat4 R_inv;

uniform float width;
uniform float height;

uniform sampler2D texture2;

void main()
{
    gl_Position = projection * view * model
    * R_inv * K_inv * vec4((aPos.x + 0.5) * width, (aPos.y + 0.5) * height, -texture(texture2, aTexCoord).r*1.0, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

