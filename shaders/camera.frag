#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float weight;

uniform float width;
uniform float height;

// texture samplers
uniform sampler2D rgb;
uniform sampler2D depth;
uniform sampler2D mask;

void main()
{
    FragColor = texture(rgb, TexCoord);
    FragColor.a = weight;
//    vec2 up = vec2(0.0f, 3.0f / height);
//    vec2 down = vec2(0.0f, -3.0f / height);
//    vec2 left = vec2(-3.0f / width, 0.0f);
//    vec2 right = vec2(3.0f / width, 0.0f);
    float threshold = 1.0;
    if (texture(mask, TexCoord).r < 0.5f)
    discard;
//    if (texture(depth, TexCoord).r < 0.0f)
//    discard;
//    if (
//    texture(mask, TexCoord).r < threshold
//    || texture(mask, TexCoord + up).r < threshold
//    || texture(mask, TexCoord + down).r < threshold
//    || texture(mask, TexCoord + left).r < threshold
//    || texture(mask, TexCoord + right).r < threshold
//    )
//    discard;
//    if (TexCoord.x < 0.3 && TexCoord.y < 0.6)
//        discard;
}

