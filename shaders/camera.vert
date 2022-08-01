#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 cropCoord;
out float weight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 K_inv;
uniform mat4 R_inv;

uniform vec3 centers[5];
uniform vec3 center;
uniform int idx;
uniform int debug;
uniform int imgcrop;

uniform float width;
uniform float height;
uniform float w_crop;
uniform float h_crop;
uniform float x_crop;
uniform float y_crop;

uniform sampler2D depth;

void main()
{
    float d = texture(depth, vec2(aPos.x, aPos.y)).r;
    if (debug == 1) {
        d = 1;
    }
    TexCoord = vec2((aPos.x * w_crop + x_crop) / width, (aPos.y * h_crop + y_crop) / height);
    cropCoord = vec2(aPos.x, aPos.y);
    if (imgcrop == 1) {
        TexCoord = cropCoord;
    }
    vec2 uv = vec2((aPos.x * w_crop + x_crop) / width, (aPos.y * h_crop + y_crop) / height);
    vec4 Xc = K_inv * vec4(uv, 1.0f, 1.0f);
    vec4 Xw = R_inv * vec4(Xc.xyz * d, 1.0f);

    vec3 OV = (view * vec4(Xw.x, Xw.y, Xw.z, 0.0f)).xyz;
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
    weight = 1 - angle / max_angle + 0.001;

    weight = 1;

    vec4 Xv = view * model * vec4(Xw.x, Xw.y, Xw.z, Xw.w);
    gl_Position = projection * vec4(Xv.x, -Xv.y, -Xv.z, Xv.w);
}

