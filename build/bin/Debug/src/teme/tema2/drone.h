#pragma once

#include "components/simple_scene.h"


namespace teme
{
	class Drone
	{
	public:
		Drone::Drone(std::string name, float size, glm::vec3 initial_pos, float acceleration);
		~Drone();

		void Rotate(float delta);
		void Thrust(float delta);
		void Forward(float delta);
		void Lateral(float delta);
		void Move(float delta);

		void RotatePropeler(float delta);


		std::string name;
		float size;

		Mesh* body;
		glm::vec3 position;
		

		float rotation_acceleration;
		float rotation;

		float thrust;
		float forward;
		float lateral;
		float max_speed;
		float acceleration;

		Mesh* propeler;
		std::vector<glm::vec3> propelers_pos;
		float propeler_rotation;

		float colision_radius;

		const float EPSILON = 1e-6f;
	};
}
