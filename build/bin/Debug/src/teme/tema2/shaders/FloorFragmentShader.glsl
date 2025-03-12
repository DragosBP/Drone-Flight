#version 330
// Input
in vec3 frag_position;
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_texture;

in float noise;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 new_color = vec3(frag_color.x, frag_color.y - 0.2, frag_color.z);
    out_color = vec4(mix(new_color, frag_color, noise), 1);
}
