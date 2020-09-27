// Element.cpp
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <common/objloader.hpp>

#include "Element.hpp"
#include <iostream>
//---------------------------------------------------------------------------------
extern glm::mat4 Projection;
extern glm::mat4 View;
extern GLuint MatrixID;
extern GLuint vertexbuffer;
extern GLuint colorbuffer;
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
Element::Element(int _index,
	double position_x, double position_y, double position_z,
	double velocity_x, double velocity_y, double velocity_z,
	int max_hist) 
{
	index = _index;
	position = glm::vec3(position_x, position_y, position_z);
	velocity = glm::vec3(velocity_x, velocity_y, velocity_z);
}

// initialize the collision vertex
void Element::init_cv(GLfloat* vbuff, int vnum, glm::vec3 newp) {
	// initilize it
	//printf("collision vertex initialize\n");
	for (int i = 0; i < vnum * 3;) {
		collision_vertex.push_back(glm::vec3(vbuff[i], vbuff[i + 1], vbuff[i + 2]) * myscale);
		//std::cout << vbuff[i] << ", " << vbuff[i + 1] << ", " << vbuff[i + 2] << std::endl;
		i += 3;
	}

	//printf("collsion vertex after move\n");
	// and move it to correct location
	for (int i = 0; i < collision_vertex.size(); i++) {
		// since the center should be at origin
		collision_vertex[i] += newp;
		//std::cout << collision_vertex[i].x << ", " << collision_vertex[i].y << "," << collision_vertex[i].z << std::endl;
	}
}

// update collision vertex while update Creature or Vehicle
void Element::update_cv() {
	//printf("collsiion vertex after update (need change scale)\n");
	for (int i = 0; i < collision_vertex.size(); i++) {
		collision_vertex[i] += velocity * 0.25f;
		//std::cout << collision_vertex[i].x << ", " << collision_vertex[i].y << "," << collision_vertex[i].z << std::endl;
	}
}