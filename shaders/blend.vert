#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;

out vec3 TexCoord;
out float depth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float width;
uniform float height;

uniform vec3 centers[5];
uniform vec3 center;
uniform int idx;

out float weight;

void main()
{
    vec4 Xw = model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f);
    vec4 Xc = view * Xw;
    gl_Position = projection * Xc;
    depth = -(view * model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f)).z;
    vec3 OV = Xc.xyz;
    vec3 OC = Xw.xyz - center;
    float angle = degrees(acos(dot(OV, OC) / length(OV) / length(OC)));
    weight = 180 - angle;
    weight = 180;
    float angles[5];
    for (int i = 0; i < 5; ++i) {
        if (i == idx)
            continue;
        OC = Xw.xyz - centers[i];
        angles[i] = degrees(acos(dot(OV, OC) / length(OV) / length(OC)));
        if (angles[i] < angle) {
            weight = 0.0;
            break;
        }
    }
    TexCoord = aTexCoord;
}

