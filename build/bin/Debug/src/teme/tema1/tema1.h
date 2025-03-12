#pragma once

#include "components/simple_scene.h"
#include "teme/tema1/tank.h"


namespace teme
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

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
        void InitHeightMap(int screen_width);
        bool DetectColision(glm::vec3 current);
        int DetectTankCollision(glm::vec3 current, int bullet_id);
        void SimulateExplosion(glm::vec3 center);
        void SimulateLandslide();

    protected:
        // Operations
        glm::ivec2 resolution;
        glm::mat3 modelMatrix;

        // Terrain
        float square_terrain_length;
        float terrain_density;
        float terrain_start;
        std::vector<float> height_map;

        // Landslide
        float terrain_diff;
        float terrain_error;
        bool landslide;

        // Physics
        float g;
        float power;

        // Explosion
        float explosion_radius;
        float tank_radius;

        // Players
        std::vector<Tank*> players;
    };
}   // namespace teme
