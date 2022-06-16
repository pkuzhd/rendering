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
    vec4 Xv = view * Xw;
    gl_Position = projection * Xv;
    depth = -(view * model * vec4(aPos.x, -aPos.y, -aPos.z, 1.0f)).z;
    vec3 OV = Xv.xyz;
    vec3 OC = (view * vec4(Xw.xyz - center, 0.0f)).xyz;
    float angle = degrees(acos(max(-1.0f, min(1.0f, dot(OV, OC) / length(OV) / length(OC)))));
    weight = 180 - angle;
    weight = 180;
    float angles[5];
    float max_angle = 0.0f;
    for (int i = 0; i < 5; ++i) {
//        if (i == idx)
//            continue;
        OC = (view * vec4(Xw.xyz - centers[i], 0.0f)).xyz;
        angles[i] = degrees(acos(max(-1.0f, min(1.0f, dot(OV, OC) / length(OV) / length(OC)))));
        max_angle = max(max_angle, angles[i]);
//        if (angles[i] < angle) {
//            weight = 0.0;
//            break;
//        }
    }
    weight = 1 - angle / max_angle;
//    weight = 1;
    TexCoord = aTexCoord;
}

