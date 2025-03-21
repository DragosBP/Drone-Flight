#version 330

// Input
in vec3 world_position;
in vec3 world_normal;
in vec3 vector_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform vec3 new_light_position;

uniform bool is_spot;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms
uniform vec3 color;
uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

void point_light_contribution(vec3 light_pos, out float diffuse_light, out float specular_light) {
    vec3 N = vector_normal;
    vec3 L = normalize(new_light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    
    diffuse_light = material_kd * max(dot(N,L), 0);

    vec3 R = reflect(-L, N);

    specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
}


void main()
{
    vec3 N = normalize(vector_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize( L + V );

    float ambient_light = 0.25 * material_kd;

    float diffuse_light = material_kd * max(dot(N,L), 0);

    vec3 R = reflect(-L, N);

    float specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light == 0)
    {
        specular_light = 0;
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.

    float new_diffuse;
    float new_specular;

    point_light_contribution(new_light_position, new_diffuse, new_specular);

    float new_atenuare = 1 / (pow(distance(world_position, new_light_position), 2) + 1);

    float new_light = ambient_light + new_atenuare * (new_diffuse + new_specular);
    if (is_spot) {
        float cut_off = radians(30.0f);
        float spot_light = dot(-L, light_direction);
        if (spot_light > cos(cut_off))
        {
            float spot_light_limit = cos(cut_off);

            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);

            float light =new_light+ ambient_light + light_att_factor * diffuse_light;

            vec3 color = object_color * light;

            out_color = vec4(color, 1);
        } else {
            float light =new_light+ ambient_light;

            vec3 color = object_color * light;

            out_color = vec4(color, 1);
        }
    } else {
        float atenuare = 1 / (pow(distance(world_position, light_position), 2) + 1);

        float light = new_light+ ambient_light + atenuare * (diffuse_light + specular_light);

        vec3 color = object_color * light;

        out_color = vec4(color, 1);
    }
}
