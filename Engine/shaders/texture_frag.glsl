#version 460

out vec4 shaded_color;

in vec3 frag_tex_coord;
in vec4 color;

uniform sampler3D ourTexture;

void main()
{
    shaded_color = texture(ourTexture, frag_tex_coord);
    //shaded_color = color;

}
