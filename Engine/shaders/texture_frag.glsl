#version 460

in vec4 world_frag_pos;
in vec4 color;
in vec4 world_normal;
in vec4 world_eye_pos;

out vec4 shaded_color;

const float KA = 0.3;
const float KD = 0.6;
const float KS = 0.1;

void main()
{
    vec3 n = normalize(world_normal.xyz);
    vec3 frag_to_eye = normalize(world_eye_pos.xyz - world_frag_pos.xyz);
    vec3 frag_to_light = frag_to_eye;
    vec3 r = 2.0f * dot(frag_to_light, n) * n - frag_to_light;
    vec3 diffuse = color.xyz * abs(dot(n, frag_to_eye.xyz)) * KD;
    vec3 ambient = color.xyz * KA;
    vec3 specular = vec3(1.0f) * dot(r, frag_to_eye) * KS;

    vec3 outColor = (ambient + diffuse + specular);
    shaded_color = vec4(outColor, 1.0f);
}
