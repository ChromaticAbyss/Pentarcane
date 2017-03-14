#include "Minigames.h"

#include "LabyrinthPartDefinition_Manager.h"
#include "LabyrinthInstance.h"


#include "OpenGLContainer.h"

#include "common\shader.hpp"
#include "common\quaternion_utils.hpp"
#include "Log.h"
#include <math.h>

#include <iostream>

using namespace std;

Minigames::Minigames(std::vector<std::string> labyrinth_names, int spell, std::vector<int> monster_ids, LabyrinthPartDefinition_Manager* part_def_manager) 
	:done(false),labyrinths()
{
	int size = labyrinth_names.size();

	for (int i = 0; i < size; i++) {
		labyrinths.push_back(LabyrinthInstance(labyrinth_names[i], spell, monster_ids[i], part_def_manager));
	}

};


std::vector<LabyrinthResult> Minigames::Resolve(OpenGLContainer* open_gl) {

	//Loop and make timed calls to: Rendering(up to upper limit set in config), Animation&Input(Lets say 30 a second to make things fluid but the same on all machines), 
	float min_time_between_renders = 1.0f / 60.0f; //60 fps, possibly user setting
	float time_last_render = float(glfwGetTime());
	float min_time_game_logic = 1.0f / 30.0f;
	float time_last_game_logic = float(glfwGetTime())-0.5f / 60.0f;


	//------------------------OpenGL setup-------------------------------------
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Set shader to model shader
	glUseProgram(open_gl->program_id_3d);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//View and Projection matrix are fixed for Minigames
	glm::mat4 ProjectionMatrix = GetProjectionMatrixForMinigame();
	open_gl->ViewMatrix = GetViewMatrixForMinigame();
	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way
	//Light Position
	glm::vec3 lightPos = glm::vec3(5, 0, 200);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);


	//-------------------------------------done-------------------------------------------
	while (done == false) {
		float current_time = float(glfwGetTime());

		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			time_last_render = current_time;

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_SCISSOR_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(open_gl->program_id_3d);
			//TODO: Frail construction!
			if (labyrinth_results.size() > 0) {
				int size = labyrinths.size();
				for (int i = 0; i < size; i++) {
					if (labyrinth_results[i].state == LabyrinthResult::state_keys::running) { //Only render running labyrinths
						auto port = ViewportPosSize(i, size, open_gl);
						glViewport(port[0], port[1], port[2], port[3]);
						glScissor(port[0], port[1], port[2], port[3]);

						labyrinths[i].Render(Transform(), open_gl);
					}
				}
			}


			//Set up open_gl for interface rendering
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			//glDisable(GL_BLEND);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(open_gl->program_id_interface_shader);
			if (labyrinth_results.size() > 0) {
				int size = labyrinths.size();
				for (int i = 0; i < size; i++) {
					if (labyrinth_results[i].state == LabyrinthResult::state_keys::running) { //Only render running labyrinths
						auto port = ViewportPosSize(i, size, open_gl);
						glViewport(port[0], port[1], port[2], port[3]);
						glScissor(port[0], port[1], port[2], port[3]);

						labyrinths[i].Render2D(open_gl);
					}
				}
			}


			glDisable(GL_SCISSOR_TEST);
			glViewport(0, 0, open_gl->window_x, open_gl->window_y);

			glfwSwapBuffers(open_gl->window);
			//cout << "Total render time: " << (float(glfwGetTime()) - measure_full_render_time) << endl;
		}

		//Logic
		if (current_time - time_last_game_logic > min_time_game_logic) {
			time_last_game_logic = current_time;

			//float measure_full_logic_time = current_time;

			//TODO: actual game logic calls such as -input polling, (enemy) motion and AI, animations
			glfwPollEvents();

			vector<float> inputs;
			inputs.push_back(0.0f);
			inputs.push_back(0.0f);
			//Input device = keyboard
			if (glfwGetKey(open_gl->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				inputs[0] += 1.0f;
			}
			if (glfwGetKey(open_gl->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				inputs[0] -= 1.0f;
			}
			if (glfwGetKey(open_gl->window, GLFW_KEY_UP) == GLFW_PRESS) {
				inputs[1] += 1.0f;
			}
			if (glfwGetKey(open_gl->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				inputs[1] -= 1.0f;
			}


			labyrinth_results.clear();
			for (unsigned int i = 0; i < labyrinths.size(); i++) {
				//
				labyrinth_results.push_back(labyrinths[i].LogicStep(inputs));
			}

			bool all_labyrinths_done = true;
			for (unsigned int ii = 0; ii < labyrinth_results.size(); ii++) {
				if (labyrinth_results[ii].state == LabyrinthResult::state_keys::running) {
					all_labyrinths_done = false;
				}
			}
			if (all_labyrinths_done) {
				return labyrinth_results; //The vector of results from the minigames
			}
			//TODO: Analyze results

			//cout << "Total logic time: " << (float(glfwGetTime()) - measure_full_logic_time) << endl;
		}

		// Check if the ESC key was pressed or the window was closed
		if (glfwGetKey(open_gl->window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			//Do some other stuff with the labrinths that are still running(fail them!)
		}
	}

	return labyrinth_results;//Or whatever the results are
}

std::vector<float> Minigames::ViewportPosSize(int nr, int nr_max, OpenGLContainer* open_gl)
{
	std::vector<float> return_value{ 0,0,0,0 };

	//X
	return_value[0] = nr*open_gl->window_x / nr_max;



	//SizeX
	return_value[2] = open_gl->window_x / nr_max;

	//SizeY
	return_value[3] = min(open_gl->window_y, open_gl->window_x / nr_max);

	//Y
	return_value[1] = open_gl->window_y - return_value[3];


	return return_value;
}





glm::mat4 Minigames::GetViewMatrixForMinigame() const {
	float camera_x = 0.0f;
	float camera_y = -20.0f;
	float camera_z = 40.0f;

	glm::vec3 position(camera_x, camera_y, camera_z);

	glm::vec3 origin( //Our view target
		0.0f,
		0.0f,
		0.0f
	);
	//std::cout<<"View target: "<<origin.x<<","<<origin.y<<","<<origin.z<<std::endl;

	glm::vec3 direction = -position;
	direction = glm::normalize(direction);


	glm::vec3 right = glm::vec3(
		1,
		0,
		0
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);
	//std::cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@UP: x"<<up.x<<" y:"<<up.y<<" z:"<<up.z<<std::endl;

	//std::cout<<"Scalar produkt:"<<(up.x*direction.x+up.y*direction.y+up.z*direction.z)<<std::endl;


	return glm::lookAt(
		position,           // Camera is here
		origin, // and to the origin
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
};

glm::mat4 Minigames::GetProjectionMatrixForMinigame() const {
	return glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
};