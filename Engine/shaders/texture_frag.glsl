#version 460

out vec4 shaded_color;

in vec3 frag_tex_coord;

in float is_visible;

uniform sampler3D ourTexture;

const float DISCARD_EPSILON = 0.01f;

// TODO: Move to SSBO, editable from application.
const float OPACITY = 0.6f;

void main()
{
    shaded_color = vec4(texture(ourTexture, frag_tex_coord).xyz, OPACITY);
    if(length(shaded_color.xyz) < DISCARD_EPSILON || is_visible < 0.0f)
    {
        discard;
    }
}
