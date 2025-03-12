#pragma once

#include "components/simple_scene.h"

struct bullet_struct {
	glm::vec3 bullet_speed;
	glm::vec3 bullet_pos;
	int bullet_id;
};

namespace teme
{
	class Tank
	{
	public:
		Tank(int tank_id, glm::vec3 tank_start, float tank_speed, float tank_power, float bullet_time, std::string name, float color_dif);
		~Tank();
	
		void CalculateTankPosition(std::vector<float> height_map, float square_terrain_length);
		void AddNewBullet();
		void FreeBullet(int id);

		bool isAlive;

		std::string name;
		int tank_id;
		
		glm::vec3 tank_pos;
		float tank_speed;
		int health;
		
		float weapon_rotation;
		float power;
		
		std::vector<bullet_struct*> bullets;
		float bullet_time;

		Mesh* body;
		Mesh* weapon;
		Mesh* trajectory;
		Mesh* bullet;
		Mesh* health_bar;
		Mesh* health_bar_container;
	};

}