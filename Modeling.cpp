// Modeling.cpp

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

#include "Modeling.hpp"
//---------------------------------------------------------------------------------
extern glm::mat4 Projection;
extern glm::mat4 View;
extern GLuint MatrixID;
extern GLuint vertexbuffer;
extern GLuint colorbuffer;

//---------------------------------------------------------------------------------
// some test function
void draw_cube(glm::mat4 Model, float r, float g, float b)
{
	//printf("wtf\n");
	// +Z, -Z

	draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, +1.0f)), r, g, b);
	draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, -1.0f)), 0.75 * r, 0.75 * g, 0.75 * b);

	// +X, -X

	glm::mat4 RY = glm::rotate((float)(0.5 * M_PI),
		glm::vec3(0.0f,
			1.0f,
			0.0f));

	draw_square(Model * glm::translate(glm::vec3(+1.0f, 0.0f, 0.0f)) * RY, 0.5 * r, 0.5 * g, 0.5 * b);
	draw_square(Model * glm::translate(glm::vec3(-1.0f, 0.0f, 0.0f)) * RY, 0.5 * r, 0.5 * g, 0.5 * b);

	// +Y, -Y

	glm::mat4 RX = glm::rotate((float)(0.5 * M_PI),
		glm::vec3(1.0f,
			0.0f,
			0.0f));

	draw_square(Model * glm::translate(glm::vec3(0.0f, +1.0f, 0.0f)) * RX, 0.35 * r, 0.35 * g, 0.35 * b);
	draw_square(Model * glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)) * RX, 0.5 * r, 0.5 * g, 0.5 * b);

}

void draw_square(glm::mat4 Model, float r, float g, float b)
{
	glm::mat4 M = glm::scale(glm::vec3(-1.0f, -1.0f, 0.0f));

	draw_right_triangle(Model * M, r, g, b);
	draw_right_triangle(Model, r, g, b);
}


// with shear, bottom-left at (-1, -1), bottom-right at (1, -1),
// top-right at (1, 1)

void draw_right_triangle(glm::mat4 Model, float r, float g, float b)
{
	glm::mat4 S = glm::shearX3D(glm::mat4(1.0f), 0.5f, 0.0f);
	glm::mat4 T = glm::translate(glm::vec3(-1.0f, 1.0f, 0.0f));

	draw_triangle(Model * glm::inverse(T) * S * T, r, g, b);
}


// bottom-left at (-1, -1), bottom-right at (1, -1),
// top at (0, 1)

// Draw triangle with particular modeling transformation and color (r, g, b) (in range [0, 1])
// Refers to globals in section above (but does not change them)

void draw_triangle(glm::mat4 Model, float r, float g, float b)
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	// make this transform available to shaders  
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0,                  // attribute. 0 to match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// all vertices same color
	//std::cout << "r,g,b: " << r << ", " << g << "," << b << std::endl;
	GLfloat g_color_buffer_data[] = {
	  r, g, b,
	  r, g, b,
	  r, g, b,
	};

	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(1,                                // attribute. 1 to match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
//---------------------------------------------------------------------------------