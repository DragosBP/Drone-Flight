#include "teme/tema1/tank.h"
#include "teme/tema1/tema1_object2D.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace teme;

Tank::Tank(int tank_id, glm::vec3 tank_start, float tank_speed, float tank_power, float bullet_time, string name, float color_dif)
{
    this->isAlive = true;

    this->name = name;
    this->tank_id = tank_id;

    this->tank_pos = tank_start;
    this->tank_speed = tank_speed;
    this->health = 10;

    this->weapon_rotation = 0;
    this->power = tank_power;
    this->bullet_time = bullet_time;

    this->body = tema1_object2D::CreateTank(
        name + "_body",
        tank_start,
        1,
        color_dif
    );

    this->weapon = tema1_object2D::CreateGun(
        name + "_weapon",
        tank_start,
        1,
        glm::vec3(0.017f, 0.017f, 0.016f)
    );

    this->trajectory = tema1_object2D::CreateTrajectory(
        name + "_trajectory",
        1
    );

    this->bullet = tema1_object2D::CreateCircle(
        name + "_bullet",
        tank_start,
        4,
        glm::vec3(0.0f, 0.0f, 0.0f)
    );

    this->health_bar = tema1_object2D::CreateHealthBar(
        name + "_health_bar",
        1,
        glm::vec3(0.588f, 0.8f, 0.905f)
    );

    this->health_bar_container = tema1_object2D::CreateHealthBar(
        name + "_health_bar_container",
        1,
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
}

Tank::~Tank()
{

}

void Tank::CalculateTankPosition(vector<float> height_map, float square_terrain_length) {
    glm::vec3 A = glm::vec3
    (
        tank_pos.x,
        height_map[floor(tank_pos.x / square_terrain_length)],
        0
    );

    glm::vec3 B = glm::vec3
    (
        tank_pos.x + square_terrain_length,
        height_map[floor((tank_pos.x + square_terrain_length) / square_terrain_length)],
        0
    );

    float t = (tank_pos.x - A.x) / (B.x - A.x);
    float tankY = A.y + t * (B.y - A.y);

    glm::vec3 diff = B - A;
    float rotation = atan2(diff.y, diff.x);

    tank_pos = glm::vec3(tank_pos.x, tankY, rotation);
}

void Tank::AddNewBullet() {
    bullet_struct *new_bullet = (bullet_struct*)malloc(sizeof(bullet_struct));
    new_bullet->bullet_pos = glm::vec3(
        this->tank_pos.x - 28 * sin(this->tank_pos.z),
        this->tank_pos.y + 28 * cos(this->tank_pos.z),
        0
    );
    new_bullet->bullet_speed = glm::vec3(
        cos(RADIANS(this->weapon_rotation + 90)) * this->power,
        sin(RADIANS(this->weapon_rotation + 90)) * this->power,
        0
    );
    new_bullet->bullet_id = this->tank_id;
    this->bullets.push_back(new_bullet);
}

void Tank::FreeBullet(int id) {
    free(this->bullets[id]);
    this->bullets.erase(this->bullets.begin() + id);
}