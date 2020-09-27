// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES

#include <thread>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
//using namespace std;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <iostream>

//#include <hw5\Modeling.cpp>


#include "Modeling.hpp"
#include "Vehicle.hpp"
#include "Creature.hpp"
#include "FixedObject.hpp"

//#define DRAW_CUBE
//---------------------------------------------------------------------------------
#define FRAME_WIDTH 1024
#define FRAME_HEIGHT 768
// the actual game
#define FW 6
#define FH 12
#define BFW 16
#define BFH 22
// for the orientation of the obj
#define FORWARD 0
#define LEFT 1
#define BACKWARD 2
#define RIGHT 3

GLuint vertexbuffer;
GLuint colorbuffer;
GLuint MatrixID;

GLuint programID;
// for object load in
GLuint objprogramID;
GLuint objMatrixID;
GLuint objViewMatrixID;
GLuint objModelMatrixID;
GLuint objTextureID;
GLuint objLightID;

// ground obj
GLuint gd_vertexbuffer, gd_uvbuffer, gd_normalbuffer;
GLuint gdTexture;
vector<glm::vec3> gd_vertices, gd_normals;
vector<glm::vec2> gd_uvs;

//vehicle obj
GLuint car_vertexbuffer, car_uvbuffer, car_normalbuffer, car_elementbuffer;
GLuint carTexture;
vector<glm::vec3> car_vertices, car_indexed_vertices;
vector<glm::vec2> car_uvs, car_indexed_uvs;
vector<glm::vec3> car_normals, car_indexed_normals;
vector<unsigned short> car_indices;

//creature obj
GLuint ani_vertexbuffer, ani_uvbuffer, ani_normalbuffer, ani_elementbuffer;
GLuint aniTexture;
vector<glm::vec3> ani_vertices, ani_indexed_vertices;
vector<glm::vec2> ani_uvs, ani_indexed_uvs;
vector<glm::vec3> ani_normals, ani_indexed_normals;
vector<unsigned short> ani_indices;
//extern GLfloat* vertex_data;
//extern int vertexNum;
extern vector<glm::vec3> jumpPoints;
int myindex = 0;

// tree obj
GLuint tree_vertexbuffer, tree_uvbuffer, tree_normalbuffer;
GLuint treeTexture;
vector<glm::vec3> tree_vertices, tree_normals;
vector<glm::vec2> tree_uvs;

// honey obj
GLuint honey_vertexbuffer, honey_uvbuffer, honey_normalbuffer;
GLuint honeyTexture;
vector<glm::vec3> honey_vertices, honey_normals;
vector<glm::vec2> honey_uvs;


glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;

// game component
Vehicle* myCar;
vector<Vehicle* > cars;
Creature* animal;
vector<FixedObject* > trees;
vector<FixedObject* > honeys;
//FixedObject* tree;

bool timeon = true;
extern int timecount;
extern bool randomCar;

// enable keyinput for movement
bool flag = true;
// win?
bool win = false;
bool winhoney = false;

// Billboard
GLuint billprogramID, CameraRight_worldspace_ID, CameraUp_worldspace_ID;
GLuint ViewProjMatrixID, BillboardPosID, BillboardSizeID, LifeLevelID;
GLuint billTextureID, billTexture;
GLuint billboard_vertex_buffer;

// some variable for test
GLuint vertexbufferc;
GLuint colorbufferc;
//---------------------------------------------------------------------------------
// declearation

// function for the game
//void draw_honey(glm::mat4);
void draw_ground(glm::mat4);
void game_initialize();
void load_game_obj();
void build_billBoard();
void draw_billBoard();
void draw_world(glm::mat4);
void update_world();
void set_camera();
//---------------------------------------------------------------------------------
void clean_up() {
	glDeleteBuffers(1, &car_vertexbuffer);
	glDeleteBuffers(1, &car_uvbuffer);
	glDeleteBuffers(1, &car_normalbuffer);
	glDeleteBuffers(1, &car_elementbuffer);

	glDeleteBuffers(1, &ani_vertexbuffer);
	glDeleteBuffers(1, &ani_uvbuffer);
	glDeleteBuffers(1, &ani_normalbuffer);
	glDeleteBuffers(1, &ani_elementbuffer);

	glDeleteBuffers(1, &tree_vertexbuffer);
	glDeleteBuffers(1, &tree_uvbuffer);
	glDeleteBuffers(1, &tree_normalbuffer);

	glDeleteBuffers(1, &honey_vertexbuffer);
	glDeleteBuffers(1, &honey_uvbuffer);
	glDeleteBuffers(1, &honey_normalbuffer);

	glDeleteProgram(objprogramID);
	glDeleteProgram(billprogramID);


}

void free_component() {
	printf("free game component\n");
	for (int i = 0; i < cars.size(); i++) {
		free(cars[i]->testv);
		delete(cars[i]);
	}
	free(animal->test);
	for (int i = 0; i < trees.size(); i++) {
		free(trees[i]->testv);
		delete(trees[i]);
	}
}
//---------------------------------------------------------------------------------
//function for the game

// draw the ground for the game
void draw_ground(glm::mat4 model) {
	
	//glm::mat4 RX = glm::rotate((float)(0.5 * M_PI),
	//	glm::vec3(1.0f,
	//		0.0f,
	//		0.0f));
	////draw_square(model * glm::translate(glm::vec3(0, 0.0f, 0)) * glm::scale(glm::vec3(2.75f, 1.0f, 6.0f))
	//	//* RX, 0.3f, 0.7f, 0.0f);
	//draw_square(model * glm::translate(glm::vec3(0, -0.001f, 0)) * glm::scale(glm::vec3(4.0f, 1.0f, 8.0f))
	//	 * RX , 0.5f, 0.5f, 0.0f);
	
	
	//draw_cube(model, 0.5f,0.5f,0.0f);
	//glUseProgram(objprogramID);
	glm::mat4 MVP = Projection * View * model;
	glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(objModelMatrixID, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(objViewMatrixID, 1, GL_FALSE, &View[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gdTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(objTextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, gd_vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, gd_uvbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, gd_normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, gd_vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
}

void game_initialize() {
	//init cars
	cars.push_back(
		new Vehicle(-1,
			-2.25 / 0.25, 0.72, 4 / 0.25,
			0.009, 0.0, 0.0,
			1,
			-1)
	);

	// init trees
	vector<glm::vec2> trees_data{	// tree position_xz
		// first line free (from left to right)
		glm::vec2(-3.15,4.6)/0.45f, glm::vec2(-1.35,4.6) / 0.45f,glm::vec2(-0.65,4.6) / 0.45f,
		glm::vec2(3.15, 4.6) / 0.45f,
		// second line free (from left to right)
		glm::vec2(-3.00, 1.6)/0.45f, glm::vec2(-2.15,1.6)/0.45f,
		glm::vec2(1.15,1.6)/0.45f, glm::vec2(2.15, 1.6)/0.45f, glm::vec2(3.00, 1.6)/0.45f,
		glm::vec2(0.25,2.1)/0.45f,
		glm::vec2(1.15,0.8)/0.45f,
		// thired line of tree
		glm::vec2(-3.00, -2.0) / 0.45f, glm::vec2(-2.15, -2.0) / 0.45f, glm::vec2(0.25,-2.0) / 0.45f,
		glm::vec2(3.15, -2.0) / 0.45f,
		// some other trees (close to far, left to right)
		glm::vec2(-3.15, 5.75)/0.45f, glm::vec2(-2.15, 6.75) / 0.45f, glm::vec2(-1.15, 6.75) / 0.45f,glm::vec2(0, 6.75) / 0.45f, glm::vec2(1.15, 6.75) / 0.45f, glm::vec2(2.15, 6.75) / 0.45f, glm::vec2(3.15, 5.75) / 0.45f,
		glm::vec2(-4.55, 4.6) / 0.45f, glm::vec2(4.55, 4.6) / 0.45f,
		glm::vec2(-4.55, 2.5) / 0.45f,glm::vec2(-3.15, 2.5)/0.45f, glm::vec2(3.15, 2.5)/0.45f, glm::vec2(4.55, 2.5) / 0.45f,
		glm::vec2(-4.55, 0.7) / 0.45f,glm::vec2(-3.15, 0.7)/0.45f, glm::vec2(3.15, 0.7)/0.45f, glm::vec2(4.55, 0.7) / 0.45f,
		glm::vec2(-4.55, -2.0) / 0.45f, glm::vec2(4.55, -2.0) / 0.45f,
		glm::vec2(-4.55, -4.85) / 0.45f, glm::vec2(-3.15, -4.85)/0.45f, glm::vec2(3.15, -4.85)/0.45f, glm::vec2(4.55, -4.85) / 0.45f,
		glm::vec2(-3.15, -5.75) / 0.45f, glm::vec2(-2.15, -6.75) / 0.45f, glm::vec2(-1.15, -6.75) / 0.45f,glm::vec2(0, -6.75) / 0.45f, glm::vec2(1.15, -6.75) / 0.45f, glm::vec2(2.15, -6.75) / 0.45f,glm::vec2(3.15, -5.75) / 0.45f

	};
	for (int i = 0; i < trees_data.size(); i++) {
		trees.push_back(new FixedObject(-1, 0.45,
			trees_data[i].x, 1, trees_data[i].y,
			-1)
		);
	}

	// init honeys
	vector<glm::vec2> honey_data{   // honey position xz
		glm::vec2(2.7, 4.6) / 0.25f,
		glm::vec2(2.25,0.5) / 0.25f,
		glm::vec2(-1.15,-2.0) / 0.25f,
		glm::vec2(0, -5.5) / 0.25f

	};
	for (int i = 0; i < honey_data.size(); i++) {
		honeys.push_back(new FixedObject(-1, 0.25,
			honey_data[i].x, 1, honey_data[i].y,
			-1)
		);
	}

	// init animal
	animal = new Creature(-1,
		0, 1.1, (FH/2/0.25) - 1,
		0.0, 0.0, 0.0,
		(double)FW, (double)FH,
		-1);
	
	
	printf("init finish\n");

}

void load_game_obj() {
	
	// ground
	loadOBJ("ground.obj", gd_vertices, gd_uvs, gd_normals);
	gdTexture = loadDDS("grassTexture.DDS");
	objTextureID = glGetUniformLocation(objprogramID, "myTextureSampler");

	glGenBuffers(1, &gd_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gd_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, gd_vertices.size() * sizeof(glm::vec3), &gd_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &gd_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gd_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, gd_uvs.size() * sizeof(glm::vec2), &gd_uvs[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &gd_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gd_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, gd_normals.size() * sizeof(glm::vec3), &gd_normals[0], GL_STATIC_DRAW);
	
	// tree
	loadOBJ("tree.obj", tree_vertices, tree_uvs, tree_normals);
	treeTexture = loadDDS("treeTexture.DDS");
	objTextureID = glGetUniformLocation(objprogramID, "myTextureSampler");

	glGenBuffers(1, &tree_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, tree_vertices.size() * sizeof(glm::vec3), &tree_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tree_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tree_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, tree_uvs.size() * sizeof(glm::vec2), &tree_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tree_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tree_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, tree_normals.size() * sizeof(glm::vec3), &tree_normals[0], GL_STATIC_DRAW);

	// honey
	loadOBJ("honey.obj", honey_vertices, honey_uvs, honey_normals);
	honeyTexture = loadDDS("honeyTexture.DDS");
	objTextureID = glGetUniformLocation(objprogramID, "myTextureSampler");

	glGenBuffers(1, &honey_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, honey_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, honey_vertices.size() * sizeof(glm::vec3), &honey_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &honey_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, honey_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, honey_uvs.size() * sizeof(glm::vec2), &honey_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &honey_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, honey_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, honey_normals.size() * sizeof(glm::vec3), &honey_normals[0], GL_STATIC_DRAW);

	// car
	loadOBJ("car.obj", car_vertices, car_uvs, car_normals);
	//	printf("load cube failed\n");
	carTexture = loadDDS("carTexture.DDS");
	objTextureID = glGetUniformLocation(objprogramID, "myTextureSampler");

	indexVBO(car_vertices, car_uvs, car_normals, car_indices, car_indexed_vertices, car_indexed_uvs, car_indexed_normals);

	// Load into array buffers

	glGenBuffers(1, &car_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, car_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, car_indexed_vertices.size() * sizeof(glm::vec3), &car_indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &car_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, car_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, car_indexed_uvs.size() * sizeof(glm::vec2), &car_indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &car_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, car_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, car_indexed_normals.size() * sizeof(glm::vec3), &car_indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &car_elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, car_elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, car_indices.size() * sizeof(unsigned short), &car_indices[0], GL_STATIC_DRAW);
	
	// animal
	loadOBJ("bear.obj", ani_vertices, ani_uvs, ani_normals);
	//	printf("load cube failed\n");
	aniTexture = loadDDS("bearTexture.DDS");
	objTextureID = glGetUniformLocation(objprogramID, "myTextureSampler");

	indexVBO(ani_vertices, ani_uvs, ani_normals, ani_indices, ani_indexed_vertices, ani_indexed_uvs, ani_indexed_normals);

	// Load into array buffers

	glGenBuffers(1, &ani_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ani_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, ani_indexed_vertices.size() * sizeof(glm::vec3), &ani_indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ani_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ani_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, ani_indexed_uvs.size() * sizeof(glm::vec2), &ani_indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ani_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ani_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, ani_indexed_normals.size() * sizeof(glm::vec3), &ani_indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &ani_elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ani_elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ani_indices.size() * sizeof(unsigned short), &ani_indices[0], GL_STATIC_DRAW);
	
}

void build_billBoard() {
	// build billboard
	billprogramID = LoadShaders("Billboard.vertexshader", "Billboard.fragmentshader");

	// Vertex shader
	CameraRight_worldspace_ID = glGetUniformLocation(billprogramID, "CameraRight_worldspace");
	CameraUp_worldspace_ID = glGetUniformLocation(billprogramID, "CameraUp_worldspace");
	ViewProjMatrixID = glGetUniformLocation(billprogramID, "VP");
	BillboardPosID = glGetUniformLocation(billprogramID, "BillboardPos");
	BillboardSizeID = glGetUniformLocation(billprogramID, "BillboardSize");
	LifeLevelID = glGetUniformLocation(billprogramID, "LifeLevel");

	billTextureID = glGetUniformLocation(billprogramID, "mybillTexture");


	billTexture = loadDDS("ExampleBillboard.DDS");

	// The VBO containing the 4 vertices of the particles.
	static const GLfloat g_vertex_buffer_data[] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
}

void draw_billBoard() {
	// We will need the camera's position in order to sort the particles
			// w.r.t the camera's distance.
			// There should be a getCameraPosition() function in common/controls.cpp, 
			// but this works too.
	glm::vec3 CameraPosition(glm::inverse(View)[3]);

	glm::mat4 ViewProjectionMatrix = Projection * View;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader
	glUseProgram(billprogramID);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(billTextureID, 0);

	// This is the only interesting part of the tutorial.
	// This is equivalent to mlutiplying (1,0,0) and (0,1,0) by inverse(ViewMatrix).
	// ViewMatrix is orthogonal (it was made this way), 
	// so its inverse is also its transpose, 
	// and transposing a matrix is "free" (inversing is slooow)
	glUniform3f(CameraRight_worldspace_ID, View[0][0], View[1][0], View[2][0]);
	glUniform3f(CameraUp_worldspace_ID, View[0][1], View[1][1], View[2][1]);

	glUniform3f(BillboardPosID, animal->position.x * animal->myscale, animal->position.y * animal->myscale + 0.5f, animal->position.z * animal->myscale); // The billboard will be just above the cube
	glUniform2f(BillboardSizeID, 1.0f, 0.125f);     // and 1m*12cm, because it matches its 256*32 resolution =)

	// Generate some fake life level and send it to glsl
	//float LifeLevel = 
	glUniform1f(LifeLevelID, animal->life);

	glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);


	// Draw the billboard !
	// This draws a triangle_strip which looks like a quad.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0);
}

void draw_world(glm::mat4 model) {
	// draw trees
	//tree->draw(model);
	for (int i = 0; i < trees.size(); i++) {
		trees[i]->draw(model);
	}

	// draw cars
	for (int i = 0; i < cars.size(); i++) {
		cars[i]->draw(model);
	}

	// draw animal
	if (!animal->updating) {
		animal->draw(model);
	}
	else {

		animal->position = jumpPoints[myindex];
		animal->draw(Model);
		myindex++;
		if (myindex >= jumpPoints.size()) {
			animal->finalize_update();
		}
	}

	//draw honey
	//draw_honey(model);
	for (int i = 0; i < honeys.size(); i++) {
		honeys[i]->draw_honey(model);
	}
}

void update_world() {
	//printf("update_world!!!!!!!!!!!!!!!!!!!!!!!!!!");
	
	// check if win
	if (animal->life >= 0.24f * 4) {
		//printf("win!!!!!!!!!\n");
		win = true;
		if (winhoney && !animal->updating) {
			// generate win honey
			printf("generate win honey\n");
			winhoney = false;
			if (animal->life >= 0.24f * 4) {
				honeys.push_back(new FixedObject(-1, 0.25,
					animal->new_postion.x, animal->new_postion.y + 4.0f, animal->new_postion.z,
					-1));
			}
			animal->qura = 2;
			animal->get_orientation();
		}
		// disable animal moves
		flag = false;
		if (honeys.size() != 0) {
			if (honeys[0]->position.y > animal->new_postion.y + 2.0f)
				honeys[0]->position = glm::vec3(honeys[0]->position.x, honeys[0]->position.y - 0.005, honeys[0]->position.z);
		}
	}
	// update
	for (auto i = cars.begin(); i != cars.end();) {
		// remove
		if (((*i)->qura == 1 && (*i)->position.x >= BFW / (2 * (*i)->myscale) + 1) ||
			((*i)->qura == 0 && (*i)->position.x <= -(BFW / (2 * (*i)->myscale) + 1))
			) {
			i = cars.erase(i);
			//i--;
			continue;
		}
		//update
		(*i)->update(FW, FH);
		++i;
	}
	// update  random car 
	if (randomCar) {
		random_vehicle();
		//printf("random add finish\n");
	}

	// check if collision animal and cars
	if (animal->is_collision() && !animal->updating && !win) {
		//printf("collision with car!!!!!!!!!!!!!\n");
		animal->flatScale = 0.01;
		flag = false;
	}
	
}
//---------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//printf("catch key input\n");
	bool vertical;
	bool forward;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && flag) {
		animal->velocity = glm::vec3(0, 0, -2);
		animal->qura = 0;
		//animal->update(FW, FH, 0, -0.25);
		vertical = true;
		forward = true;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && flag) {
		animal->velocity = glm::vec3(0, 0, 2);
		animal->qura = 2;
		//animal->update(FW, FH, 0, 0.25);
		vertical = true;
		forward = false;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS && flag) {
		animal->velocity = glm::vec3(-2, 0, 0);
		animal->qura = 1;
		//animal->update(FW, FH, -0.25, 0);
		vertical = false;
		forward = false;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && flag) {
		animal->velocity = glm::vec3(2, 0, 0);
		animal->qura = 3;
		//animal->update(FW, FH, 0.25, 0);
		vertical = false;
		forward = true;
	}
	/*else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		flag = !flag;
		return;
	}*/
	else if (key == GLFW_KEY_R && action == GLFW_PRESS && !animal->updating) {
		printf("restart game !!!!!: ");
		flag = true;
		win = false;
		free_component();
		cars.clear();
		trees.clear();
		honeys.clear();
		game_initialize();
		return;
	}
	else {
		return;
	}
	if (!animal->updating) {
		//printf("updaing no update!!!!!!!!\n");
		animal->update(FW, FH, 0, 0, vertical, forward);
		myindex = 0;
	}
	
}

void set_camera() {
	float x = animal->position.x * 0.25;
	float y = animal->position.y * 0.25;
	float z = animal->position.z * 0.25;
	float cam_x = x;
	float cam_y = 4;
	float cam_z = z + 4;
	View = glm::lookAt(
		glm::vec3(cam_x, cam_y, cam_z), // Camera is at (4,3,3), in World Space
		glm::vec3(x, 0, z), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "MyGame", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, FRAME_WIDTH / 2, FRAME_HEIGHT / 2);

	// Dark blue background
	glClearColor(0.0f, 0.4f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);


	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("Myshader.vertexshader", "Myshader.fragmentshader");
	objprogramID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");


	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");

	objMatrixID = glGetUniformLocation(objprogramID, "MVP");
	objViewMatrixID = glGetUniformLocation(objprogramID, "V");
	objModelMatrixID = glGetUniformLocation(objprogramID, "M");

	
	objLightID = glGetUniformLocation(objprogramID, "LightPosition_worldspace");
	

	// build billboard
	build_billBoard();

	
	// load obj and texture
	load_game_obj();

	//initialize game component
	game_initialize();
	// start the timer
	thread t1(timer);
	do {
		
		//if (flag) {
			update_world();
			
		//}
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// catch key input
		glfwSetKeyCallback(window, key_callback);
		
		computeMatricesFromInputs();
		// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		//Projection = getProjectionMatrix();
		Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		// Or, for an ortho camera :
		//Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

		// Camera matrix
		//View = getViewMatrix();
		set_camera();

	// Model matrix : an identity matrix (model will be at the origin)
		Model = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		
		// Use our shader
		//glUseProgram(programID);
		glDisable(GL_BLEND);
		glUseProgram(objprogramID);

		glm::vec3 lightPos = glm::vec3(-4, 6, -7);
		glUniform3f(objLightID, lightPos.x, lightPos.y, lightPos.z);
		
		

		//glDisableVertexAttribArray(0);
		draw_ground(Model);

		
		
		//myCar->draw(Model);
		draw_world(Model);
		
		// draw billboard
		draw_billBoard();
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	// destory timer thread 
	timeon = false;
	t1.join();
	t1.~thread();
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	clean_up();
	free_component();
	

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


