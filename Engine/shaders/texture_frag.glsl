#version 460

out vec4 shaded_color;

in vec3 frag_tex_coord;

uniform sampler3D ourTexture;

const float DISCARD_EPSILON = 0.01f;

void main()
{
    shaded_color = texture(ourTexture, frag_tex_coord);
    if(length(shaded_color.xyz) < DISCARD_EPSILON)
    {
        discard;
    }
}
