#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, vec2(TexCoord.x, -TexCoord.y));
    FragColor.a = 0.00001f;
}

