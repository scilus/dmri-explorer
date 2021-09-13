#version 460
in vec4 v_wpos;
in vec3 v_color;
in vec4 v_normal;
in vec4 v_weye;
in float v_isVisible;

out vec4 color;

const float KA = 0.3;
const float KD = 0.6;
const float KS = 0.1;
const float IS_VISIBLE_EPSILON = 0.1f;

void main()
{
    if(v_isVisible < 1.0f - IS_VISIBLE_EPSILON)
    {
        discard;
    }

    vec3 n = normalize(v_normal.xyz);
    vec3 eye = normalize(v_weye.xyz - v_wpos.xyz);
    vec3 light = eye;
    vec3 r = 2.0f * dot(light, n) * n - light;
    vec3 diffuse = v_color * abs(dot(n, eye.xyz)) * KD;
    vec3 ambient = v_color * KA;
    vec3 specular = vec3(1.0, 1.0, 1.0) * dot(r, eye) * KS;

    vec3 outColor = ambient + diffuse + specular;
    color = vec4(outColor, 1.0f);
}
