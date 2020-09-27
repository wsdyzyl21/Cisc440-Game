// Creature.hpp
#ifndef CREATRUE_HPP
#define CREATURE_HPP
//---------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <glm/glm.hpp>

using namespace std;
#include "Element.hpp"
//---------------------------------------------------------------------------------
class Creature : public Element
{
public:
	GLfloat life;
	bool updating;
	GLfloat* test;
	double frameW;
	double frameH;
	double xl, xr, zu, zd;
	int qura;
	glm::mat4 orientation;
	vector<float> ori {1.0f, -0.5f, 0.0f, 0.5f};  // the four orientation of the animal
	float flatScale;


	Creature(int,				// index
		double,double,double,	// init position
		double,double,double,	// init velocity
		double, double,			// FW, FW
	int = 0);					// number of past states

	void draw(glm::mat4);
	void update(int, int, int, int, bool, bool);
	void finalize_update();
	bool out_of_bound();
	bool is_collision();
	bool obstacle_collision();
	void eat_honey();
	void get_orientation();
};
#endif