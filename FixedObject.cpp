// FixedObject.cpp
// Include GLEW
#include <GL/glew.h>
#define _USE_MATH_DEFINES
//#include <Windows.h>

//#include "Vehicle.hpp"
//#include "Modeling.hpp"
#include "FixedObject.hpp"
#include <iostream>


#define SCALE 0.45
//---------------------------------------------------------------------------------
extern glm::mat4 View;
extern glm::mat4 Projection;

extern GLuint objModelMatrixID;
extern GLuint objViewMatrixID;
extern GLuint objMatrixID;
//extern GLuint objLightID;
extern GLuint objTextureID;

extern GLuint tree_vertexbuffer;
extern GLuint tree_uvbuffer;
extern GLuint tree_normalbuffer;
extern GLuint treeTexture;
extern vector<glm::vec3> tree_vertices;

extern GLuint honey_vertexbuffer;
extern GLuint honey_uvbuffer;
extern GLuint honey_normalbuffer;
extern GLuint honeyTexture;
extern vector<glm::vec3> honey_vertices;

float shiftingScale = 0.25f;
float shifting = 0.00001f;
float scale_shift() {
	if (shiftingScale >= 0.27f) {
		shifting = -0.00001f;
	}
	else if (shiftingScale <= 0.23f) {
		shifting = 0.00001f;
	}
	shiftingScale += shifting;
	return shiftingScale;
}

//---------------------------------------------------------------------------------

FixedObject::FixedObject(int _index, double my_scale,
	double position_x, double position_y, double position_z,
	int max_hist) : Element(_index, position_x, position_y, position_z, 0, 0, 0, max_hist)
{
	//printf("FixedObject built\n");
	myscale = my_scale;
	
	// initialize collision testv
	testv = (GLfloat*)malloc(12 * sizeof(GLfloat));
	testv[0] = -1; testv[1] = 0; testv[2] = 1;
	testv[3] = 1; testv[4] = 0; testv[5] = 1;
	testv[6] = -1; testv[7] = 0; testv[8] = -1;
	testv[9] = 1; testv[10] = 0; testv[11] = -1;
	// init_cv
	init_cv(testv, 4, glm::vec3(position_x, position_y, position_z) * myscale);
}

void FixedObject::draw(glm::mat4 model) {
	glm::mat4 R = glm::rotate((float)(1.0 * M_PI),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 RotationMatrix = R; // identity    -- glm::toMat4(quat_orientations[i]);
	glm::mat4 TranslationMatrix = translate(glm::mat4(), glm::vec3(position.x, position.y, position.z));
	glm::mat4 ScaleMatrix = scale(glm::mat4(), glm::vec3(SCALE, SCALE, SCALE));
	glm::mat4 ModelMatrix = ScaleMatrix * TranslationMatrix * RotationMatrix;

	glm::mat4 MVP = Projection * View * model * ModelMatrix;
	glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(objModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(objViewMatrixID, 1, GL_FALSE, &View[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, treeTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(objTextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, tree_vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, tree_uvbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, tree_normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);



	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, tree_vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void FixedObject::draw_honey(glm::mat4 model) {
	glm::mat4 TranslationMatrix = translate(glm::mat4(), glm::vec3(position.x, position.y, position.z));
	glm::mat4 ScaleMatrix = scale(glm::mat4(), glm::vec3(myscale, myscale, myscale));
	glm::mat4 ModelMatrix = ScaleMatrix * TranslationMatrix;

	glm::mat4 MVP = Projection * View * model * ModelMatrix;
	glUniformMatrix4fv(objMatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(objModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(objViewMatrixID, 1, GL_FALSE, &View[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, honeyTexture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(objTextureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, honey_vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, honey_uvbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, honey_normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);



	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, honey_vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}