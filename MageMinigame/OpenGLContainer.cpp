
//#include <stdio.h>

#include <string>
#include <vector>


#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "Log.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include "shader.hpp"

#include "OpenGLContainer.h"

using namespace std;
using namespace glm;



OpenGLContainer::OpenGLContainer() 
	:window_x(1600),window_y(900)
{

	if (!glfwInit())
	{
		fprintf_s(stderr, "Failed to initialize GLFW\n");
		return;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_x, window_y, "Mage Minigame", NULL, NULL);
	if (window == NULL) {
		fprintf_s(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf_s(stderr, "Failed to initialize GLEW\n");
		return;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, window_x / 2, window_y / 2);



	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	//GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );
	//TEST: Advanced Shader
	program_id_3d = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(program_id_3d, "MVP");
	ViewMatrixID = glGetUniformLocation(program_id_3d, "V");
	ModelMatrixID = glGetUniformLocation(program_id_3d, "M");
	TextureID = glGetUniformLocation(program_id_3d, "myTextureSampler");
	// Get a handle for our "LightPosition" uniform
	light_id = glGetUniformLocation(program_id_3d, "LightPosition_worldspace");

	LightColourIntensity = glGetUniformLocation(program_id_3d, "LightColourIntensity");

	

	 program_id_interface_shader = LoadShaders("InterfaceShading.vertexshader", "InterfaceShading.fragmentshader");

	 ModelMatrixID2D = glGetUniformLocation(program_id_interface_shader, "M");

	Grey_interface = glGetUniformLocation(program_id_interface_shader, "Grey");
	C_R_interface = glGetUniformLocation(program_id_interface_shader, "Red");
	C_G_interface = glGetUniformLocation(program_id_interface_shader, "Green");
	C_B_interface = glGetUniformLocation(program_id_interface_shader, "Blue");
	Transparency_interface = glGetUniformLocation(program_id_interface_shader, "Transparency");




}
void OpenGLContainer::UpdateWindowSize()
{
	glfwGetWindowSize(window, &window_x, &window_y);
/*	if (window_x != 1600) {
		Log("Debug", "Window size X not 1600");
	}*/
}
;

