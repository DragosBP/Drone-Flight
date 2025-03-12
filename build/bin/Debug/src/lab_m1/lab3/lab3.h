#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Lab3 : public gfxc::SimpleScene
    {
     public:
        Lab3();
        ~Lab3();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void GenerateTerrain();
        void InitHeightMap();
        glm::vec3 CalculateTankPosition(glm::vec3 current);

     protected:
        glm::mat3 modelMatrix;
        float square_terrain_length;
        float terrain_density;
        float terrain_start;
        int screen_width;
        std::vector<float> height_map;
        float tank_speed;
        int tank_size;
        glm::vec3 tank_pos;
        glm::vec3 tank_weapon_pos;
        float tank_weapon_rotation;
    };
}   // namespace m1
