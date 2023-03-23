#version 460
#extension GL_ARB_shading_language_include : require

#include "/include/orthogrid_util.glsl"
#include "/include/frag_util.glsl"

in vec4 color;
in vec4 world_normal;
in float is_visible;
in float fade_enabled;

out vec4 shaded_color;

void main()
{
    if(is_visible < 0.0f)
    {
        discard;
    }

    vec3 n = normalize(world_normal.xyz);
    vec3 frag_to_eye = normalize(world_eye_pos.xyz - world_frag_pos.xyz);
    vec3 frag_to_light = frag_to_eye;
    vec3 r = 2.0f * dot(frag_to_light, n) * n - frag_to_light;
    vec3 diffuse = color.xyz * abs(dot(n, frag_to_eye.xyz)) * KD;
    vec3 ambient = color.xyz * KA;
    vec3 specular = vec3(1.0f) * dot(r, frag_to_eye) * KS;

    vec3 outColor = (ambient + diffuse + specular) * (fade_enabled > 0 ? GetFading() : 1.0f);
    shaded_color = vec4(outColor, 1.0f);
}
