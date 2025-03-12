#pragma once

#include "teme/tema2/drone.h"
#include "teme/tema2/tema2_object3D.h"

using namespace teme;

static std::vector<glm::vec3> InitPropelersPos(float size) {
	float length = size;
	float width = length / 10;
	float height = width;

	float deltaL = length / 2;
	float deltaW = width / 2;
	float deltaH = height / 2;

	std::vector<glm::vec3> propelers_pos = {
		glm::vec3(-(deltaL - deltaH), height + deltaH + height / 5, 0),
		glm::vec3(deltaL - deltaH, height + deltaH + height / 5, 0),
		glm::vec3(0, height + deltaH + height / 5, deltaL - deltaH),
		glm::vec3(0, height + deltaH + height / 5, -(deltaL - deltaH))
	};

	return propelers_pos;
}

Drone::Drone(std::string name, float size, glm::vec3 initial_pos, float acceleration) {
	this->name = name;
	this->size = size;

	this->body = tema2_object3D::CreateDrone(name + "_body", size);
	this->position = initial_pos;


	this->rotation_acceleration = acceleration * 20;
	this->rotation = 45;

	this->thrust = 0;
	this->forward = 0;
	this->lateral = 0;
	this->max_speed = 5 * acceleration;
	this->acceleration = acceleration;


	this->propeler = tema2_object3D::CreatePropeler(name + "_propeler", size);
	this->propelers_pos = InitPropelersPos(size);
	this->propeler_rotation = 0;

	this->colision_radius = size / 2;
}

Drone::~Drone() {

}


void Drone::Rotate(float delta) {
	rotation -= delta * rotation_acceleration;

	if (rotation >= 360) {
		rotation -= 360;
	}
	else if (rotation < 0) {
		rotation += 360;
	}
}

void Drone::Thrust(float delta) {
	thrust += acceleration * delta;

	if (thrust > max_speed) {
		thrust = max_speed;
	}
	else if (thrust < -max_speed) {
		thrust = -max_speed;
	}
}

void Drone::Forward(float delta) {
	forward += acceleration * delta;

	if (forward > max_speed) {
		forward = max_speed;
	}
	else if (forward < -max_speed) {
		forward = -max_speed;
	}
}

void Drone::Lateral(float delta) {
	lateral += acceleration * delta;

	if (lateral > max_speed) {
		lateral = max_speed;
	}
	else if (lateral < -max_speed) {
		lateral = -max_speed;
	}
}

void Drone::Move(float delta) {
	glm::vec3 forward_speed = glm::vec3(
		sin(RADIANS(rotation + 45)) * forward,
		0,
		cos(RADIANS(rotation + 45)) * forward
	);

	glm::vec3 lateral_speed = glm::vec3(
		sin(RADIANS(rotation + 90 + 45)) * lateral,
		0,
		cos(RADIANS(rotation + 90 + 45)) * lateral
	);

	glm::vec3 speed;
	speed.x = forward_speed.x + lateral_speed.x;
	speed.z = forward_speed.z + lateral_speed.z;

	position.z += speed.z * delta;
	position.x += speed.x * delta;
	position.y += thrust * delta;
}

void Drone::RotatePropeler(float delta) {
	propeler_rotation += delta * 160;
	if (propeler_rotation >= 360) {
		propeler_rotation -= 360;
	}
	else if (propeler_rotation < 0) {
		propeler_rotation += 360;
	}
}