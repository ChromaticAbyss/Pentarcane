#pragma once



#define GLEW_STATIC
#include <GL/glew.h>
//#include <GL/glut.h>


#include <glfw3.h>

#include "glm\glm.hpp"

#include <vector>



//TODO: Make setup for 3D, setup for 2D member functions
class OpenGLContainer{
public:
	OpenGLContainer();

	GLFWwindow* window;

	GLuint MatrixID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint TextureID;

	GLuint ModelMatrixID2D;

	GLuint vertexbuffer;
	GLuint uvbuffer;

	GLuint light_id;

	glm::mat4 PV;//Contains the projection-view matrix that is calculated once every run through the display loop and then stays the same for all objects rendered!
	glm::mat4 ViewMatrix; //The view matrix, see above

	GLuint program_id_3d;

	GLuint program_id_interface_shader;
	GLuint Grey_interface;
	GLuint C_R_interface;
	GLuint C_G_interface;
	GLuint C_B_interface;
	GLuint Transparency_interface;

	GLuint LightColourIntensity;


	int window_x;
	int window_y;
	void UpdateWindowSize();
private:

};

