#include "teme/tema2/tema2_object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

std::vector<VertexFormat> CubeVertices(glm::vec3 cube_center, float deltaH, glm::vec3 color, std::vector<VertexFormat> source) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(cube_center + glm::vec3(-deltaH, -deltaH,  deltaH), color),
        VertexFormat(cube_center + glm::vec3(deltaH, -deltaH,  deltaH), color),
        VertexFormat(cube_center + glm::vec3(-deltaH,  deltaH,  deltaH), color),
        VertexFormat(cube_center + glm::vec3(deltaH,  deltaH,  deltaH), color),
        VertexFormat(cube_center + glm::vec3(-deltaH, -deltaH, -deltaH), color),
        VertexFormat(cube_center + glm::vec3(deltaH, -deltaH, -deltaH), color),
        VertexFormat(cube_center + glm::vec3(-deltaH,  deltaH, -deltaH), color),
        VertexFormat(cube_center + glm::vec3(deltaH,  deltaH, -deltaH), color),
    };

    source.insert(source.end(), vertices.begin(), vertices.end());
    return source;
}

std::vector<unsigned int> CubeIndices(int nr, std::vector<unsigned int> source) {
    std::vector<unsigned int> indices = {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    for (int i = 0; i < indices.size(); i++) {
        indices[i] += 8 * nr;
    }

    source.insert(source.begin(), indices.begin(), indices.end());

    return source;
}

Mesh* tema2_object3D::CreateDrone(std::string name, float size) {
    float length = size;
    float width = length / 10;
    float height = width;
    
    float deltaL = length / 2;
    float deltaW = width / 2;
    float deltaH = height / 2;

    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 color = glm::vec3(0.772, 0.772, 0.772);

    std::vector<glm::vec3> cubes_center = {
        glm::vec3(-1 * (deltaL - deltaH), height, 0),
        glm::vec3(deltaL - deltaH, height, 0),
        glm::vec3(0, height, deltaL - deltaH),
        glm::vec3(0, height, -(deltaL - deltaH))
    };

    std::vector<VertexFormat> vertices
    {
        // Bottom site of the cross
        VertexFormat(center + glm::vec3(-deltaW, -deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, -deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(-deltaW, -deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, -deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaL, -deltaH,  -deltaW), color),
        VertexFormat(center + glm::vec3(+deltaL, -deltaH,  +deltaW), color),
        VertexFormat(center + glm::vec3(-deltaL, -deltaH,  +deltaW), color),
        VertexFormat(center + glm::vec3(-deltaL, -deltaH,  -deltaW), color),

        // Top sde of the cross
        VertexFormat(center + glm::vec3(-deltaW, +deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, +deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(-deltaW, +deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, +deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaL, +deltaH,  -deltaW), color),
        VertexFormat(center + glm::vec3(+deltaL , +deltaH,  +deltaW), color),
        VertexFormat(center + glm::vec3(-deltaL, +deltaH,  +deltaW), color),
        VertexFormat(center + glm::vec3(-deltaL, +deltaH,  -deltaW), color),
    };

    std::vector<unsigned int> indices = {
        // Bottom side
        0, 1 ,2,        0, 3, 1,
        4, 5, 6,        4, 6, 7,
        //0, 1 ,2,        0, 3, 1,

        
        // End sides
        0, 8, 3,        8, 11, 3,
        4, 12, 5,       12, 13, 5,
        1, 9, 2,        9, 10, 2,
        6, 14, 7,       14, 15, 7,
        //0, 4, 1,        4, 5, 1,
        //2, 6, 3,        6, 7, 3,

        // Lateral side
        0, 2, 10,       10, 8, 0,
        1, 3, 11,       11, 9, 1,
        4, 7, 15,       15, 12, 4,
        6, 5, 14,       14, 13, 5,
        //0, 3, 7,        7, 4, 0,
        //2, 1, 6,        6, 5, 1,

        // Top side
        8, 9 ,10,       8, 11, 9,
        12, 13, 14,     12, 14, 15,
        //4, 5, 6,        4, 6, 7,
    };

    // The cubes on top
    for (int i = 0; i < cubes_center.size(); i++) {
        vertices = CubeVertices(cubes_center[i], deltaH, color, vertices);
        indices = CubeIndices(i + 2, indices);
    }

    Mesh* tmp = new Mesh(name);
    tmp->InitFromData(vertices, indices);
    return tmp;
}

Mesh* tema2_object3D::CreatePropeler(std::string name, float size) {
    float length = size / 3;
    float width = length / 10;
    float height = width;

    length *= 1.2;

    float deltaL = length / 2;
    float deltaW = width / 2;
    float deltaH = height / 2;


    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 color = glm::vec3(0, 0, 0);

    std::vector<VertexFormat> vertices = 
    {
        // Bottom site of the cross
        VertexFormat(center + glm::vec3(-deltaW, -deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, -deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(-deltaW, -deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, -deltaH,  -deltaL), color),

        // Top sde of the cross
        VertexFormat(center + glm::vec3(-deltaW, +deltaH,  -deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, +deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(-deltaW, +deltaH,  +deltaL), color),
        VertexFormat(center + glm::vec3(+deltaW, +deltaH,  -deltaL), color),
    };

    std::vector<unsigned int> indices = {
        // Bottom side
        0, 1 ,2,        0, 3, 1,

        // End sides
        0, 4, 3,        4, 7, 3,
        1, 5, 2,        9, 6, 2,

        // Lateral side
        0, 2, 6,        6, 4, 0,
        1, 3, 7,        7, 5, 1,

        // Top side
        4, 5 ,6,        4, 7, 5,
    };

    Mesh* tmp = new Mesh(name);
    tmp->InitFromData(vertices, indices);
    return tmp;
}

Mesh* tema2_object3D::CreateTerrain(int N, int M, float size) {
    glm::vec3 bottom_right_corner = glm::vec3((float)N / 2.0f * size, 0, (float)M / 2.0f * size);

    std::vector<VertexFormat> vertices;
    glm::vec3 color = glm::vec3(0, 0.4, 0);

    //printf("%.2f %.2f\n\n", bottom_right_corner.x, bottom_right_corner.z);

    for (int i = 0; i < N + 1; i++) {
        for (int j = 0; j < M + 1; j++) {
            vertices.push_back(VertexFormat(bottom_right_corner - glm::vec3(size * j, 0, size * i), color));
        }
    }

    std::vector<unsigned int> indices;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            indices.push_back(i * (M + 1) + j);
            indices.push_back(i * (M + 1) + j + 1);
            indices.push_back((i + 1) * (M + 1) + j);

            indices.push_back((i + 1) * (M + 1) + j);
            indices.push_back(i * (M + 1) + j + 1);
            indices.push_back((i + 1) * (M + 1) + j + 1);
        }
    }

    Mesh* tmp = new Mesh("terrain");
    tmp->InitFromData(vertices, indices);
    return tmp;
}

Mesh* tema2_object3D::CreateTree(float size, int nr_layers) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Trunk part of the tree
    float trunk_d = size / 1.2;
    float trunk_r = trunk_d / 2;
    float trunk_h = trunk_d * 2.5;

    glm::vec3 trunk_color = glm::vec3(0.2509, 0.1568, 0.0549);
    for (int i = 0; i <= 360 * 2; i++) {
        float x = trunk_r * cos(RADIANS(i));
        float z = trunk_r * sin(RADIANS(i));

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), trunk_color));
        vertices.push_back(VertexFormat(glm::vec3(x, trunk_h, z), trunk_color));
    }

    for (int i = 0; i < 360 * 2; i++) {
        indices.push_back(i);
        indices.push_back(i + 2);
        indices.push_back(i + 1);

        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
    }

    // Leaves
    float leaf_d = 2 * size;
    float leaf_r = leaf_d / 2;
    float leaf_h = leaf_d;

    float dist = leaf_h / 3;
    
    glm::vec3 leaf_color = glm::vec3(0, 0.5, 0);

    for (int i = 0; i < nr_layers; i++) {
        float base = trunk_h + i * dist;
        int start = vertices.size();
        
        vertices.push_back(VertexFormat(glm::vec3(0, base + leaf_h, 0), leaf_color));

        for (int j = 0; j <= 360; j++) {
            float x = leaf_r * cos(RADIANS(j));
            float z = leaf_r * sin(RADIANS(j));

            vertices.push_back(VertexFormat(glm::vec3(x, base, z), leaf_color));
        }

        for (int j = start + 1; j < start + 1 + 360; j++) {
            indices.push_back(start);
            indices.push_back(j);
            indices.push_back(j + 1);
        }

        start += 360;
    }

    Mesh* tmp = new Mesh("tree");
    tmp->InitFromData(vertices, indices);
    return tmp;
}   

Mesh* tema2_object3D::CreateRock(float size) {
    float width = size;
    float length = width * 2;
    float height = width;
    glm::vec3 color = glm::vec3(0.5, 0.5, 0.5);

    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 0, length), color),
        VertexFormat(glm::vec3(width, 0, length), color),
        VertexFormat(glm::vec3(0, height, length), color),
        VertexFormat(glm::vec3(width, height, length), color),
        VertexFormat(glm::vec3(0, 0, 0), color),
        VertexFormat(glm::vec3(width, 0, 0), color),
        VertexFormat(glm::vec3(0, height, 0), color),
        VertexFormat(glm::vec3(width, height, 0), color),
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    Mesh* tmp = new Mesh("rock");
    tmp->InitFromData(vertices, indices);
    return tmp;
}

Mesh* tema2_object3D::CreateGoal() {
    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 color = glm::vec3(0, 0, 0);

    float width = 0.2f;
    float length = 2;
    float deltaL = length / 2;
    float deltaW = width / 2;

   
    std::vector<VertexFormat> vertices = {
       // Bottom site of the bottom line
       VertexFormat(center + glm::vec3(+deltaL, -width,  -deltaW), color), // 0
       VertexFormat(center + glm::vec3(+deltaL, -width,  +deltaW), color), // 1
       VertexFormat(center + glm::vec3(-deltaL, -width,  +deltaW), color), // 2
       VertexFormat(center + glm::vec3(-deltaL, -width,  -deltaW), color), // 3

       // Top side of the bottom line
       VertexFormat(center + glm::vec3(+deltaL, 0,  -deltaW), color), // 4
       VertexFormat(center + glm::vec3(+deltaL, 0,  +deltaW), color), // 5
       VertexFormat(center + glm::vec3(-deltaL, 0,  +deltaW), color), // 6
       VertexFormat(center + glm::vec3(-deltaL, 0,  -deltaW), color), // 7


       // Bottom side of the left line
       VertexFormat(center + glm::vec3(+deltaL + width, -width,  +deltaW), color), // 8
       VertexFormat(center + glm::vec3(+deltaL + width, -width,  -deltaW), color), // 9

       // Top sode of the left line
       VertexFormat(center + glm::vec3(+deltaL, length + width,  +deltaW), color), // 10
       VertexFormat(center + glm::vec3(+deltaL, length + width,  -deltaW), color), // 11
       VertexFormat(center + glm::vec3(+deltaL + width, length + width, +deltaW), color), // 12
       VertexFormat(center + glm::vec3(+deltaL + width, length + width,  -deltaW), color), // 13


       // Bottom side of the right line
       VertexFormat(center + glm::vec3(-deltaL - width, -width,  -deltaW), color), // 14
       VertexFormat(center + glm::vec3(-deltaL - width, -width,  +deltaW), color), // 15

       // Top side of the left line
       VertexFormat(center + glm::vec3(-deltaL, length + width,  +deltaW), color), // 16
       VertexFormat(center + glm::vec3(-deltaL, length + width,  -deltaW), color), // 17
       VertexFormat(center + glm::vec3(-deltaL - width, length + width,  -deltaW), color), // 18
       VertexFormat(center + glm::vec3(-deltaL - width, length + width,  +deltaW), color), // 19

      
       // Bottom side of top lane
       VertexFormat(center + glm::vec3(+deltaL, length,  -deltaW), color), // 20
       VertexFormat(center + glm::vec3(+deltaL, length,  +deltaW), color), // 21
       VertexFormat(center + glm::vec3(-deltaL, length,  +deltaW), color), // 22
       VertexFormat(center + glm::vec3(-deltaL, length,  -deltaW), color), // 23
    };

    std::vector<unsigned int> indices = {
        // Bottom line
        0, 1, 2,        0, 3, 2,
        0, 1, 4,        4, 5, 1,
        0, 3, 7,        7, 4, 3,
        1, 2, 5,        5, 6, 2,
        3, 7, 2,        2, 6, 7,
        4, 5, 6,        4, 7, 2,

        // Left line
        9, 1, 0,        8, 1, 9,
        9, 0, 11,       11, 13, 9,
        0, 1, 10,       10, 11, 0,
        1, 8, 10,       10, 12, 8,
        8, 9, 13,       13, 12, 8,
        13, 11, 10,     10, 13, 12,

        // Right line
        3, 15, 14,      3, 15, 2,
        3, 17, 14,      14, 18, 17,
        14, 15, 18,     18, 19, 15,
        2, 15, 19,      19, 2, 16,
        2, 3, 17,       17, 16, 2,
        17, 16, 19,     19, 18, 17,

        // Top line
        20, 21, 22,     22, 23, 20,
        20, 11, 23,     23, 17, 11,
        23, 17, 16,     16, 22, 23,
        22, 21, 10,     10, 16, 22,
        21, 20, 11,     11, 10, 21,
        10, 11, 16,     16, 17, 11
    };

    Mesh* tmp = new Mesh("goal");
    tmp->InitFromData(vertices, indices);
    return tmp;
}

Mesh* tema2_object3D::CreateArrow(float size) {
    float deltaS = size / 2;
    glm::vec3 color = glm::vec3(0.6, 0, 0);

    std::vector<VertexFormat> vertices = {
        // Square Part
        VertexFormat(glm::vec3(0, -size, deltaS), color),
        VertexFormat(glm::vec3(0, -size, -deltaS), color),
        VertexFormat(glm::vec3(0, 0, -deltaS), color),
        VertexFormat(glm::vec3(0, 0, deltaS), color),

        // Triangle part
        VertexFormat(glm::vec3(0, 0, size), color),
        VertexFormat(glm::vec3(0, 0, -size), color),
        VertexFormat(glm::vec3(0, size + deltaS, 0), color),

    };

    std::vector<unsigned int> indices = {
        // Square Part
        0, 1, 2,
        2, 3, 0,

        // Triangle part
        4, 5, 6
    };

    Mesh* tmp = new Mesh("goal_arrow");
    tmp->InitFromData(vertices, indices);
    return tmp;
}
