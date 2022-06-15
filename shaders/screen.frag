#version 330 core

uniform sampler2D sampler;

in vec2 texVar;
out vec4 color;

void main() {
    vec4 premul = texture(sampler, texVar);
    color = premul;
//    color = vec4(1.0, 0.0, 0.0, 1.0);
//    color.rgb = premul.rgb / premul.a;
//    color.a = premul.a;
}