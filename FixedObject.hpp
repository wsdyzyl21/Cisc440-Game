// FixedObject.hpp
#ifndef FIXEDOBJECT_HPP
#define FIXEDOBJECT_HPP
//---------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <GL\glew.h>
using namespace std;

#include "Element.hpp"

//---------------------------------------------------------------------------------
class FixedObject : public Element
{
public:
	GLfloat* testv;

	FixedObject(int,double,
		double, double, double,		// init position
		int i = 1);

	void draw(glm::mat4);
	void draw_honey(glm::mat4);
};

#endif