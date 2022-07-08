#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out float weight;
out float flag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 K_inv;
uniform mat4 R_inv;

uniform vec3 centers[5];
uniform vec3 center;
uniform int idx;

uniform float width;
uniform float height;

uniform sampler2D depth;

void main()
{
    float d = texture(depth, vec2(aTexCoord.x, 1.0f-aTexCoord.y)).r;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    vec2 uv = vec2(aPos.x * width, aPos.y * height);
    vec4 Xc = K_inv * vec4(uv, 1.0f, 1.0f);
    vec4 Xw = R_inv * vec4(Xc.xyz * d, 1.0f);

    vec4 Xv = view * vec4(Xw.x, -Xw.y, -Xw.z, Xw.w);
    vec3 OV = Xv.xyz;
    vec3 OC = (view * vec4(Xw.xyz - center, 0.0f)).xyz;
    float angle = degrees(acos(max(-1.0f, min(1.0f, dot(OV, OC) / length(OV) / length(OC)))));
    weight = 180 - angle;
    weight = 180;
    float angles[5];
    float max_angle = 0.0f;
    for (int i = 0; i < 5; ++i) {
        OC = (view * vec4(Xw.xyz - centers[i], 0.0f)).xyz;
        angles[i] = degrees(acos(max(-1.0f, min(1.0f, dot(OV, OC) / length(OV) / length(OC)))));
        max_angle = max(max_angle, angles[i]);
    }
    weight = 1 - angle / max_angle;

    gl_Position = projection * view * model * vec4(Xw.x, -Xw.y, -Xw.z, Xw.w);

    if (d <= 0.001)
    flag = 0;
    else
    flag = 1;
}

