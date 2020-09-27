// Vehicle.cpp
// Include GLEW
#include <GL/glew.h>
#define _USE_MATH_DEFINES
#include <Windows.h>

#include "Vehicle.hpp"
#include "Modeling.hpp"
#include <iostream>


#define SCALE 0.25
//---------------------------------------------------------------------------------
extern glm::mat4 View;
extern glm::mat4 Projection;

extern GLuint objModelMatrixID;
extern GLuint objViewMatrixID;
extern GLuint objMatrixID;
//extern GLuint objLightID;
extern GLuint objTextureID;

extern GLuint carTexture;
extern GLuint car_vertexbuffer;
extern GLuint car_uvbuffer;
extern GLuint car_normalbuffer;
extern GLuint car_elementbuffer;
extern vector<unsigned short> car_indices;

// timer for random Vehicle
extern bool timeon;
int timecount;
bool randomCar;
extern vector<Vehicle* > cars;
//---------------------------------------------------------------------------------
void timer() {
	timecount = 0;
	while (timeon) {
		Sleep(400);
		// every two second
		//timecount = (timecount + 1) % 2;
		//if (timecount == 1)
		randomCar = true;
		//std::cout << "timecount: " << timecount << std::endl;
	}
}

// ranInfo vec4(x,z,velocity, qura)
vector<glm::vec4> ranInfo{glm::vec4(-(14 / (2 * SCALE) + 1), 4/SCALE, 0.018,1), // toward right closest
glm::vec4(-(14 / (2 * SCALE) + 1), 0 / SCALE, 0.035,1),
glm::vec4(-(14 / (2 * SCALE) + 1), -3 / SCALE, 0.017,1),						// toward right farthest
glm::vec4((14 / (2 * SCALE) + 1), 3 / SCALE, -0.027,0),						// toward left closest
glm::vec4((14 / (2 * SCALE) + 1), -1 / SCALE, -0.014,0),
glm::vec4((14 / (2 * SCALE) + 1), -4 / SCALE, -0.03,0),						// toward right farthest
};

void random_vehicle() {
	int r = rand() % ranInfo.size();
	//std::cout << "pick random: " << r << std::endl;
	
	cars.push_back(new Vehicle(-1,
		ranInfo[r].x, 0.72, ranInfo[r].y,
		ranInfo[r].z, 0.0 ,0.0,
		ranInfo[r].w,
		-1));
	/*cars.push_back(
		new Vehicle(-1,
			ranInfo[r].x, 0.72, 4 / 0.25,
			0.009, 0.0, 0.0,
			1,
			-1)
	);*/
	//printf("added random car\n");
	randomCar = false;
}
//---------------------------------------------------------------------------------

Vehicle::Vehicle(int _index,
	double position_x, double position_y, double position_z,
	double velocity_x, double velocity_y, double velocity_z,
	int qu,
	int max_hist) : Element(_index, position_x, position_y, position_z, velocity_x, velocity_y, velocity_z, max_hist)
{
	printf("vehicle built\n");
	myscale = SCALE;
	//std::cout << myscale << std::endl;
	// init qura orientation
	qura = qu;
	get_orientation();
	// initialize collision testv
	testv = (GLfloat*)malloc(12 * sizeof(GLfloat));
	testv[0] = -2; testv[1] = 0; testv[2] = 1;
	testv[3] = 2; testv[4] = 0; testv[5] = 1;
	testv[6] = -2; testv[7] = 0; testv[8] = -1;
	testv[9] = 2; testv[10] = 0; testv[11] = -1;
	// init_cv
	init_cv(testv, 4, glm::vec3(position_x, position_y, position_z) * myscale);
}

// draw the vehicle
void Vehicle::draw(glm::mat4 model) {
	//printf("draw vehicel\n");
	//draw_cube(model * glm::translate(glm::vec3(2,2,2)), 1.0f, 1.0f, 1.0f);

	 // set light position

	//glm::vec3 lightPos = glm::vec3(-4, 4, -4);
	//glUniform3f(objLightID, lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 RotationMatrix = orientation;//glm::mat4(); // identity    -- glm::toMat4(quat_orientations[i]);
	glm::mat4 TranslationMatrix = translate(glm::mat4(), glm::vec3(position.x, position.y, position.z));
	glm::mat4 ScaleMatrix = scale(glm::mat4(), glm::vec3(SCALE, SCALE, SCALE));
	glm::mat4 ModelMatrix = ScaleMatrix * TranslationMatrix * RotationMatrix;

	glm::mat4 MVP = Projection * View * model * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(objModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(objViewMatrixID, 1, GL_FALSE, &View[0][0]);

	// Bind this object's texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(objTextureID, 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, car_vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, car_uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, car_normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, car_elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		car_indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	return;
}

// update information for vehicle
void Vehicle::update(int fw, int fh) {
	//printf("update vehicle\n");
	//if (position.x >= fw/(2*SCALE) + 1) {
	//	glm::vec3 old = position;
	//	position.x = -(fw/(2*SCALE) + 1);

	//	// update collision vertex
	//	velocity = position - old;
	//	update_cv();
	//}
	//velocity = glm::vec3(0.009, 0.0, 0.0);
	position = position + velocity;

	update_cv();


}

void Vehicle::get_orientation() {
	//std::cout << "qura: " << qura << std::endl;
	glm::mat4 ro = glm::rotate(
		(float)(ori[qura] * M_PI),
		glm::vec3(0.0f, 1.0f, 0.0f));
	orientation = ro;
}

