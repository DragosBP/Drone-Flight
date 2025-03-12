#include "teme/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include "teme/tema2/tema2_object3D.h"

using namespace std;
using namespace teme;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    } 
    
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("ColorShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "FloorVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "FloorFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("GoalShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "GoalVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::TEME, "tema2", "shaders", "GoalFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    float start_height = 7;
    gravity = -1.2;
     
    // Camera
    {
        show_collision_boxes = false;
        camera_type = FREE_CAM;
        camera = new implemented::Tema_Camera();
    }

    // Rocks
    {
        Mesh* rock = tema2_object3D::CreateRock(2.5);
        AddMeshToList(rock);
    }
    // Trees
    {
        nr_layers = 3;
        Mesh* tree = tema2_object3D::CreateTree(1, nr_layers);
        AddMeshToList(tree);
    }
    // Goals
    {
        current_goal = 0;
        show_direction = false;
        Mesh* goal = tema2_object3D::CreateGoal();
        AddMeshToList(goal);
    }

    // Terrain initialisation
    {
        float N = 200;
        float M = 200;
        float density = 0.4;
        
        terrain = tema2_object3D::CreateTerrain(100 + N, 100 + M, density);
        AddMeshToList(terrain);
        
        area_height = N * density;
        area_width = M * density;
    
        InitObstacles(10, 30, 150);
    }

    // Drone Initialisation
    {
        glm::vec3 start = glm::vec3(0, 0, 0);
        drone = new Drone(
            "drone",
            1,
            start,
            3
        );
        AddMeshToList(drone->body);
        AddMeshToList(drone->propeler);
        drone->position.y = start_height;

    }

    // Goal Arrow
    {
        Mesh* arrow = tema2_object3D::CreateArrow(0.45f);
        AddMeshToList(arrow);
    }

    // Minimap Initialisation
    {
        glm::ivec2 resolution = window->GetResolution();
        minimap = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    }
}

static float RandSize() {
    return 0.5 + (float)(rand()) / RAND_MAX * 0.5;
}

static int RandPos(int size) {
    return rand() % size;
}

static float RandHeight() {
    return 2.5 + (float)(rand()) / RAND_MAX * (4.0 - 2.5);
}

void Tema2::InitGoals(int nr_goals, glm::vec3 bottom_right_corner) {
    for (int i = 0; i < nr_goals; i++) {
        float height = RandHeight();

        // Choose a random spot
        int x = RandPos(area_height - 1);
        int z = RandPos(area_width - 1);
        bool rotated = (rand() % 2 == 0);

        // Check if it is free
        int nr;
        for (nr = 0; nr < 20; nr++) {
            bool ok = true;
            for (int j = x - 2; j < x + 2 && j < area_height && ok; j++) {
                if (j < 0)
                    continue;
                for (int k = z - 2; k < z + 2 && k < area_width && ok; k++) {
                    if (k < 0)
                        continue;
                    if (obstacle_map[k][j] != 0) {
                        ok = false;
                    }
                }
            }

            if (ok)
                break;

            x = (x + 7) % (int)floor(area_height);
            z = (z + 9) % (int)floor(area_width);
        }

        // Give up if you don't find any
        if (nr == 20)
            break;

        // Add it to the obstacle map
        for (int j = x - 2; j < x + 2 && j < area_height; j++) {
            if (j < 0)
                continue;
            for (int k = z - 2; k < z + 2 && k < area_width; k++) {
                if (k < 0)
                    continue;
                obstacle_map[k][j] = GOAL;
            }
        }

        // Add it to the obstacle vector
        for (int i = 0; i < 4; i++) {
            Obstacle* goal = new Obstacle(GOAL, 1, glm::vec3(x, height, z) - bottom_right_corner, rotated, i);
            obstacles.push_back(goal);
        }

        // Create goal hitbox
        glm::vec3 min = glm::vec3(x, height, z) - bottom_right_corner;
        glm::vec3 max = glm::vec3(x, height, z) - bottom_right_corner;
        if (rotated) {
            min += glm::vec3(-0.1, 0 , -1);
            max += glm::vec3(0.1, 2, 1);
        } else {
            min += glm::vec3(-1, 0, -0.1);
            max += glm::vec3(1, 2, 0.1);
        }

        goal_hitbox.push_back(std::make_pair(min, max));
    }
}

void Tema2::InitRocks(int nr_rocks, glm::vec3 bottom_right_corner) {
    for (int i = 0; i < nr_rocks; i++) {
        float size = RandSize();

        // Choose a random spot
        int x = RandPos(area_height - 1);
        int z = RandPos(area_width - 1);
        bool rotated = (rand() % 2 == 0);

        // Check if it is free
       int nr;
        for (nr = 0; nr < 20; nr++) {
            bool ok = true;
            if (rotated) {
                for (int j = x; j < x + 5 && j < area_height; j++) {
                    for (int k = z; k > z - 3 && k > 0; k--) {
                        if (obstacle_map[k][j] == ROCK) {
                            ok = false;
                        }
                    }
                }
            }
            else {
                for (int j = x; j < x + 3 && j < area_height; j++) {
                    for (int k = z; k < z + 5 && k < area_width; k++) {
                        if (obstacle_map[k][j] == ROCK) {
                            ok = false;
                        };
                    }
                }
            }

            if (ok)
                break;

            x = (x + 7) % (int)floor(area_height);
            z = (z + 9) % (int)floor(area_width);
        }

        // Give up if you don't find a free space
        if (nr == 20)
            break;

        // Add it to the obstacle map
        if (rotated) {
            for (int j = x; j < x + 5 && j < area_height; j++) {
                for (int k = z; k > z - 3 && k > 0; k--) {
                    obstacle_map[k][j] = ROCK;
                }
            }
        }
        else {
            for (int j = x; j < x + 3 && j < area_height; j++) {
                for (int k = z; k < z + 5 && k < area_width; k++) {
                    obstacle_map[k][j] = ROCK;
                }
            }
        }

        // Add it to the obstacle vector
        Obstacle* rock = new Obstacle(ROCK, size,  glm::vec3(x, 0, z) - bottom_right_corner, rotated);
        obstacles.push_back(rock);
    }
}

void Tema2::InitTrees(int nr_trees, glm::vec3 bottom_right_corner) {
    for (int i = 0; i < nr_trees; i++) {
        float size = 1;

        // Choose a random spot
        int x = RandPos(area_height - 1);
        int z = RandPos(area_width - 1);

        // Check if it is free
        int nr;
        for (nr = 0; nr < 20; nr++) {
            bool ok = true;
            for (int j = x - 1; j < x + 1 && j < area_height && ok; j++) {
                if (j < 0)
                    continue;
                for (int k = z - 1; k < z + 1 && k < area_width && ok; k++) {
                    if (k < 0)
                        continue;
                    if (obstacle_map[k][j] != 0) {
                        ok = false;
                    }
                }
            }

            if (ok)
                break;

            x = (x + 3) % (int)floor(area_height);
            z = (z + 5) % (int)floor(area_width);
        }

        // Give up if you don't find any
        if (nr == 20)
            break;

        for (int j = x - 1; j < x + 1 && j < area_height; j++) {
            if (j < 0)
                continue;
            for (int k = z - 1; k < z + 1 && k < area_width; k++) {
                if (k < 0)
                    continue;
                obstacle_map[k][j] = TREE;
            }
        }
        Obstacle* tree = new Obstacle(TREE, size, glm::vec3(x + 0.5, 0, z + 0.5) - bottom_right_corner, false, nr_layers);
        obstacles.push_back(tree);
    }
}

void Tema2::InitObstacles(int nr_goals, int nr_roks, int nr_trees) {
    glm::vec3 bottom_right_corner = glm::vec3((float)area_height / 2.0f, 0, (float)area_width / 2.0f);

    obstacle_map.resize(area_width);
    for (int i = 0; i < area_width; i++) {
        obstacle_map[i].resize(area_height, 0);
    }

    Tema2::InitGoals(nr_goals, bottom_right_corner);

    Tema2::InitRocks(nr_roks, bottom_right_corner);

    Tema2::InitTrees(nr_trees, bottom_right_corner);
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.596, 0.729, 0.776, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

static bool AABB_Sphere_collision(glm::vec3 box_min, glm::vec3 box_max, glm::vec3 center, float radius) {
    float x = max(box_min.x, min(center.x, box_max.x));
    float y = max(box_min.y, min(center.y, box_max.y));
    float z = max(box_min.z, min(center.z, box_max.z));

    float distance = sqrt(
        (x - center.x) * (x - center.x) + 
        (y - center.y) * (y - center.y) +
        (z - center.z) * (z - center.z)
    );

    return distance < radius;
}

bool Tema2::DetectCollision() {
    for (int i = 0; i < obstacles.size(); i++) {
        Obstacle* obs = obstacles[i];

        if (AABB_Sphere_collision(
            obs->collision_base_min, 
            obs->collision_base_max, 
            drone->position, 
            drone->colision_radius
        )) {
            return true;
        }

        if (obs->type == TREE && AABB_Sphere_collision(
            obs->collision_top_min,
            obs->collision_top_max,
            drone->position,
            drone->colision_radius
        )) {
            return true;
        }
    }

    if (drone->position.y < drone->colision_radius + 0.2)
        return true;

    return false;
}

bool Tema2::DetectGoalCollision() {
    if (AABB_Sphere_collision(
        goal_hitbox[current_goal].first,
        goal_hitbox[current_goal].second,
        drone->position,
        drone->colision_radius / 2
    )) {
        return true;
    }

    return false;
}

static std::pair<float, float> TransformPolar(glm::vec3 direction) {

    float theta= acos(direction.y / glm::length(direction));
    float phi = atan2(direction.x, direction.z);

    return std::make_pair(theta, phi);
}

void Tema2::RenderScene(float deltaTimeSeconds) {
    // Terrain
    {
        modelMatrix = glm::mat4(1);
        RenderSimpleMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);
    }

    // Obstacles
    {
        for (int i = 0; i < obstacles.size(); i++) {
            Obstacle* obs = obstacles[i];

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, obs->position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(obs->size));


            switch (obs->type) {
                // Rocks
            case ROCK:
                if (obs->rotated) {
                    modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
                }
                RenderSimpleMesh(meshes["rock"], shaders["ColorShader"], modelMatrix);
                break;
                // Trees
            case TREE:
                RenderSimpleMesh(meshes["tree"], shaders["ColorShader"], modelMatrix);
                break;
                // Goals
            case GOAL:
                if (obs->rotated) {
                    modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
                }
                int goal_type;
                if (current_goal * 4 == i) {
                    goal_type = NEXT;
                }
                else if (current_goal * 4 < i) {
                    goal_type = LOCKED;
                }
                else {
                    goal_type = DONE;
                }
                RenderSimpleMesh(meshes["goal"], shaders["GoalShader"], modelMatrix, goal_type);
                i += 3;
                break;
            default:
                printf("Render_Scene - Something went wrong\n");
                break;
            }
        }
    }


    // Drone
    {
        //drone->thrust -= gravity * deltaTimeSeconds;
        glm::vec3 old_position = drone->position;
        drone->Move(deltaTimeSeconds);
        if (DetectCollision()) {
            drone->position = old_position;
            drone->thrust = -drone->thrust / 2;
            drone->forward = -drone->forward / 2;
            drone->lateral = -drone->lateral / 2;
        }

        if (current_goal < goal_hitbox.size() && DetectGoalCollision()) {
            current_goal++;
        }
        // Drone Body
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, drone->position);
            modelMatrix = glm::rotate(modelMatrix, RADIANS(drone->rotation), glm::vec3(0, 1, 0));
            if (camera_type != FIRST_PERSON)
                RenderSimpleMesh(meshes["drone_body"], shaders["ColorShader"], modelMatrix);
        }

        // Drone Propelers
        {
            for (int i = 0; i < drone->propelers_pos.size(); i++) {
                // Body positions
                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, drone->position);
                modelMatrix = glm::rotate(modelMatrix, RADIANS(drone->rotation), glm::vec3(0, 1, 0));

                // Propeler position
                modelMatrix = glm::translate(modelMatrix, drone->propelers_pos[i]);
                modelMatrix = glm::rotate(modelMatrix, RADIANS(drone->propeler_rotation), glm::vec3(0, 1, 0));
                RenderSimpleMesh(meshes["drone_propeler"], shaders["ColorShader"], modelMatrix);
            }

            drone->RotatePropeler(deltaTimeSeconds);
        }

        // Goal Arrow
        if (show_direction)
        {
            glm::vec3 direction = obstacles[current_goal * 4]->position - drone->position;

            std::pair<float, float> rotation = TransformPolar(direction);
            glm::vec3 arrow_pos = drone->position + glm::vec3(0, 1, 0);

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, arrow_pos);

            modelMatrix = glm::rotate(modelMatrix, rotation.second - RADIANS(90), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, -rotation.first, glm::vec3(0, 0, 1));
            RenderSimpleMesh(meshes["goal_arrow"], shaders["ColorShader"], modelMatrix);
        }
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    

    // Camera
    {
        if (camera_type == THIRD_PERSON) {
            camera->Set(
                drone->position + glm::vec3(-1.5f * cos(-RADIANS(drone->rotation - 45)), 0.6f, -1.5f * sin(-RADIANS(drone->rotation - 45))),
                drone->position, 
                glm::vec3(0, 1, 0)
            );
        }
        else if (camera_type == FIRST_PERSON) {
            camera->Set(
                drone->position + glm::vec3(0.15f * cos(-RADIANS(drone->rotation - 45)), 0, 0.15f * sin(-RADIANS(drone->rotation - 45))),
                drone->position + glm::vec3(0.50f * cos(-RADIANS(drone->rotation - 45)), 0, 0.50f * sin(-RADIANS(drone->rotation - 45))),
                glm::vec3(0, 1, 0)
            );
        }
    }

    RenderScene(deltaTimeSeconds);

    // Collisions
    if (show_collision_boxes) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Drone collision sphere
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, drone->position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(2 * drone->colision_radius));
            RenderSimpleMesh(meshes["sphere"], shaders["ColorShader"], modelMatrix);
        }

        // Obstacles collision extremes
        {
            for (int i = 0; i < obstacles.size(); i++) {
                Obstacle* obs = obstacles[i];
                // Rocks and Goals
                if (obs->type == ROCK) {
                    glm::vec3 pos = (obs->collision_base_max + obs->collision_base_min);
                    pos = glm::vec3(pos.x / 2, pos.y / 2, pos.z / 2);

                    glm::vec3 size = obs->collision_base_max - obs->collision_base_min;
                    size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));
                        
                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, pos);
                    modelMatrix = glm::scale(modelMatrix, size);
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix);
                }
                // Trees
                else if (obs->type == TREE) {
                    // Trunk
                    glm::vec3 pos = (obs->collision_base_max + obs->collision_base_min);
                    pos = glm::vec3(pos.x / 2, pos.y / 2, pos.z / 2);

                    glm::vec3 size = obs->collision_base_max - obs->collision_base_min;
                    size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));

                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, pos);
                    modelMatrix = glm::scale(modelMatrix, size);
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix);          
                        
                    // Leaves
                    pos = (obs->collision_top_max + obs->collision_top_min);
                    pos = glm::vec3(pos.x / 2, pos.y / 2, pos.z / 2);

                    size = obs->collision_top_max - obs->collision_top_min;
                    size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));

                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, pos);
                    modelMatrix = glm::scale(modelMatrix, size);
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix);
                }
                else {
                    glm::vec3 pos = (obs->collision_base_max + obs->collision_base_min);
                    pos = glm::vec3(pos.x / 2, pos.y / 2, pos.z / 2);

                    glm::vec3 size = obs->collision_base_max - obs->collision_base_min;
                    size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));

                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, pos);
                    modelMatrix = glm::scale(modelMatrix, size);
                    RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix);
                }
            }
        }

        // Draw Goal Collision Boxes
        {
            for (int i = 0; i < goal_hitbox.size(); i++) {
                glm::vec3 pos = (goal_hitbox[i].second + goal_hitbox[i].first);
                pos = glm::vec3(pos.x / 2, pos.y / 2, pos.z / 2);

                glm::vec3 size = goal_hitbox[i].second - goal_hitbox[i].first;
                size = glm::vec3(abs(size.x), abs(size.y), abs(size.z));

                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, pos);
                modelMatrix = glm::scale(modelMatrix, size);
                RenderSimpleMesh(meshes["box"], shaders["ColorShader"], modelMatrix);
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    }

    // Minimap
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(minimap.x, minimap.y, minimap.width, minimap.height);
        
        int tmp = camera_type;
        camera_type = THIRD_PERSON;

        glm::vec3 pos = drone->position;
        pos.y += 7;
        camera->Set(
            drone->position + glm::vec3(-0.1f, 7, 0),
            drone->position,
            glm::vec3(0, 1, 0)
        );

        RenderScene(deltaTimeSeconds);
        
        camera_type = tmp;
    }
}


void Tema2::FrameEnd()
{
    //DrawCoordinateSystem();
}


void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, int goal_type)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    float value = sin(Engine::GetElapsedTime());

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    int ModelLocation = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(ModelLocation, 1, false, glm::value_ptr(modelMatrix));

    int ViewLocation = glGetUniformLocation(shader->GetProgramID(), "View");
    glm::mat4 viewMatrix = (camera_type == FREE_CAM) ? GetSceneCamera()->GetViewMatrix() : camera->GetViewMatrix();
    glUniformMatrix4fv(ViewLocation, 1, false, glm::value_ptr(viewMatrix));

    int ProjectionLocation = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    glUniformMatrix4fv(ProjectionLocation, 1, false, glm::value_ptr(projectionMatrix));

    if (goal_type != NONE) {
        int TypeLocation = glGetUniformLocation(shader->GetProgramID(), "type");
        glUniform1i(TypeLocation, goal_type);
    }

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{

    // Thrust
    if (window->KeyHold(GLFW_KEY_W) && !(camera_type == FREE_CAM && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))) {
        drone->Thrust(deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_S) && !(camera_type == FREE_CAM && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))) {
        drone->Thrust(-deltaTime);
    }

    // Forward
    if (window->KeyHold(GLFW_KEY_I)) {
        drone->Forward(deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_K)) {
        drone->Forward(-deltaTime);
    }

    // Rotate
    if (window->KeyHold(GLFW_KEY_D) && !(camera_type == FREE_CAM && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))) {
        drone->Rotate(deltaTime); 
    }
    if (window->KeyHold(GLFW_KEY_A) && !(camera_type == FREE_CAM && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))) {
        drone->Rotate(-deltaTime); 
    }

    // Lateral
    if (window->KeyHold(GLFW_KEY_J)) {
        drone->Lateral(deltaTime);
    }
    if (window->KeyHold(GLFW_KEY_L)) {
        drone->Lateral(-deltaTime);
    }

    // Brake
    if (window->KeyHold(GLFW_KEY_SPACE)) {
        drone->thrust /= (1 + deltaTime);
        drone->forward /= (1 + deltaTime);
        drone->lateral /= (1 + deltaTime);
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F3) {
        show_collision_boxes = !show_collision_boxes;
    }

    if (key == GLFW_KEY_P) {
        camera_type = (camera_type + 1) % 3;
    }
    
    if (key == GLFW_KEY_O) {
        show_direction = !show_direction;
    }

    if (key == GLFW_KEY_RIGHT_SHIFT) {
        drone->rotation = 45;
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
