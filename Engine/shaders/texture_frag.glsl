#version 460

out vec4 shaded_color;

in vec3 frag_tex_coord;

uniform sampler3D ourTexture;

void main()
{
    shaded_color = texture(ourTexture, frag_tex_coord);
    //shaded_color = vec4(1.0f,1.0f,1.0f,1.0f);

}
