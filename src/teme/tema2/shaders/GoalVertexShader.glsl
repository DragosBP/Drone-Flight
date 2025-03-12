#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int type;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_texture;
out vec3 frag_color;

void main()
{
    frag_position = v_position;
    frag_normal = v_normal;
    frag_texture = v_texture;

    if (type == 0) {
        frag_color = vec3(0, 0, 0);
    } else if (type == 1) {
        frag_color = vec3(0.6f, 0, 0);
    } else {
        frag_color = vec3(1, 1, 1);
    }

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
