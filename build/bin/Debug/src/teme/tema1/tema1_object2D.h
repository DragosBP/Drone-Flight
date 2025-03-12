#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace tema1_object2D
{

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color);
    Mesh* CreateTank(const std::string& name, glm::vec3 tankBottomCenter, int size, float color_dif);
    Mesh* CreateGun(const std::string& name, glm::vec3 tankBottomCenter, int size, glm::vec3 color);
    Mesh* CreateTrajectory(const std::string& name, int size);
    Mesh* CreateHealthBar(const std::string& name, int size, glm::vec3 color);
}
