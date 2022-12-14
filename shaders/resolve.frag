#version 330 core

uniform sampler2D sampler;

in vec2 texVar;
out vec4 color;

void main() {
    vec4 premul = texture(sampler, texVar);
    color.rgb = premul.rgb / premul.a;
    color.a = premul.a;
}