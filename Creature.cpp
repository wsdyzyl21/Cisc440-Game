// Creature.cpp
// Include GLEW
#include <GL/glew.h>
#define _USE_MATH_DEFINES

#include "Creature.hpp"
#include "Modeling.hpp"
#include "Vehicle.hpp"
#include "FixedObject.hpp"
#include <iostream>

#define SCALE 0.25
#define CONTROL_Y 3.5
#define CONTROL_XZ 0.5
//---------------------------------------------------------------------------------
extern glm::mat4 View;
extern glm::mat4 Projection;
extern glm::mat4 Model;
extern GLuint MatrixID;
//extern GLuint programID;

//extern GLuint objprogramID;;
extern GLuint objModelMatrixID;
extern GLuint objViewMatrixID;
extern GLuint objMatrixID;
//extern GLuint objLightID;

extern GLuint objTextureID;

extern GLuint aniTexture;
extern GLuint ani_vertexbuffer;
extern GLuint ani_uvbuffer;
extern GLuint ani_normalbuffer;
extern GLuint ani_elementbuffer;
extern vector<unsigned short> ani_indices;

// car list
extern vector<Vehicle* > cars;
// tree list
extern vector<FixedObject* > trees;
// honey list
extern vector<FixedObject* > honeys;

// some variable for test
//int vertexNum;
//GLfloat* vertex_data = NULL;
vector<glm::vec3> jumpPoints;
//GLfloat* color_data = NULL;
extern GLuint vertexbufferc;
extern GLuint colorbufferc;

//win 
extern bool winhoney;


//---------------------------------------------------------------------------------
// implement the algorithm to get a curve smooth path for the move of animal

struct sPoint3D {
	float x;
	float y;
	float z;
};

//vector<sPoint3D> control_points;

// calculate control points
vector<sPoint3D> calculate_control(glm::vec3 start, glm::vec3 end, bool vertical, bool forward) {
	vector<sPoint3D> control_points;
	glm::vec3 ts, te;
	if (vertical) {
		if (forward) {
			ts = glm::vec3(0, -CONTROL_Y, CONTROL_XZ);
			te = glm::vec3(0, -CONTROL_Y, -CONTROL_XZ);
		}
		else {
			ts = glm::vec3(0, -CONTROL_Y, -CONTROL_XZ);
			te = glm::vec3(0, -CONTROL_Y, CONTROL_XZ);
		}
	}
	else {
		if (forward) {
			ts = glm::vec3(-CONTROL_XZ, -CONTROL_Y, 0);
			te = glm::vec3(CONTROL_XZ, -CONTROL_Y, 0);
		}
		else {
			ts = glm::vec3(CONTROL_XZ, -CONTROL_Y, 0);
			te = glm::vec3(-CONTROL_XZ, -CONTROL_Y, 0);
		}
	}
	glm::vec3 s1 = start + ts;
	glm::vec3 e1 = end + te;
	control_points.push_back({s1.x, s1.y, s1.z});
	control_points.push_back({ start.x, start.y, start.z });
	control_points.push_back({ end.x, end.y, end.z });
	control_points.push_back({ e1.x, e1.y, e1.z });

	// check if those points are correct
	//printf("calculate control points\n");
	/*
	for (int i = 0; i < control_points.size(); i++) {
		std::cout << control_points[i].x << ", " << control_points[i].y << ", " << control_points[i].z << std::endl;
	}
	*/

	return  control_points;
}
sPoint3D getSplinePoint(float t, vector<sPoint3D> control_points) {
	int p0, p1, p2, p3;
	p1 = (int)t + 1;
	p2 = p1 + 1;
	p3 = p2 + 1;
	p0 = p1 - 1;

	t = t - (int)t;
	float tt = t * t;
	float ttt = tt * t;

	float q1 = -ttt + 2.0f * tt - t;
	float q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
	float q3 = -3.0f * ttt + 4.0f * tt + t;
	float q4 = ttt - tt;

	float tx = 0.5f * (control_points[p0].x * q1 + control_points[p1].x * q2 + control_points[p2].x * q3 + control_points[p3].x * q4);
	float ty = 0.5f * (control_points[p0].y * q1 + control_points[p1].y * q2 + control_points[p2].y * q3 + control_points[p3].y * q4);
	float tz = 0.5f * (control_points[p0].z * q1 + control_points[p1].z * q2 + control_points[p2].z * q3 + control_points[p3].z * q4);

	return { tx,ty,tz };
}

void getSpline(vector<sPoint3D> control_points) {
	int index = 0;
	//loop
	//vector<double> vec;
	for (float t = 0.0f; t < (float)control_points.size() - 3.0f; t += 0.008f) {
		// call getSplinePoint to get point
		// add point to vertex_data
		sPoint3D pos = getSplinePoint(t, control_points);
		jumpPoints.push_back(glm::vec3(pos.x, pos.y, pos.z));
		index += 3;
	}
	
}


//---------------------------------------------------------------------------------
Creature::Creature(int _index,
	double position_x, double position_y, double position_z,
	double velocity_x, double velocity_y, double velocity_z,
	double fw, double fh,
	int max_hist) : Element(_index, position_x, position_y, position_z, velocity_x, velocity_y, velocity_z, max_hist)
{
	printf("Creature built\n");
	myscale = SCALE;
	flatScale = SCALE;
	life = 0.0f;
	updating = false;
	frameW = fw;
	frameH = fh;
	// calcaluate the four boundaries 
	xl = -(fw / 2);
	xr = fw / 2;
	zu = -(fh / 2);
	zd = fh / 2;
	// init qura 
	qura = 2;
	// init orientation
	orientation = glm::mat4();
	//init collision test
	test = (GLfloat*)malloc(12 * sizeof(GLfloat));
	test[0] = -0.76f; test[1] = 0; test[2] = 0.76f;
	test[3] = 0.76f; test[4] = 0; test[5] = 0.76f;
	test[6] = -0.76f; test[7] = 0; test[8] = -0.76f;
	test[9] = 0.76f; test[10] = 0; test[11] = -0.76f;

	// notice to scale the second arg back
	init_cv(test, 4, glm::vec3(position_x, position_y, position_z) * myscale);
}

void Creature::draw(glm::mat4 model) {
	// set light position
	//std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
	//glm::vec3 lightPos = glm::vec3(-4, 4, -4);
	//glUniform3f(objLightID, lightPos.x, lightPos.y, lightPos.z);

	

	glm::mat4 RotationMatrix = orientation;//glm::mat4(); // identity    -- glm::toMat4(quat_orientations[i]);
	glm::mat4 TranslationMatrix = translate(glm::mat4(), glm::vec3(position.x, position.y, position.z));
	glm::mat4 ScaleMatrix = scale(glm::mat4(), glm::vec3(SCALE, flatScale, SCALE));
	glm::mat4 ModelMatrix = ScaleMatrix  * TranslationMatrix * RotationMatrix;

	glm::mat4 MVP = Projection * View * model * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(objModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(objViewMatrixID, 1, GL_FALSE, &View[0][0]);

	// Bind this object's texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aniTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(objTextureID, 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, ani_vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, ani_uvbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, ani_normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ani_elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		ani_indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//printf("draw");
	return;
}

void Creature::update(int fw, int fh, int tx, int tz, bool vertical, bool forward) {
	new_postion = position + velocity;
	// update orientation
	get_orientation();
	if (out_of_bound()) {
		printf("at edge\n");
		return;
	}
	if (obstacle_collision()) {
		printf("hit tree\n");
		return;
	}
	eat_honey();
	//printf("update animal position\n");
	updating = true;
	//position = glm::vec3(position.x + tx, position.y, position.z + tz);
	//position = position + velocity;
	
	vector<sPoint3D> cc = calculate_control(position, new_postion, vertical, forward);
	getSpline(cc);

	// update the collsiion vertex
	update_cv();
	
	//position = new_postion;
	//std::cout << position.x << ", " << position.z << std::endl;
}

void Creature::finalize_update() {
	updating = false;
	position = new_postion;
	// free vertex_data?????
	//free(vertex_data);
	std::cout << "animal position_xz: " << position.x * SCALE << ", " << position.z * SCALE << std::endl;
	jumpPoints.clear();
}

bool Creature::out_of_bound() {
	//return false;
	//std::cout << new_postion.x * SCALE<< ", " << new_postion.z * SCALE << std::endl;
	return new_postion.x * SCALE - SCALE < xl || new_postion.x * SCALE + SCALE > xr || 
		new_postion.z * SCALE - SCALE < zu || new_postion.z * SCALE + SCALE > zd;
}

bool Creature::is_collision() {
	for (int i = 0; i < cars.size(); i++) {
		// left_down
		bool ld = collision_vertex[0].x >= cars[i]->collision_vertex[0].x && collision_vertex[0].x <= cars[i]->collision_vertex[1].x &&
			collision_vertex[0].z >= cars[i]->collision_vertex[2].z && collision_vertex[0].z <= cars[i]->collision_vertex[0].z;
		// right_down
		bool rd = collision_vertex[1].x >= cars[i]->collision_vertex[0].x && collision_vertex[1].x <= cars[i]->collision_vertex[1].x &&
			collision_vertex[1].z >= cars[i]->collision_vertex[2].z && collision_vertex[1].z <= cars[i]->collision_vertex[0].z;
		// left_up
		bool lu = collision_vertex[2].x >= cars[i]->collision_vertex[0].x && collision_vertex[2].x <= cars[i]->collision_vertex[1].x &&
			collision_vertex[2].z >= cars[i]->collision_vertex[2].z && collision_vertex[2].z <= cars[i]->collision_vertex[0].z;
		// right_up
		bool ru = collision_vertex[3].x >= cars[i]->collision_vertex[0].x && collision_vertex[3].x <= cars[i]->collision_vertex[1].x &&
			collision_vertex[3].z >= cars[i]->collision_vertex[2].z && collision_vertex[3].z <= cars[i]->collision_vertex[0].z;

		if (ld || rd || lu || ru) {
			return false;
		}

	}
	return false;
}

bool Creature::obstacle_collision() {
	glm::vec3 move = (new_postion - position) * myscale;
	glm::vec3 cv0 = collision_vertex[0] + move;
	glm::vec3 cv1 = collision_vertex[1] + move;
	glm::vec3 cv2 = collision_vertex[2] + move;
	glm::vec3 cv3 = collision_vertex[3] + move;
	for (int i = 0; i < trees.size(); i++) {
		// left_down
		bool ld = cv0.x >= trees[i]->collision_vertex[0].x && cv0.x <= trees[i]->collision_vertex[1].x &&
			cv0.z >= trees[i]->collision_vertex[2].z && cv0.z <= trees[i]->collision_vertex[0].z;
		// right_down
		bool rd = cv1.x >= trees[i]->collision_vertex[0].x && cv1.x <= trees[i]->collision_vertex[1].x &&
			cv1.z >= trees[i]->collision_vertex[2].z && cv1.z <= trees[i]->collision_vertex[0].z;
		// left_up
		bool lu = cv2.x >= trees[i]->collision_vertex[0].x && cv2.x <= trees[i]->collision_vertex[1].x &&
			cv2.z >= trees[i]->collision_vertex[2].z && cv2.z <= trees[i]->collision_vertex[0].z;
		// right_up
		bool ru = cv3.x >= trees[i]->collision_vertex[0].x && cv3.x <= trees[i]->collision_vertex[1].x &&
			cv3.z >= trees[i]->collision_vertex[2].z && cv3.z <= trees[i]->collision_vertex[0].z;

		if (ld || rd || lu || ru) {
			return true;
		}

	}
	return false;
}

void Creature::eat_honey() {
	glm::vec3 move = (new_postion - position) * myscale;
	glm::vec3 cv0 = collision_vertex[0] + move;
	glm::vec3 cv1 = collision_vertex[1] + move;
	glm::vec3 cv2 = collision_vertex[2] + move;
	glm::vec3 cv3 = collision_vertex[3] + move;

	for (auto i = honeys.begin(); i != honeys.end();) {
		bool ld = cv0.x >= (*i)->collision_vertex[0].x && cv0.x <= (*i)->collision_vertex[1].x &&
			cv0.z >= (*i)->collision_vertex[2].z && cv0.z <= (*i)->collision_vertex[0].z;
		// right_down
		bool rd = cv1.x >= (*i)->collision_vertex[0].x && cv1.x <= (*i)->collision_vertex[1].x &&
			cv1.z >= (*i)->collision_vertex[2].z && cv1.z <= (*i)->collision_vertex[0].z;
		// left_up
		bool lu = cv2.x >= (*i)->collision_vertex[0].x && cv2.x <= (*i)->collision_vertex[1].x &&
			cv2.z >= (*i)->collision_vertex[2].z && cv2.z <= (*i)->collision_vertex[0].z;
		// right_up
		bool ru = cv3.x >= (*i)->collision_vertex[0].x && cv3.x <= (*i)->collision_vertex[1].x &&
			cv3.z >= (*i)->collision_vertex[2].z && cv3.z <= (*i)->collision_vertex[0].z;
		
		if (ld || rd || lu || ru) {
			printf("eat honey!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			life += 0.24f;
			i = honeys.erase(i);
			if (life >= 0.24f * 4) {
				winhoney = true;
			}
			continue;
		}
		++i;

	}
}

void Creature::get_orientation() {
	std::cout <<"qura: " <<qura << std::endl;
	glm::mat4 ro = glm::rotate(
		(float)(ori[qura] * M_PI),
		glm::vec3(0.0f, 1.0f, 0.0f));
	orientation = ro;
}