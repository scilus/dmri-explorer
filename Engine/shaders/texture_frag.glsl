#version 460

out vec4 shaded_color;

in vec3 frag_tex_coord;

uniform sampler3D ourTexture;

void main()
{
    shaded_color = texture(ourTexture, frag_tex_coord);
    if(shaded_color.x<0.01f && shaded_color.y<0.01f && shaded_color.z<0.01f)
    {
        discard;
    }
}
