#pragma once

#include "teme/tema2/obstacle.h"
#include "teme/tema2/tema2_object3D.h"

using namespace teme;

Obstacle::Obstacle(unsigned int type, float size, glm::vec3 position, bool rotated, int nr_layers) {
	this->type = type;
	this->size = size;
	this->position = position;

	this->rotated = rotated;

	if (type == ROCK) {
		float width = size * 2.5; // IF COLLISION BAD - CHECK THE CREATION SIZE
		float length = width * 2;
		float height = width;

		collision_top_min = glm::vec3();
		collision_top_max = glm::vec3();

		if (rotated) {
			collision_base_min = position + glm::vec3(0, 0, -width);
			collision_base_max = position + glm::vec3(length, height, 0);
		}
		else {
			collision_base_min = position;
			collision_base_max = position + glm::vec3(width, height, length);
		}
	} else if (type == TREE) {
		float trunk_d = size / 1.2;
		float trunk_r = trunk_d / 2;
		float trunk_h = trunk_d * 2.5;

		collision_base_min = position + glm::vec3(-trunk_r, 0, -trunk_r);
		collision_base_max = position + glm::vec3(trunk_r, trunk_h, trunk_r);

		float leaf_d = 2 * size;
		float leaf_r = leaf_d / 2;
		float leaf_h = leaf_d;

		float dist = leaf_h / 3;

		collision_top_min = position + glm::vec3(-leaf_r, trunk_h, -leaf_r);
		collision_top_max = position + glm::vec3(leaf_r, trunk_h + leaf_h + (nr_layers - 1) * dist, leaf_r);
	}
	else {
		float width_goal = 0.2f;
		float length_goal = 2;
		float deltaL = length_goal / 2;
		float deltaW = width_goal / 2;

		collision_top_min = glm::vec3();
		collision_top_max = glm::vec3();
		switch (nr_layers)
		{
		case 0:
			if (rotated) {
				collision_base_min = position + glm::vec3(-deltaW, -width_goal, -deltaL);
				collision_base_max = position + glm::vec3(+deltaW, 0, +deltaL);
			}
			else {
				collision_base_min = position + glm::vec3(-deltaL, -width_goal, -deltaW);
				collision_base_max = position + glm::vec3(+deltaL, 0, +deltaW);
			}
			break;
		case 1:
			if (rotated) {
				collision_base_min = position + glm::vec3(-deltaW, -width_goal, deltaL);
				collision_base_max = position + glm::vec3(deltaW, length_goal + width_goal, deltaL + width_goal);
			}
			else {
				collision_base_min = position + glm::vec3(deltaL, -width_goal, -deltaW);
				collision_base_max = position + glm::vec3(deltaL + width_goal, length_goal + width_goal, +deltaW);
			}
			break;
		case 2:
			if (rotated) {
				collision_base_min = position + glm::vec3(-deltaW, -width_goal, -deltaL - width_goal);
				collision_base_max = position + glm::vec3(deltaW, length_goal + width_goal, -deltaL);
			}
			else {
				collision_base_min = position + glm::vec3(-deltaL - width_goal, -width_goal, -deltaW);
				collision_base_max = position + glm::vec3(-deltaL, length_goal + width_goal, +deltaW);
			}
			break;
		case 3:
			if (rotated) {
				collision_base_min = position + glm::vec3(-deltaW, length_goal, -deltaL);
				collision_base_max = position + glm::vec3(+deltaW, length_goal + width_goal, +deltaL);
			}
			else {
				collision_base_min = position + glm::vec3(-deltaL, length_goal, -deltaW);
				collision_base_max = position + glm::vec3(+deltaL, length_goal + width_goal, +deltaW);
			}
			break;
		default:
			collision_base_min = glm::vec3();
			collision_base_max = glm::vec3();
			printf("Obstacle.cpp - Something went bad\n");
			break;
		}
	}
}

Obstacle::~Obstacle() {

}