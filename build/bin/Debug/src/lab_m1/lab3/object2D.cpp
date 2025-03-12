#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


static glm::vec3 ObtainColor(glm::vec3 origninal, float color_dif) {
    float r = origninal.x + color_dif;
    float g = origninal.y + color_dif;
    float b = origninal.z + color_dif;
    
    glm::vec3 res = glm::vec3(
        r > 1 ? r - 1 : r, 
        g > 1 ? g - 1 : g,
        b > 1 ? b - 1 : b
    );

    return res;
}

Mesh* object2D::CreateTank(
    const std::string& name,
    glm::vec3 tankBottomCenter,
    int size,
    float color_dif)
{

    glm::vec3 base_bottom_color = ObtainColor(glm::vec3(0.454f, 0.392f, 0.305f), color_dif);
    glm::vec3 bottom_center = tankBottomCenter;
    int tank_bottom_short_length = 20 * size;
    int tank_bottom_long_length = tank_bottom_short_length + tank_bottom_short_length / 5;
    int tank_bottom_heigth = 2 * tank_bottom_short_length / 5;

    glm::vec3 base_top_color = ObtainColor(glm::vec3(0.8f, 0.686f, 0.533f), color_dif);
    glm::vec3 top_center = bottom_center + glm::vec3(0, tank_bottom_heigth, 0);
    int tank_top_short_length = tank_bottom_long_length + 4 * size;
    int tank_top_long_length = tank_top_short_length + tank_top_short_length / 4;
    int tank_top_heigth = 2 * tank_bottom_heigth;

    glm::vec3 semicircle_center = top_center + glm::vec3(0, tank_top_heigth, 0);
    int semicircle_radius = tank_top_heigth;

    std::vector<VertexFormat> vertices =
    {
        // Bottom of the tank
        VertexFormat(bottom_center, base_bottom_color),
        VertexFormat(bottom_center + glm::vec3(tank_bottom_short_length, 0, 0), base_bottom_color),
        VertexFormat(bottom_center + glm::vec3(tank_bottom_long_length, tank_bottom_heigth, 0), base_bottom_color),
        VertexFormat(bottom_center + glm::vec3(-tank_bottom_short_length, 0, 0), base_bottom_color),
        VertexFormat(bottom_center + glm::vec3(-tank_bottom_long_length, tank_bottom_heigth, 0), base_bottom_color),

        // Top of the tank
        VertexFormat(top_center, base_top_color),
        VertexFormat(top_center + glm::vec3(tank_top_long_length, 0, 0), base_top_color),
        VertexFormat(top_center + glm::vec3(tank_top_short_length, tank_top_heigth, 0), base_top_color),
        VertexFormat(top_center + glm::vec3(-tank_top_long_length, 0, 0), base_top_color),
        VertexFormat(top_center + glm::vec3(-tank_top_short_length, tank_top_heigth, 0), base_top_color),

        // Center of the semicircle
        VertexFormat(semicircle_center, base_top_color)
    };

    // The whole semicircle
    for (int i = 0; i <= 200; i++) {
        float angle = RADIANS(i);
        float x = cos(angle) * (float)semicircle_radius;
        float y = sin(angle) * (float)semicircle_radius;
        
        vertices.push_back(VertexFormat(semicircle_center + glm::vec3(x, y, 0), base_top_color));
    }

    Mesh* tank = new Mesh(name);

    std::vector<unsigned int> indices = 
    { 
        0, 1, 2, 
        0, 4, 3,
        0, 2, 4,
        5, 6, 7,
        5, 9, 8,
        5, 7, 9
    };
    
    for (int i = 0; i < 200; i++) {
        indices.push_back(10);
        indices.push_back(10 + i);
        indices.push_back(10 + i + 1);
    }

    tank->InitFromData(vertices, indices);
    return tank;
}

Mesh* object2D::CreateGun(
    const std::string& name,
    glm::vec3 tankBottomCenter,
    int size) 
{
    int weapon_length = 20 * size + 20 * size / 5 + 4 * size;
    int weapon_girth = 3 * size;
    glm::vec3 weapon_base = tankBottomCenter + glm::vec3(0, 28 * size, 0); // 24 * size + weapon_girth + size

    glm::vec3 left_corner = weapon_base - glm::vec3(weapon_girth / 2, 0, 0);
    glm::vec3 color = glm::vec3(0.017f, 0.017f, 0.016f);
    
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(left_corner, color),
        VertexFormat(left_corner + glm::vec3(weapon_girth, 0, 0), color),
        VertexFormat(left_corner + glm::vec3(weapon_girth, weapon_length, 0), color),
        VertexFormat(left_corner + glm::vec3(0, weapon_length, 0), color)
    };

    Mesh* weapon = new Mesh(name);
    
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 0, 2 };

    weapon->InitFromData(vertices, indices);
    return weapon;
}