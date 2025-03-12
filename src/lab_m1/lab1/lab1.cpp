#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
    :speed(0),
    mesh_type(BOX),
    color_red(0),
    pos_box(glm::vec3(0.0f, 0.5f, 0.0f)),
    jumping(false),
    falling(false),
    max_jump_height(1.5f),
    current_jump_height(0.0f),
    jump_speed(3)
{
    // TODO(student): Never forget to initialize class variables
}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("bunny");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.

}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(color_red, 0, 0, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    switch (mesh_type)
    {
    case BOX:
        RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));
        break;
    case TEAPOT:
        RenderMesh(meshes["teapot"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));
        break;
    case SPHERE:
        RenderMesh(meshes["sphere"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));
        break;
    default:
        RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));
        break;
    }

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(pos_box.x, pos_box.y, pos_box.z));

    RenderMesh(meshes["bunny"], glm::vec3(2, 0.5f, 3), glm::vec3(0.04f));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.

}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.

    if (mods & GLFW_MOD_SHIFT) {
        speed = 10;
    }
    else {
        speed = 3;
    }

    if (window->MouseHold(GLFW_MOUSE_BUTTON_2)) {
        return;
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        pos_box.x += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        pos_box.x -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        pos_box.z += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        pos_box.z -= speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        pos_box.y += speed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        pos_box.y -= speed * deltaTime;
    }

    if (jumping) {
        if (current_jump_height >= max_jump_height) {
            falling = true;
            jumping = false;
        }
        float delta_y = jump_speed * deltaTime;
        pos_box.y += delta_y;
        current_jump_height += delta_y;
    }

    if (falling) {
        if (current_jump_height <= 0) {
            falling = false;
        }
        float delta_y = jump_speed * deltaTime;
        pos_box.y -= delta_y;
        current_jump_height -= delta_y;
    }
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.
        switch (color_red) {
        case 0:
            color_red = 1;
            break;
        case 1:
            color_red = 0;
            break;
        }

    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.
    if (key == GLFW_KEY_G) {
        switch (mesh_type)
        {
        case BOX:
            mesh_type = TEAPOT;
            break;
        case TEAPOT:
            mesh_type = SPHERE;
            break;
        case SPHERE:
            mesh_type = BOX;
            break;
        default:
            mesh_type = BOX;
            break;
        }
    }
    if (!jumping && !falling && key == GLFW_KEY_SPACE)
        jumping = true;
}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
