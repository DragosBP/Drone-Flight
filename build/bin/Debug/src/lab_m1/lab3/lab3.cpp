#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}

void Lab3::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    screen_width = resolution.x;

    terrain_density = 0.01f;
    terrain_start = 0;
    square_terrain_length = screen_width / 2000.0f;

    glm::vec3 corner = glm::vec3(0, 0, 0);
    Mesh* square1 = object2D::CreateSquare(
        "square_terrain", 
        corner, 
        1.0f, 
        glm::vec3(0.843f, 0.874f, 0.129f), 
        true
    );
    AddMeshToList(square1);

    tank_size = 1;
    tank_speed = 100.0f;
    glm::vec3 tank_start = glm::vec3(0, 0, 0);
    Mesh* tank1 = object2D::CreateTank(
        "tank_player1",
        tank_start,
        tank_size,
        0
    );
    AddMeshToList(tank1);
    
    Mesh* tank1_weapon = object2D::CreateGun(
        "tank_weapon_player1",
        tank_start,
        tank_size
    );
    AddMeshToList(tank1_weapon);
    InitHeightMap();

    tank_pos = CalculateTankPosition(glm::vec3(130, 0, 0));
    tank_weapon_rotation = 0.0f;
}

float static TerrainFunction(float x, float t) {
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

void Lab3::InitHeightMap() {
    float height;
    float i;
    
    for (i = 0; i < screen_width; i += square_terrain_length) {
        height = TerrainFunction(i + terrain_start, terrain_density);
        height_map.push_back(height);
    }

    height = TerrainFunction(i + terrain_start, terrain_density);
    height_map.push_back(height);
}

void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.588f, 0.8f, 0.905f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Lab3::GenerateTerrain() {
    float height_st, height_dr;
    float i;
    height_st = height_map[0];

    for (i = 0; i < height_map.size() - 1; i++) {
        height_dr = height_map[i + 1];

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(i * square_terrain_length, height_st);
        modelMatrix *= transform2D::Cut((height_dr - height_st) / square_terrain_length);
        modelMatrix *= transform2D::Translate(0, -max(height_dr, height_st));
        modelMatrix *= transform2D::Scale(square_terrain_length, max(height_dr, height_st));
        RenderMesh2D(meshes["square_terrain"], shaders["VertexColor"], modelMatrix);

        height_st = height_dr;
    }

}

glm::vec3 Lab3::CalculateTankPosition(glm::vec3 current) {
    glm::vec3 A = glm::vec3
    (
        current.x,
        height_map[floor(current.x / square_terrain_length)],
        0
    );

    glm::vec3 B = glm::vec3
    (
        current.x + square_terrain_length ,
        height_map[floor((current.x + square_terrain_length) / square_terrain_length)],
        0
    );

    float t = (current.x - A.x) / (B.x - A.x);
    float tankY = A.y + t * (B.y - A.y);

    glm::vec3 diff= B - A;
    float rotation = atan2(diff.y, diff.x);

    tank_weapon_pos = glm::vec3(current.x, tankY, rotation);

    return glm::vec3(current.x, tankY, rotation);
}


void Lab3::Update(float deltaTimeSeconds)
{
    // Terrain
    GenerateTerrain();

    // Tank 
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tank_pos.x, tank_pos.y);
    modelMatrix *= transform2D::Rotate(tank_pos.z);
    RenderMesh2D(meshes["tank_player1"], shaders["VertexColor"], modelMatrix);

    //Weapon
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tank_weapon_pos.x, tank_weapon_pos.y);
    modelMatrix *= transform2D::Rotate(tank_weapon_pos.z);

    modelMatrix *= transform2D::Translate(0, 28 * tank_size);
    modelMatrix *= transform2D::Rotate(RADIANS(tank_weapon_rotation));
    modelMatrix *= transform2D::Translate(0, -28 * tank_size);
    RenderMesh2D(meshes["tank_weapon_player1"], shaders["VertexColor"], modelMatrix);
}


void Lab3::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_A)) {
        tank_pos.x -= deltaTime * tank_speed;
        tank_pos = CalculateTankPosition(tank_pos);
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        tank_pos.x += deltaTime * tank_speed;
        tank_pos = CalculateTankPosition(tank_pos);
    }
    if (window->KeyHold(GLFW_KEY_W) && tank_weapon_rotation > -90) {
        tank_weapon_rotation -= deltaTime * tank_speed;
    }
    if (window->KeyHold(GLFW_KEY_S) && tank_weapon_rotation < 90) {
        tank_weapon_rotation += deltaTime * tank_speed;
    }
}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    printf("%d %d\n", mouseX, window->GetResolution().y - mouseY);
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab3::OnWindowResize(int width, int height)
{
}
