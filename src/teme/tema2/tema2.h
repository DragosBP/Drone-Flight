#pragma once

#include <vector>

#include "components/simple_scene.h"
#include "teme/tema2/drone.h"
#include "teme/tema2/obstacle.h"
#include "teme/tema2/camera.h"


#define THIRD_PERSON 0
#define FIRST_PERSON 1
#define FREE_CAM 2

#define NONE -1
#define LOCKED 0
#define NEXT 1
#define DONE 2

namespace teme
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
         struct ViewportArea
         {
             ViewportArea() : x(0), y(0), width(1), height(1) {}
             ViewportArea(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {}
             int x;
             int y;
             int width;
             int height;
         };

        Tema2();
        ~Tema2();

        void Init() override;


     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, int goal_type = -1);
        void RenderScene(float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void InitObstacles(int nr_goals, int nr_roks, int nr_trees);
        void InitGoals(int nr_goals, glm::vec3 bottom_right_corner);
        void InitRocks(int nr_rocks, glm::vec3 bottom_right_corner);
        void InitTrees(int nr_trees, glm::vec3 bottom_right_corner);

        bool DetectCollision();
        bool DetectGoalCollision();
    
    protected:
        glm::mat4 modelMatrix;
        
        implemented::Tema_Camera *camera;
        int camera_type;
        bool show_collision_boxes;
        
        Drone* drone;

        Mesh* terrain;
        float area_width;
        float area_height;
        ViewportArea minimap;
        
        std::vector<std::vector<unsigned int>> obstacle_map;
        std::vector<Obstacle *> obstacles;
        int nr_layers;
        
        std::vector<std::pair<glm::vec3, glm::vec3>> goal_hitbox;
        int current_goal;

        bool show_direction;

        float gravity;
    };
}