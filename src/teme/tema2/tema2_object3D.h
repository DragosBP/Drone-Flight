#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace tema2_object3D
{
    Mesh* CreateDrone(std::string name, float size);
    Mesh* CreatePropeler(std::string name, float size);
    Mesh* CreateTerrain(int N, int M, float size);
    Mesh* CreateTree(float size, int nr_layers);
    Mesh* CreateRock(float size);
    Mesh* CreateGoal();
    Mesh* CreateArrow(float size);
}
