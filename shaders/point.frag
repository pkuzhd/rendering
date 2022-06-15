#version 330 core
out vec4 FragColor;

in vec3 TexCoord;
in float depth;

uniform float width;
uniform float height;


void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    // FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor = vec4(TexCoord.xyz, 1.0);
//    float d = (depth - 1.2) / 0.3;
//    float r = d;
//    float g = (d - 0.5) * 2;
//    float b = (1.0 - d);
//    FragColor = vec4(r, g, b, 0.0);
//    if (depth < 1.5)
//    discard;
}

