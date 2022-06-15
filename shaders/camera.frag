#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float width;
uniform float height;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor = texture(texture1, TexCoord);
    vec2 up = vec2(0.0f, 1.0f / height);
    vec2 down = vec2(0.0f, -1.0f / height);
    vec2 left = vec2(1.0f / width, 0.0f);
    vec2 right = vec2(1.0f / width, 0.0f);
    float threshold = 0.12;
    if (
    texture(texture2, TexCoord).r < threshold
    || texture(texture2, TexCoord + up).r < threshold
    || texture(texture2, TexCoord + down).r < threshold
    || texture(texture2, TexCoord + left).r < threshold
    || texture(texture2, TexCoord + right).r < threshold
    )
    discard;
}

