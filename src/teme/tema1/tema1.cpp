#include "teme/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "teme/tema1/tema1_transform2D.h"
#include "teme/tema1/tema1_object2D.h"

using namespace std;
using namespace teme;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{

}


Tema1::~Tema1()
{
}

void Tema1::Init()
{
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    int screen_width = resolution.x;

    // Init terrain
    g = -0.6f;
    terrain_density = 0.01f;
    terrain_start = 0;
    square_terrain_length = screen_width / 2000.0f;

    // Init landslide
    terrain_diff = 1;
    terrain_error = 0.5;

    glm::vec3 corner = glm::vec3(0, 0, 0);
    Mesh* square1 = tema1_object2D::CreateSquare(
        "square_terrain",
        corner,
        1.0f,
        glm::vec3(0.843f, 0.874f, 0.129f),
        true
    );
    AddMeshToList(square1);
    InitHeightMap(screen_width);


    // Variables of the tanks
    explosion_radius = 40;
    tank_radius = 40;

    float tank_speed = 130.0f;
    float tank_power = 20.0f;
    float bullet_time = 30.0f;

    // Init the first tank
    Tank* player1 = new Tank(
        0,
        corner, 
        tank_speed,
        tank_power,
        bullet_time,
        "player1",
        0
    );
    AddMeshToList(player1->body);
    AddMeshToList(player1->weapon);
    AddMeshToList(player1->trajectory);
    AddMeshToList(player1->bullet);
    AddMeshToList(player1->health_bar);
    AddMeshToList(player1->health_bar_container);
    player1->tank_pos.x = 130;
    player1->CalculateTankPosition(height_map, square_terrain_length);
    players.push_back(player1);

    Tank* player2 = new Tank(
        1,
        corner,
        tank_speed,
        tank_power,
        bullet_time,
        "player2",
        0.9
    );
    AddMeshToList(player2->body);
    AddMeshToList(player2->weapon);
    AddMeshToList(player2->trajectory);
    AddMeshToList(player2->bullet);
    AddMeshToList(player2->health_bar);
    AddMeshToList(player2->health_bar_container);

    player2->tank_pos.x = 730;
    player2->CalculateTankPosition(height_map, square_terrain_length);
    players.push_back(player2);
}

void Tema1::FrameStart()
{
    glClearColor(0.588f, 0.8f, 0.905f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

float TerrainFunction(float x, float t) {
    // Valori t: (rand(0...1000) * 2 + 2000) / 10000
    // Terenul ridicat cu un 6 si inmultit cu 20
    // capat stanga: st = rand(0...150), capat dreapta: dr = st + 30

    float y =
        sin(t * x) +
        0.6f * sin(t * 2.0f * x) +
        0.9f * sin(t * 1.4f * x) +
        1.5f * sin(t * 0.3f * x) +
        1.2f * sin(t * 0.8f * x) +
        2.0f * sin(t * 0.2f * x) +
        1.7f * sin(t * 1.2f * x);

    return (y + 6.0f) * 20;
}

void Tema1::InitHeightMap(int screen_width) {
    float height;
    float i;

    for (i = 0; i < screen_width; i += square_terrain_length) {
        height = TerrainFunction(i + terrain_start, terrain_density);
        height_map.push_back(height);
    }

    height = TerrainFunction(i + terrain_start, terrain_density);
    height_map.push_back(height);
}

void Tema1::GenerateTerrain() {
    float height_st, height_dr;
    float i;
    height_st = height_map[0];

    for (i = 0; i < height_map.size() - 1; i++) {
        height_dr = height_map[i + 1];

        modelMatrix = glm::mat3(1);
        modelMatrix *= tema1_transform2D::Translate(i * square_terrain_length, height_st);
        modelMatrix *= tema1_transform2D::Cut((height_dr - height_st) / square_terrain_length);
        modelMatrix *= tema1_transform2D::Translate(0, -max(height_dr, height_st));
        modelMatrix *= tema1_transform2D::Scale(square_terrain_length, max(height_dr, height_st));
        RenderMesh2D(meshes["square_terrain"], shaders["VertexColor"], modelMatrix);

        height_st = height_dr;
    }

}

int Tema1::DetectTankCollision(glm::vec3 current, int bullet_id) {
    for (int i = 0; i < players.size(); i++) {
        Tank* player = players[i];

        if (player->tank_id == bullet_id || !player->isAlive)
            continue;

        glm::vec3 center = glm::vec3(
            player->tank_pos.x + 14 * sin(player->tank_pos.z),
            player->tank_pos.y + 14 * cos(player->tank_pos.z),
            0
        );

        float delta_X2 = (center.x - current.x) * (center.x - current.x);
        float delta_Y2 = (center.y - current.y) * (center.y - current.y);


        if (delta_X2 + delta_Y2 < tank_radius * tank_radius) {
            
            return i;
        }
    }
    return -1;
}

bool Tema1::DetectColision(glm::vec3 current) {
    int i = floor(current.x / square_terrain_length);
    glm::vec3 A = glm::vec3(i * square_terrain_length, height_map[i], 0);
    glm::vec3 B = glm::vec3((i + 1) * square_terrain_length, height_map[i + 1], 0);

    float t = (current.x - A.x) / (B.x - A.x);
    glm::vec3 I = A * (1 - t) + B * t;

    return I.y - current.y > 0;
}

void Tema1::SimulateExplosion(glm::vec3 center) {
    int start = floor(center.x / square_terrain_length);
    int left;
    int right;

    if (center.x - explosion_radius > 0) {
        left = floor((center.x - explosion_radius) / square_terrain_length);
    }
    else {
        left = 0;
    }

    if (center.x + explosion_radius < resolution.x) {
        right = ceil((center.x + explosion_radius) / square_terrain_length);
    }
    else {
        right = resolution.x / square_terrain_length;
    }

    for (int i = left; i < right; i++) {
        float x = abs(center.x - i * square_terrain_length);
        if (x > explosion_radius)
            continue;
        
        float val = sqrt(explosion_radius * explosion_radius - x * x);

        if (height_map[i] > center.y - val)
            height_map[i] = center.y - val;
        
        if (height_map[i] < 0)
            height_map[i] = 0;
    }

    landslide = true;
}

void Tema1::SimulateLandslide() {
    landslide = false;
    for (int i = 0; i < height_map.size() - 1; i++) {
        if (height_map[i] - height_map[i + 1] > terrain_diff) {
            height_map[i] -= terrain_error;
            height_map[i + 1] += terrain_error;
            landslide = true;
        }
        else if (height_map[i + 1] - height_map[i] > terrain_diff) {
            height_map[i + 1] -= terrain_error;
            height_map[i] += terrain_error;
            landslide = true;
        }
    }

    for (int i = 0; i < players.size(); i++) {
        players[i]->CalculateTankPosition(height_map, square_terrain_length);
    }
}

void Tema1::Update(float deltaTimeSeconds)
{
    // Terrain
    GenerateTerrain();

    for (int i = 0; i < players.size(); i++) {
        Tank* player = players[i];

        if (player->isAlive) {
            // Tank 
            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1_transform2D::Translate(player->tank_pos.x, player->tank_pos.y);
            modelMatrix *= tema1_transform2D::Rotate(player->tank_pos.z);
            RenderMesh2D(meshes[player->name + "_body"], shaders["VertexColor"], modelMatrix);

            // Weapon
            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1_transform2D::Translate(player->tank_pos.x - 28 * sin(player->tank_pos.z), player->tank_pos.y + 28 * cos(player->tank_pos.z));

            modelMatrix *= tema1_transform2D::Rotate(RADIANS(player->weapon_rotation));
            RenderMesh2D(meshes[player->name + "_weapon"], shaders["VertexColor"], modelMatrix);

            // Health_Bar
            modelMatrix = glm::mat3(1);
            int val = 60 - 6 * player->health;
            modelMatrix *= tema1_transform2D::Translate((60 - val) / 2, 0);
            modelMatrix *= tema1_transform2D::Translate(player->tank_pos.x, player->tank_pos.y + 72);
            modelMatrix *= tema1_transform2D::Scale(val, 12);
            RenderMesh2D(meshes[player->name + "_health_bar"], shaders["VertexColor"], modelMatrix);

            // Health_Bar_Container
            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1_transform2D::Translate(player->tank_pos.x, player->tank_pos.y + 70);

            modelMatrix *= tema1_transform2D::Scale(64, 16);
            RenderMesh2D(meshes[player->name + "_health_bar_container"], shaders["VertexColor"], modelMatrix);

        }

        // Bullets
        for (int j = 0; j < player->bullets.size(); j++) {
            bullet_struct* bullet = player->bullets[j];

            float addToX = player->bullet_time * bullet->bullet_speed.x * deltaTimeSeconds;
            float addToY = player->bullet_time * bullet->bullet_speed.y * deltaTimeSeconds + (g * deltaTimeSeconds * deltaTimeSeconds / 2);

            bullet->bullet_speed.y += player->bullet_time * g * deltaTimeSeconds;

            bullet->bullet_pos += glm::vec3(addToX, addToY, 0);

            if (bullet->bullet_pos.x < 0 || bullet->bullet_pos.x > resolution.x) {
                free(bullet);
                player->bullets.erase(player->bullets.begin() + j);
                j--;
                continue;
            }

            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1_transform2D::Translate(bullet->bullet_pos.x, bullet->bullet_pos.y);

            RenderMesh2D(meshes[player->name + "_bullet"], shaders["VertexColor"], modelMatrix);

            // With terrain
            if (DetectColision(bullet->bullet_pos)) {
                SimulateExplosion(bullet->bullet_pos);

                player->FreeBullet(j);
                j--;
            }

            // With another tank
            int rez = DetectTankCollision(bullet->bullet_pos, bullet->bullet_id);
            if (rez != -1) {
                player->FreeBullet(j);
                j--;

                players[rez]->health--;
                if (players[rez]->health == 0) {
                    players[rez]->isAlive = false;
                }

            }
        }
    }

    // So the trajectory is behind the tanks
    for (int i = 0; i < players.size(); i++) {
        Tank* player = players[i];

        if (!player->isAlive)
            continue;

        // Trajectory
        float translateX = player->tank_pos.x - 28 * sin(player->tank_pos.z);
        float translateY = player->tank_pos.y + 28 * cos(player->tank_pos.z);

        float vX = cos(RADIANS(player->weapon_rotation + 90)) * player->power;
        float vY = sin(RADIANS(player->weapon_rotation + 90)) * player->power;

        float deltaT = 1;

        while (true) {
            if (translateX < 0 || translateX > resolution.x || translateY <= 0
                || DetectColision(glm::vec3(translateX, translateY, 0)) || DetectTankCollision(glm::vec3(translateX, translateY, 0), player->tank_id) != -1)
                break;
            float addToX = vX * deltaT;
            float addToY = vY * deltaT + (g * deltaT * deltaT / 2);

            translateX += addToX;
            translateY += addToY;

            vY += g * deltaT;

            float rotate = atan2(addToY, addToX);
            float distance = sqrt((addToX * addToX) + (addToY * addToY));
            glm::vec3 center = glm::vec3(translateX + addToX / 2, translateY + addToY / 2, 0);

            modelMatrix = glm::mat3(1);
            modelMatrix *= tema1_transform2D::Translate(translateX, translateY);
            modelMatrix *= tema1_transform2D::Rotate(rotate - RADIANS(90));
            modelMatrix *= tema1_transform2D::Scale(4, distance);

            RenderMesh2D(meshes[player->name + "_trajectory"], shaders["VertexColor"], modelMatrix);
        }
    }

    // Landslide
    if (landslide) {
        SimulateLandslide();
    }
}


void Tema1::FrameEnd()
{
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    int buffer = 5;

    // Player 1 input logic
    if (players[0]->isAlive) {
        if (window->KeyHold(GLFW_KEY_A) && players[0]->tank_pos.x > buffer) {
            players[0]->tank_pos.x -= deltaTime * players[0]->tank_speed;
            players[0]->CalculateTankPosition(height_map, square_terrain_length);
        }
        if (window->KeyHold(GLFW_KEY_D) && players[0]->tank_pos.x < resolution.x - buffer) {
            players[0]->tank_pos.x += deltaTime * players[0]->tank_speed;
            players[0]->CalculateTankPosition(height_map, square_terrain_length);
        }
        if (window->KeyHold(GLFW_KEY_W) && players[0]->weapon_rotation > -90) {
            players[0]->weapon_rotation -= deltaTime * players[0]->tank_speed / 2;
        }
        if (window->KeyHold(GLFW_KEY_S) && players[0]->weapon_rotation < 90) {
            players[0]->weapon_rotation += deltaTime * players[0]->tank_speed / 2;
        }
    }

    // Player 2 input logic
    if (players[1]->isAlive) {
        if (window->KeyHold(GLFW_KEY_J) && players[1]->tank_pos.x > buffer) {
            players[1]->tank_pos.x -= deltaTime * players[1]->tank_speed;
            players[1]->CalculateTankPosition(height_map, square_terrain_length);
        }
        if (window->KeyHold(GLFW_KEY_L) && players[1]->tank_pos.x < resolution.x - buffer) {
            players[1]->tank_pos.x += deltaTime * players[1]->tank_speed;
            players[1]->CalculateTankPosition(height_map, square_terrain_length);
        }
        if (window->KeyHold(GLFW_KEY_I) && players[1]->weapon_rotation > -90) {
            players[1]->weapon_rotation -= deltaTime * players[1]->tank_speed / 2;
        }
        if (window->KeyHold(GLFW_KEY_K) && players[1]->weapon_rotation < 90) {
            players[1]->weapon_rotation += deltaTime * players[1]->tank_speed / 2;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (players[0]->isAlive && key == GLFW_KEY_SPACE) {
        players[0]->AddNewBullet();
    }

    // Player 2 shooting
    if (players[1]->isAlive && key == GLFW_KEY_ENTER) {
        players[1]->AddNewBullet();
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    printf("%d %d\n", mouseX, window->GetResolution().y - mouseY);
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
