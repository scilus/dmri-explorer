#version 460
in vec3 v_color;
in vec4 v_normal;
in vec4 v_eye;

out vec4 color;

void main()
{
    vec3 n = normalize(v_normal.xyz);
    vec3 diffuse = v_color * abs(dot(n, v_eye.xyz)) * 0.7;
    vec3 ambient = v_color * 0.3;

    color = vec4(ambient + diffuse, 1.0);
}
