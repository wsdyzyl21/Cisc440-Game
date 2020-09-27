// Vehicle.hpp
#ifndef VECHICLE_HPP
#define VECHICLE_HPP
//---------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace std;

#include "Element.hpp"


//---------------------------------------------------------------------------------

class Vehicle : public Element
{
public:
	int index;
	GLfloat* testv;
	int qura;
	glm::mat4 orientation;
	vector<float> ori{ -0.5f, 0.5f};

	Vehicle(int,					// index
		double, double, double,		// init position 
		double, double, double,		// init velocity
		int,						// qura orientation
		int = 1);					// number of past states

	void draw(glm::mat4);
	void update(int fw, int fh);
	void get_orientation();
	

};

//---------------------------------------------------------------------------------
// function for timer and random
void timer();
void random_vehicle();

#endif