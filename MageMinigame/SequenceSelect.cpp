#include "SequenceSelect.h"

#include "Log.h"

#include "OpenGLContainer.h"
#include "common/shader.hpp"
#include "common/quaternion_utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tinyxml2.h"

#include <algorithm>

using namespace std;
using namespace tinyxml2;

#include "IrrklangSingleton.h"

;
SequenceSelect::SequenceSelect(OpenGLContainer* open_gl_,const std::vector<std::string>& list_of_sequences_, const vector<string>& unlocks)
	:pos(1),sequences(), open_gl(open_gl_)
	,key_esc(GLFW_KEY_ESCAPE), key_l(GLFW_KEY_LEFT), key_r(GLFW_KEY_RIGHT), key_enter(GLFW_KEY_ENTER)
{

	for (auto it = list_of_sequences_.cbegin(); it != list_of_sequences_.cend(); ++it) {
		sequences.push_back(SequenceRepresentation(*it));
	}//Loop over each file

	//TODO:Final Order: Test sequence | Unlocked, incomplete | Complete | Locked, Incomplete 
	for_each(sequences.begin(), sequences.end(), [unlocks](SequenceRepresentation& seq) {return seq.SetUnlockState(unlocks); });

	//auto it1 = partition(sequences.begin(), sequences.end(), [](SequenceRepresentation& seq) {return seq.Unlocked(); });
	auto it1 = stable_partition(sequences.begin(), sequences.end(), mem_fun_ref(&SequenceRepresentation::Unlocked));
	auto it2 = stable_partition(sequences.begin(), it1, not1(mem_fun_ref(&SequenceRepresentation::Completed)));
	//Flip the incomplete ones to the front for easy access. Always keep the testing sequence at the start (just make it impossible to navigate there on release build) 
	//reverse(++sequences.begin(), it2); 

	spinning_symbol = Instance2D("../Data/2D/Ui/SelectionMarker.png", Transform2D(0.0f, 0.0f, 0.85f, 1.0f, true));

	controls_display = Instance2D("../Data/2D/Ui/Controls.png", Transform2D(-0.80f,0.8f,0.2f,0.5f,true));

	RemakeUi();

}



std::string SequenceSelect::Run()
{	
	irrklang::ISound* music = IrrklangSingleton().Get()->play2D("../Data/Sounds/Music/Myst on the Moor.mp3", true, false, true);

	//Loop and make timed calls to: Rendering(up to upper limit set in config), Animation&Input(Lets say 30 a second to make things fluid but the same on all machines), 
	float min_time_between_renders = 1.0f / 60.0f; //60 fps, possibly user setting
	float time_last_render = 0.0f;
	float min_time_game_logic = 1.0f / 30.0f;
	float time_last_game_logic = 0.0f;
	bool done = false;
	//------------------------OpenGL setup-------------------------------------

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//View and Projection matrix are fixed for Minigames
	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); // GetProjectionMatrixForMinigame();

	open_gl->ViewMatrix = GetViewMatrix();

	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way
														  //Light Position
	glm::vec3 lightPos = glm::vec3(5, 0, 200);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);

	while (done == false) {
		float current_time = float(glfwGetTime());

		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			time_last_render = current_time;
			//TODO: actual rendering calls!
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			RenderBackground();

			Render();

			RenderUi();

			glfwSwapBuffers(open_gl->window);
			//cout <<"Rendering at: "<< current_time << endl;
		}

		//Logic
		bool esc_down = false;
		if (current_time - time_last_game_logic > min_time_game_logic) {
			time_last_game_logic = current_time;
			glfwPollEvents();

			//Logic
			esc_down = key_esc.Update(open_gl->window);
			bool l_down = key_l.Update(open_gl->window);
			bool r_down = key_r.Update(open_gl->window);
			bool enter_down = key_enter.Update(open_gl->window);

			if (l_down) {
				if (pos > 0)
				{
					pos--;
					RemakeUi();
				}
			}
			if (r_down)
			{
				if (pos + 1 < sequences.size())
				{
					pos++;
					RemakeUi();
				}
			}
			if (enter_down) {
				IrrklangSingleton().Get()->removeAllSoundSources();
				return sequences[pos].FileName();
			}
		}

		// Check if the ESC key was pressed or the window was closed
		if (esc_down || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			glFlush();
		}
	}

	IrrklangSingleton().Get()->removeAllSoundSources();
	return "";
}




void SequenceSelect::Render()
{
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Set shader to model shader
	glUseProgram(open_gl->program_id_3d);



	//For sequences objects in the front
	glm::vec3 lightPos = glm::vec3(0, -5, 7);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);
	if (sequences[pos].Completed()) {
		glUniform4f(open_gl->LightColourIntensity, 5, 250, 5, 0.4);
	}
	else {
		if (sequences[pos].Unlocked()) {
			glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 0.4);
		} else {
			glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 0.3);
		}
	}


	sequences[pos].Render(open_gl);


}

void SequenceSelect::RenderUi()
{
	//UI:
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(open_gl->program_id_interface_shader);
	//View and Projection matrix are fixed for Minigames

	controls_display.Render(glm::mat4(), open_gl);

	//Actual rendering
	name_display.Render(glm::mat4(),open_gl);

}

void SequenceSelect::RenderBackground()
{
	//static float selection_marker_rot = 0.0f;
	//selection_marker_rot += 0.1f;

	//UI:
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(open_gl->program_id_interface_shader);
	//View and Projection matrix are fixed for Minigames

	//Actual rendering
	//auto rot_mat = glm::rotate(glm::mat4(), selection_marker_rot, glm::vec3(0.0f, 0.0f, 1.0f));

	spinning_symbol.Rotate(-0.06f);
	spinning_symbol.Render(glm::mat4(), open_gl);

}

glm::mat4 SequenceSelect::GetViewMatrix() const {
	float camera_x = 0.0f;
	float camera_y = -15.0f;
	float camera_z = 10.0f;

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
}


void SequenceSelect::RemakeUi()
{
	{

		std::vector<float> b;
		std::vector<std::string> c;
		c.push_back(sequences[pos].DisplayName());
		auto d = sequences[pos].Flavour();
		c.insert(c.end(), d.begin(), d.end());

		std::vector<int> a;
		a.push_back(c.size());//[0] Size Maximum possible size ever is 7


		UiElementParameter new_para(a, b, c);
		name_display.RemakeIfDifferent("../Data/Ui/SequenceSelect_TextBox.xml", new_para);
	}
	//


};




