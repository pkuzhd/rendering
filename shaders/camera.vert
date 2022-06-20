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

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

void main()
{
    float depth = texture(texture2, aTexCoord).r;

    vec2 uv = vec2(aPos.x * width, aPos.y * height);
    vec4 Xc = K_inv * vec4(uv, 1.0f, 1.0f);
    vec4 Xw = R_inv * vec4(Xc.xyz * depth, 1.0f);
    gl_Position = projection * view * model * vec4(Xw.x, -Xw.y, -Xw.z, Xw.w);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

