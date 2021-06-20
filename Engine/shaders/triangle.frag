#version 460
in vec3 v_color;
in vec4 v_normal;
in vec4 v_eye;
in vec4 v_slice_orientation;

out vec4 color;

const float ka = 0.3;
const float kd = 0.6;
const float ks = 0.1;

void main()
{
    vec3 n = normalize(v_normal.xyz);
    vec3 eye = normalize(v_eye.xyz);
    vec3 light = normalize(v_eye.xyz);
    vec3 r = 2.0f * dot(light, n) * n - light;
    vec3 diffuse = v_color * abs(dot(n, v_eye.xyz)) * kd;
    vec3 ambient = v_color * ka;
    vec3 specular = vec3(1.0, 1.0, 1.0) * dot(r, eye) * ks;

    vec3 slice_orientation = normalize(v_slice_orientation.xyz);

    float alpha = 1.0f;
    color = vec4(ambient + diffuse + specular, alpha);
}
