#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 cropCoord;
in float weight;

uniform float width;
uniform float height;
uniform int debug;

// texture samplers
uniform sampler2D rgb;
uniform sampler2D depth;
uniform sampler2D mask;

void main()
{
    FragColor = texture(rgb, TexCoord).bgra;
    FragColor.a = weight;

    float d = texture(depth, vec2(cropCoord.x, cropCoord.y)).r;
    float threshold = 1.0;
    if (debug != 1) {
        if (texture(mask, cropCoord).r < 0.9f)
        discard;
        if (d < 0.0f)
        discard;
    }
}

