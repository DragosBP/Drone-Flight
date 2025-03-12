#pragma once

#include "components/simple_scene.h"

#define TREE 1
#define ROCK 2
#define GOAL 3

namespace teme
{
	class Obstacle
	{
	public:
		Obstacle::Obstacle(unsigned int type, float size, glm::vec3 position, bool rotated, int nr_layers = 0);
		~Obstacle();

		unsigned int type;
		float size;
		glm::vec3 position;
		bool rotated;

		glm::vec3 collision_base_min;
		glm::vec3 collision_base_max;
		glm::vec3 collision_top_min;
		glm::vec3 collision_top_max;
	};
}
