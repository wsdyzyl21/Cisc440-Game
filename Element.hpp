//ELement.hpp
#ifndef ELEMENT_HPP

#define ELEMENT_HPP

//---------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/random.hpp>

using namespace std;
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
class Element
{
public:
	int index;

	// current postion and velocity
	glm::vec3 position;
	glm::vec3 velocity;
	vector<glm::vec3> collision_vertex;
	float myscale;

	// the postion in the next time step
	glm::vec3 new_postion;

	Element(int,					// index
		double, double, double,		// init position
		double, double, double,		// init velocity
		int = 1);					// number of past states to save

	//void final_update();
	void init_cv(GLfloat* , int, glm::vec3);
	void initialize_cv(glm::vec3);
	void update_cv();

	
};


#endif