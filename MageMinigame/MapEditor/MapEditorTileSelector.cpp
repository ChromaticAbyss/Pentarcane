#include "../BuildHeader.h"

#include "MapEditorTileSelector.h"
#include "../OpenGLContainer.h"
#include "../LabyrinthPartDefinition_Manager.h"

#include "../common/shader.hpp"
#include "../common/quaternion_utils.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Log.h"

using namespace std;
using namespace glm;

MapEditorTileSelector::MapEditorTileSelector(OpenGLContainer* open_gl_,LabyrinthPartDefinition_Manager* part_def_manager_)
	:filter_yes(),filter_no(),filter_cand(),filter_ui_yes(),filter_ui_no(),representatives(),part_def_manager(part_def_manager_),open_gl(open_gl_)
{
	Update();
};


void MapEditorTileSelector::Update() {
#ifdef MAGEMINIGAME_MAPEDITOR 
    std::tie(representatives,filter_cand) = part_def_manager->GrabInstancesByFilter(filter_yes,filter_no);

	cout << "Repres:" << representatives.size() << endl;

	int x = -6;
	int y = -3;
	for (auto it = representatives.begin(); it != representatives.end(); ++it) {
		it->SetTransform(Transform(x*3.0f, y*3.0f, 0.0f, 1.0f));

		x++;
		if (x > 4) {
			y++;
			x = -6;
		}
	}

	RemakeUi();
#endif
};


std::string MapEditorTileSelector::Run() {
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
	open_gl->ViewMatrix = GetViewMatrixForEditor();
	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way
														  //Light Position




	//-------------------------------------done-------------------------------------------

	while (done == false) {
		float current_time = float(glfwGetTime());


		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			time_last_render = current_time;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render();

			RenderUi();

			glfwSwapBuffers(open_gl->window);
			//cout <<"Rendering at: "<< current_time << endl;
		}

		//Logic
		if (current_time - time_last_game_logic > min_time_game_logic) {
			time_last_game_logic = current_time;

			glfwPollEvents();
			float mx, my = 0;
			whereDoesMouseRayCutGround(mx, my);
			string ret = Logic(mx, my);
			if (ret != "") {
				done = true;
				return ret;//Something was selected
			}
		}

		// Check if the ESC key was pressed or the window was closed
		if (glfwGetKey(open_gl->window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			glFlush();
		}
	}


	return "";
};



glm::mat4 MapEditorTileSelector::GetViewMatrixForEditor() const {
	float camera_x = 0.0f;
	float camera_y = -5.0f;
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



void MapEditorTileSelector::whereDoesMouseRayCutGround(float& cutX, float& cutY) {
	double screenX = open_gl->window_x;
	double screenY = open_gl->window_y;

	//------------------------------TEEEEST-----------------------------------------------
	double mouseX;
	double mouseY;
	glfwGetCursorPos(open_gl->window, &mouseX, &mouseY);

	mouseY = -mouseY + screenY; //Mirror vertically TODO: Without this the y axis is mirrored, this fix works even tough i have no idea why. Revisit this later!
	mouseX = mouseX;//  *  (screenY/screenX);

					// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenX - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenY - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenX - 0.5f) * 2.0f,
		((float)mouseY / (float)screenY - 0.5f) * 2.0f,
		0.0,
		1.0f
	);

	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	glm::mat4 InverseProjectionMatrix = glm::inverse(glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f));

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	glm::mat4 InverseViewMatrix = glm::inverse(GetViewMatrixForEditor());

	glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
	glm::vec4 lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;

	// Faster way (just one inverse)
	//glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);

	vec3 ray_wor = lRayDir_world;

	//------------------------------TEEEEST-----------------------------------------------
	vec3 cameraPos(0, -5, 40);

	//Intersect world ray with the ground(z=0)plane
	//40 and 20 are the camera z and y positions
	float distZ = (cameraPos.z) / ray_wor.z;

	float resX = cameraPos.x - ray_wor.x*distZ;
	float resY = cameraPos.y - ray_wor.y*distZ;

	cutX = resX;
	cutY = resY;

	return;
}
void MapEditorTileSelector::RemakeUi()
{
	{
		std::vector<int> a;
		a.push_back(filter_cand.size());//int parameter[0]: 
		std::vector<float> b;

		UiElementParameter new_para(a, b, filter_cand);
		filter_ui_yes.RemakeIfDifferent("../Data/Ui/LevelEditor_TileFilter_Yes.xml", new_para);
	}
	//


	{
		std::vector<int> a;
		a.push_back(filter_cand.size());//int parameter[0]: 
		std::vector<float> b;

		UiElementParameter new_para(a, b, filter_cand);
		filter_ui_no.RemakeIfDifferent("../Data/Ui/LevelEditor_TileFilter_No.xml", new_para);
	}
	//

}
;



string MapEditorTileSelector::Logic(float mx, float my) {
	static bool left_mouse_state = false;
	int mouse_mode = 0;

	if (glfwGetMouseButton(open_gl->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		left_mouse_state = true;
	}
	else {
		//Register a click when the mouse is lifted
		if (left_mouse_state) {
			mouse_mode = 1;
		}
		left_mouse_state = false;
	}

	//Click on ui for filters
	double mouseX, mouseY;
	glfwGetCursorPos(open_gl->window, &mouseX, &mouseY);
	float uiMx = (float)mouseX / open_gl->window_x * 2 - 1;
	float uiMy = -((float)mouseY / open_gl->window_y * 2 - 1);
	if (mouse_mode > 0) {
		auto result = filter_ui_yes.CheckInterception(uiMx, uiMy, mouse_mode,open_gl);
		for (auto it = result.begin(); it != result.end(); ++it) {
			mouse_mode = 0;//flush mouse afterwards

			if (it->type == "Filter") {
				filter_yes.push_back(filter_cand[it->value[0]]);
				//TODO: remove from filer_no
				Update();
				return "";//No part selected yet
			}
		}

		auto result_n = filter_ui_no.CheckInterception(uiMx, uiMy, mouse_mode,open_gl);
		for (auto it = result_n.begin(); it != result_n.end(); ++it) {
			mouse_mode = 0;//flush mouse afterwards
			if (it->type == "Filter") {
				filter_no.push_back(filter_cand[it->value[0]]);
				//TODO: remove from filer_yes
				Update();
				return "";//No part selected yet
			}

		}

	}



	//Click on presented object to select it
	if (mouse_mode == 1)
	{
		auto dist = 1.0f;
		for (auto it = representatives.begin(); it != representatives.end(); ++it) {
			if ((it->X() - mx)*(it->X() - mx) + (it->Y() - my)*(it->Y() - my) < dist * dist) {
				return it->TypeName();
			}
		}
	}
	return "";
};

void MapEditorTileSelector::Render() {
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Set shader to model shader
	glUseProgram(open_gl->program_id_3d);

	glm::vec3 lightPos = glm::vec3(5, 0, 50);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 9.0);

	for (vector<LabyrinthPartInstance>::const_iterator it = representatives.cbegin(); it != representatives.cend(); ++it) {
		it->Render(glm::mat4(), open_gl);
	}
}


void MapEditorTileSelector::RenderUi() {
	//UI:
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(open_gl->program_id_interface_shader);
	//View and Projection matrix are fixed for Minigames

	//Actual Rendering
	filter_ui_yes.Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)), open_gl);
	filter_ui_no.Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)), open_gl);

}
