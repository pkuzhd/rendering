#version 330 core

in vec2 tex;
out vec2 texVar;

void main() {
    gl_Position = vec4(2 * tex - 1, 0, 1);
    texVar = tex;
}