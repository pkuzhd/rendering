#version 330 core
out vec4 FragColor;

in vec3 TexCoord;
in float depth;

uniform float width;
uniform float height;

uniform int idx;

in float weight;

void main()
{
    FragColor = vec4(TexCoord.xyz, weight);
//    if (weight < 0.1) {
//        FragColor.rgb = vec3(1.0, 0.0, 0.0);
//        FragColor.a = 1.0;
//    }
//    if (weight > 0.1) {
//        FragColor.rgb = vec3(0.0, 1.0, 0.0);
//        FragColor.a = 1.0;
//    }
//    if (weight == 0.1) {
//        FragColor.rgb = vec3(0.0, 0.0, 1.0);
//        FragColor.a = 1.0;
//    }
//    if (isnan(weight)) {
//        FragColor.rgb = vec3(1.0, 1.0, 0.0);
//        FragColor.a = 1.0;
//    }
//    if (isinf(weight)) {
//        FragColor.rgb = vec3(1.0, 0.0, 1.0);
//        FragColor.a = 1.0;
//    }
    //    if (idx == 2 && weight > 90)
//        FragColor.rgb = vec3(1.0, 0.0, 0.0);
//    if (idx == 0)
//    FragColor.rgb = vec3(1.0, 0.0, 0.0);
//    if (idx == 1)
//    FragColor.rgb = vec3(1.0, 1.0, 0.0);
//    if (idx == 2)
//    FragColor.rgb = vec3(0.0, 0.0, 1.0);
//    if (idx == 3)
//    FragColor.rgb = vec3(1.0, 0.0, 1.0);
//    if (idx == 4)
//    FragColor.rgb = vec3(0.0, 1.0, 0.0);
}

