#include "../BuildHeader.h"
#ifdef MAGEMINIGAME_MAPEDITOR

#include "MapEditor.h"
#include "MapEditorTileSelector.h"
#include "../LabyrinthPartDefinition_Manager.h"
#include "../tinyxml2.h"
#include "../OpenGLContainer.h"

#include "../StringUtility.h"
#include "../Log.h"

#include <iostream>

#include "../common\shader.hpp"
#include "../common\quaternion_utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "../Minigames.h"

//#include "TextInput.h"

using namespace glm;
using namespace std;
using namespace tinyxml2;





MapEditor::MapEditor(LabyrinthPartDefinition_Manager* part_def_manager_)
	:tool_select_bar(),tool_option_bar(),selected_tool(0),name_of_current_file(),parts(),variant_carriers(),part_def_manager(part_def_manager_)
	,currently_selected_part_type(),currently_selected_instance(-1),list_of_other_instances_nearby()
	,key_esc(GLFW_KEY_ESCAPE),fast_time(30)
{



	XMLDocument doc_set;
	doc_set.LoadFile("../Data/EditorSettings.xml");
	if (doc_set.ErrorID() == 0) {
		XMLElement* xml_root = doc_set.FirstChildElement("EditorSettings");
		if (xml_root != nullptr) {
			{
				XMLElement* xml_child = xml_root->FirstChildElement("LastFile");
				if (xml_child != nullptr) {
					name_of_current_file = xml_child->GetText();
					//TODO:Load map
					LoadFile(name_of_current_file);

				}
			}



		}
	}

	RemakeUi();

}

MapEditor::~MapEditor()
{

	XMLDocument doc_set;
	doc_set.LoadFile("../Data/EditorSettings.xml");
	if (doc_set.ErrorID() == 0) {
		XMLElement* xml_root = doc_set.FirstChildElement("EditorSettings");
		if (xml_root != nullptr) {
			{
				XMLElement* xml_child = xml_root->FirstChildElement("LastFile");
				if (xml_child != nullptr) {
					xml_child->SetText(name_of_current_file.c_str());
		
				}
			}


		}
		doc_set.SaveFile("../Data/EditorSettings.xml");
	}

}



void MapEditor::InitializeFromXml(tinyxml2::XMLElement * xml_root) {
	//Must be strictly additive to allow recursion!

	{ //Fast time
		XMLElement * xml_child = xml_root->FirstChildElement("FastTime");
		while (xml_child != 0) {
			fast_time =  stoi(xml_child->GetText()); //30 frames per second
			xml_child = xml_child->NextSiblingElement("FastTime");
		}//while children are found
	}


	{ //Look up parts
		XMLElement * xml_child = xml_root->FirstChildElement("Part");
		while (xml_child != 0) {

			parts.push_back(part_def_manager->GetInstanceFromXml(xml_child));

			xml_child = xml_child->NextSiblingElement("Part");
		}//while children are found
	}//look up parts


	{ //Variants, map editor loads ALL of them into a specialised data structure
		XMLElement * xml_child = xml_root->FirstChildElement("Variant");
		while (xml_child != 0) {

			variant_carriers.push_back(VariantCarrier(xml_child));

			xml_child = xml_child->NextSiblingElement("Variant");
		}
	}


};

void MapEditor::Run(OpenGLContainer* open_gl) {
	//glfwSetKeyCallback(open_gl->window, TextInputForMapEditor::key_callback);

	//Loop and make timed calls to: Rendering(up to upper limit set in config), Animation&Input(Lets say 30 a second to make things fluid but the same on all machines), 
	float min_time_between_renders = 1.0f / 60.0f; //60 fps, possibly user setting
	float time_last_render = 0.0f;
	float min_time_game_logic = 1.0f / 30.0f;
	float time_last_game_logic = 0.0f;
	bool done=false;

	//------------------------OpenGL setup-------------------------------------
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Set shader to model shader
	//GLuint program_id = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	//glUseProgram(program_id);
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//View and Projection matrix are fixed for Minigames
	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); // GetProjectionMatrixForMinigame();
	open_gl->ViewMatrix = GetViewMatrixForEditor();
	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way
														  //Light Position


	while (done == false) {
		float current_time = float(glfwGetTime());
		

		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			time_last_render = current_time;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Render(open_gl);
			glfwSwapBuffers(open_gl->window);
			//cout <<"Rendering at: "<< current_time << endl;
		}

		//Logic
		if (current_time - time_last_game_logic > min_time_game_logic) {
			time_last_game_logic = current_time;
			glfwPollEvents();
			float mx, my = 0;
			whereDoesMouseRayCutGround(open_gl,mx,my);
			Logic(open_gl,mx,my);
		}


		bool esc_down = key_esc.Update(open_gl->window);

		// Check if the ESC key was pressed or the window was closed
		if (esc_down || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			//Ask for confirmation, save map etc
		}
	}

	//glfwSetKeyCallback(open_gl->window, NULL);//Delete our input function
	Save("../Data/Labyrinths/MapEditorAbortOutput.xml");
}



void MapEditor::Render(OpenGLContainer* open_gl) {
	//Set up opengl for 3 dimensional rendering
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(open_gl->program_id_3d);
	//View and Projection matrix are fixed for Minigames
	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); // GetProjectionMatrixForMinigame();
	open_gl->ViewMatrix = GetViewMatrixForEditor();
	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way	 //Light Position
	glm::vec3 lightPos = glm::vec3(5, 0, 50);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);
	glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 9.0);


	//---------------------------------------- Actual rendering
	int nr = -1;
	for (vector<LabyrinthPartInstance>::const_iterator it = parts.cbegin(); it != parts.cend(); ++it) {
		nr++;
		if (currently_selected_instance == nr) {
			glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 50.0);//Massively brighten the selected object
		}

		it->Render(glm::mat4(), open_gl);

		if (currently_selected_instance == nr) {
			glUniform4f(open_gl->LightColourIntensity, 250, 250, 250, 9.0);//Restore old lighting for the rest of the loop
		}
	}


	//UI:
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(open_gl->program_id_interface_shader);
	//View and Projection matrix are fixed for Minigames
	
	//Actual Rendering
	tool_select_bar.Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)),open_gl);
	tool_option_bar.Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)), open_gl);
}


glm::mat4 MapEditor::GetViewMatrixForEditor() const {
	float camera_x = 0.0f;
	float camera_y = -20.0f; //Better: -20
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



void MapEditor::whereDoesMouseRayCutGround(OpenGLContainer* open_gl,float& cutX, float& cutY) {
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
	vec3 cameraPos(0, -20, 40);

	//Intersect world ray with the ground(z=0)plane
	//40 and 20 are the camera z and y positions
	float distZ = (cameraPos.z) / ray_wor.z;

	float resX = cameraPos.x - ray_wor.x*distZ;
	float resY = cameraPos.y - ray_wor.y*distZ;

	cutX = resX;
	cutY = resY;

	return;
};


void MapEditor::Logic(OpenGLContainer* open_gl,float mx, float my) {
	static bool left_mouse_state = false;
	static bool right_mouse_state = false;
	static bool nullify_until_released = false;

	int mouse_mode = 0;
	if (glfwGetMouseButton(open_gl->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		mouse_mode = 3; //3 is for: Left mouse down currently
		left_mouse_state = true;
	}
	else {
		//Register a click when the mouse is lifted
		if (left_mouse_state && nullify_until_released==false) {
			mouse_mode = 1;
		}
		nullify_until_released = false;
		left_mouse_state = false;
	}
	if (glfwGetMouseButton(open_gl->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		right_mouse_state = true;
	}
	else {
		//Register a click when the mouse is lifted
		if (right_mouse_state) {
			mouse_mode = 2;
		}
		right_mouse_state = false;
	}
	if (nullify_until_released) {
		mouse_mode = 0;
	}



	// Enter for saving the map
	static bool already_saved = false;
	if (glfwGetKey(open_gl->window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (already_saved == false) {
			SaveDialogue();
			already_saved = true;
			mouse_mode = 0;//flush mouse afterwards
			nullify_until_released = true; //mouse down/click doesn't count until next press&release!
		}		
	}else {
		already_saved = false;
	}



	//Intercept Ui
	double mouseX, mouseY;
	glfwGetCursorPos(open_gl->window, &mouseX, &mouseY);
	float uiMx = (float)mouseX / open_gl->window_x * 2 - 1;
	float uiMy = -((float)mouseY / open_gl->window_y * 2 - 1);

	if (mouse_mode > 0) {
		auto result = tool_select_bar.CheckInterception(uiMx, uiMy, mouse_mode, open_gl);
		for (auto it = result.begin(); it != result.end(); ++it) {
			mouse_mode = 0;//flush mouse afterwards
			nullify_until_released = true; //mouse down/click doesn't count until next press&release!

			if (it->type == "SelectTool") {
				selected_tool = it->value[0];
				if (selected_tool == 1) {
					currently_selected_instance = -1; //When entering selection mode, deselect a potentially selected object
				}
				RemakeUi();
				return;
			}
			if (it->type == "Save") {
				SaveDialogue();
				return;
			}
			if (it->type == "SaveAndTest") {
				TestMap(open_gl);
				return;
			}
			if (it->type == "LoadNew") {
				string file_to_load;
				cout << "File to load: ";
				std::cin >> file_to_load;
				if (file_to_load == "") {
				return;
				}
				file_to_load = "../Data/Labyrinths/" + file_to_load + ".xml";

				LoadFile(file_to_load);

				return;
			}


		}

	}



	if (selected_tool == 0) {

		if (mouse_mode > 0) {
			auto result = tool_option_bar.CheckInterception(uiMx, uiMy, mouse_mode, open_gl);
			for (auto it = result.begin(); it != result.end(); ++it) {
				mouse_mode = 0;//flush mouse afterwards
				nullify_until_released = true; //mouse down/click doesn't count until next press&release!

				if (it->type == "SelectToDraw") {
					MapEditorTileSelector temp(open_gl,part_def_manager);
					string res = temp.Run();
					if (res != "") {
						currently_selected_part_type = res;
					}
					key_esc.BlockUntilReleased();

					return;//Doing anything else this turn might be pointless
				}//SetUniqueName
			}
		}

		bool result = Tool_Draw(open_gl, mx, my, mouse_mode);

		nullify_until_released = result; //mouse down/click doesn't count until next press&release!
		if (result) {
			mouse_mode = 0;
		}

		return;
	}



	if (selected_tool == 1) {
		//As usual poll the UI first!
		if (mouse_mode == 1) {
			auto result = tool_option_bar.CheckInterception(uiMx, uiMy, mouse_mode, open_gl);
			for (auto it = result.begin(); it != result.end(); ++it) {
				mouse_mode = 0;//flush mouse afterwards
				nullify_until_released = true; //mouse down/click doesn't count until next press&release!

				if (it->type == "Delete") {
					parts[currently_selected_instance].Hide();
					currently_selected_instance = -1;
					RemakeUi();
					return;//Doing anything else this turn might be pointless
				}//Delete

				if (it->type == "SelectionStackFlip") {
					if (list_of_other_instances_nearby.size() > 0) {
						int id = list_of_other_instances_nearby.back();
						list_of_other_instances_nearby.pop_back();
						currently_selected_instance = id;
						cout << "Now selected: ID " << id << " type: " << parts[id].TypeName() << " UniqueName: " << parts[id].UniqueName() << endl;
						RemakeUi();
					} else {
						currently_selected_instance = -1; //Allows selecting something new
						RemakeUi();
					}
					return;
				}//Stack flip
				if (it->type == "SetUniqueName") {
					cout << "Enter new unqiue name:";
					string new_name;
					cin >> new_name;
					parts[currently_selected_instance].SetUniqueName(new_name);
					return;
				}//SetUniqueName
				if (it->type == "SetRandomName") {
					parts[currently_selected_instance].SetUniqueNameRandom();
					cout << "New name: " << parts[currently_selected_instance].UniqueName();
					return;
				}
				if (it->type == "ConnectTo") {
					selected_tool = 2; //Connector tool
					RemakeUi();
					return;
				}
				if (it->type == "Rotate") {
					float angle = it->valueF[0];
					Transform transf = parts[currently_selected_instance].transform;
					transf.RotateZ(angle);
					parts[currently_selected_instance].SetTransform(transf);					
					return;
				}
				if (it->type == "MoveXY") {
					int x = it->value[0];
					int y = it->value[1];
					parts[currently_selected_instance].Move(x,y);
					return;
				}

				if (it->type == "Clone") {
					
					//Create new instance
					int old_instance = currently_selected_instance;
					parts.emplace_back(parts[currently_selected_instance]);
					currently_selected_instance = parts.size() - 1;
					string old_name = parts[old_instance].UniqueName();
					if (old_name != "") {
						parts[currently_selected_instance].SetUniqueName(old_name + "(Clone)");
						Log("Clone:" + parts[currently_selected_instance].UniqueName());
					}

					return;
				}


				if (it->type == "SetFloatParameter") {
					cout << "Parameter nr: to set:" << endl;
					int nr;
					cin >> nr;
					cout << "Set to:" << endl;
					float v;
					cin >> v;
					parts[currently_selected_instance].SetScriptParameterFloat(nr,v);
				}


			}//loop over results
		}
		if (mouse_mode == 2) { //Right click brings back to selector with nothing selected
			selected_tool = 1;
			currently_selected_instance = -1;
			RemakeUi();
			return;
		}

		bool result = Tool_Select(open_gl, mx, my, mouse_mode);

		nullify_until_released = result; //mouse down/click doesn't count until next press&release!
		if (result) {
			mouse_mode = 0;
		}

		return;
	}//tool 1: Selection



	if (selected_tool == 2) {
		if (mouse_mode == 2) { //Right click brings back to selector with nothing selected
			selected_tool = 1;
			currently_selected_instance = -1;
			RemakeUi();
			return;
		}
		if (mouse_mode > 0) {
			auto result = tool_option_bar.CheckInterception(uiMx, uiMy, mouse_mode, open_gl);
			for (auto it = result.begin(); it != result.end(); ++it) {
				mouse_mode = 0;//flush mouse afterwards
				nullify_until_released = true; //mouse down/click doesn't count until next press&release!
				if (it->type == "FlushAllConnections") {
					parts[currently_selected_instance].script_parameters_string.clear();
					cout << "All connections deleted" << endl;
				}//FlushAllConnections
				if (it->type == "FinishAndDeselect") {
					selected_tool = 1;//Back to selector
					currently_selected_instance = -1;
					RemakeUi();
				}//FlushAllConnections

			}//All results
		}

		bool result = Tool_Connect(open_gl, mx, my, mouse_mode);

		nullify_until_released = result; //mouse down/click doesn't count until next press&release!
		if (result) {
			mouse_mode = 0;
		}

		return;
	}//tool 2: Connection



	if (selected_tool == 4) {

		if (mouse_mode > 0) {
			auto result = tool_option_bar.CheckInterception(uiMx, uiMy, mouse_mode, open_gl);
			for (auto it = result.begin(); it != result.end(); ++it) {
				mouse_mode = 0;//flush mouse afterwards
				nullify_until_released = true; //mouse down/click doesn't count until next press&release!
				if (it->type == "MoveAllXY") {
					int x = it->value[0];
					int y = it->value[1];
					for_each(parts.begin(), parts.end(), [x,y](LabyrinthPartInstance& p) {
						p.Move(x, y);
					});
					return;
				}
				if (it->type == "SetTimer") {
					cout << "Timer for labyrinth (seconds, current is "+intToString(fast_time)+")";
					cin >> fast_time;
				}

			}//All results
		}



		return;
	}//tool 2: Connection

}
void MapEditor::SaveDialogue()
{

		if (name_of_current_file != "") {
			Save(name_of_current_file);//name must be legal or otherwise it gets turned into ""!
		}
		else {
			cout << "Enter a name for this new Labyrinth: " << endl;
			string new_name;
			cin >> new_name;
			new_name = "../Data/Labyrinths/" + new_name + ".xml";
			name_of_current_file = new_name;
			Save(name_of_current_file);
		}

}
;


bool MapEditor::Tool_Draw(OpenGLContainer* open_gl,float mx, float my, int mouse_mode) {
	mx = floor(mx + 0.5f);
	my = floor(my + 0.5f);


	if (mouse_mode == 1) {
		if (currently_selected_part_type != "") {
			LabyrinthPartInstance temp = part_def_manager->GetInstance(currently_selected_part_type);
			temp.Move(mx, my);

			parts.push_back(temp);
		}
	}
	if (mouse_mode == 2) {
		for (auto it = parts.begin(); it != parts.end(); ++it) {
			float range = 0.3f;
			float dist_squared = (mx - it->X())*(mx - it->X()) + (my - it->Y())*(my - it->Y());
			if (dist_squared < range*range) {
				it->Hide();//Map editor will not write hidden objects
			}

		}
	}

	return false;
}


bool MapEditor::Tool_Select(OpenGLContainer* open_gl, float mx, float my, int mouse_mode) {
	//currently_selected_instance;
	mx = floor(mx + 0.5f);
	my = floor(my + 0.5f);

	//Grab the first thing in Range as the selected instance and add all others to a stack to be flipped through
	if (mouse_mode == 1) {
		if (currently_selected_instance == -1) {
			bool one_selection = false;
			list_of_other_instances_nearby.clear();
			int id = -1;
			for (auto it = parts.begin(); it != parts.end(); ++it) {
				id++;
				if(!(it->Disabled() || it->Hidden())){	
					float range = 0.3f;
					float dist_squared = (mx - it->X())*(mx - it->X()) + (my - it->Y())*(my - it->Y());
					if (dist_squared < range*range) {
						if (one_selection == false) {
							one_selection = true;
							currently_selected_instance = id;
							RemakeUi();
							if (parts[id].UniqueName() != "") {
								cout << "Sel " << parts[id].UniqueName()<<endl;
							}
							
						}
						else {
							list_of_other_instances_nearby.push_back(id);
						}
					}
				}
			}
		}//Nothing selected at the moment
	}//left click

	return false;
}


bool MapEditor::Tool_Connect(OpenGLContainer* open_gl, float mx, float my, int mouse_mode) {

	//currently_selected_instance;
	mx = floor(mx + 0.5f);
	my = floor(my + 0.5f);

	if (currently_selected_instance < 0) { //If nothing is selected we have nothing to do here
		selected_tool = 1; //back to selection mode
		return false;
	}

	//Grab everything that is clicked on and append it to the current selections 
	if (mouse_mode == 1) {
		if (currently_selected_instance > -1) {
			for (auto it = parts.begin(); it != parts.end(); ++it) {
				float range = 0.3f;
				float dist_squared = (mx - it->X())*(mx - it->X()) + (my - it->Y())*(my - it->Y());
				if (dist_squared < range*range) {
					string u_n = it->UniqueName();
					if (u_n != "") {
						cout << "Connected: " << u_n << endl;
						parts[currently_selected_instance].script_parameters_string.push_back(u_n);
					}//there is a unique name
				}

			}
		}//Nothing selected at the moment
	}//left click

	return false;
}

bool MapEditor::Tool_GlobalTool(OpenGLContainer* open_gl, float mx, float my, int mouse_mode)
{



	return false;
}

bool MapEditor::LoadFile(std::string file_to_load)
{
	//Clean up everything still on the screen
	parts.clear();
	variant_carriers.clear();
	currently_selected_instance = -1;
	currently_selected_part_type = "";
	list_of_other_instances_nearby.clear();
	fast_time = 30;
	selected_tool = 0;

	//Load the new file
	XMLDocument doc;
	doc.LoadFile(file_to_load.c_str());
	if (doc.ErrorID() == 0) {
		name_of_current_file = file_to_load;

		XMLElement* xml_root = doc.FirstChildElement("Labyrinth");
		if (xml_root != nullptr) {

			InitializeFromXml(xml_root);

		}//xml_root found
	}
	else {
		Log("Error", "Map Editor could not find file to load, making new empty file");
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_to_load << endl;
		name_of_current_file = "";
	}

	RemakeUi();


	return false;
}

void MapEditor::TestMap(OpenGLContainer* open_gl)
{
	if (name_of_current_file != "") {
		Save(name_of_current_file);

		std::vector<std::string> laby_names_2;
		laby_names_2.push_back(name_of_current_file);
		vector<int> monster_ids;
		monster_ids.push_back(1);
		Minigames min(laby_names_2, 1, monster_ids, part_def_manager);
		min.Resolve(open_gl);
	} else {
		Log("Debug", "Save and Name the map before testing it.");
	}
}




void MapEditor::Save(string file_name) {
	cout << "Saved as:" << file_name << endl;
	XMLDocument doc;
	//doc.NewDeclaration();

	XMLNode* root = doc.NewElement("Labyrinth");
	doc.InsertFirstChild(root);

	/* Test
	XMLElement * pElement = doc.NewElement("IntValue");
	pElement->SetText(10);
	root->InsertEndChild(pElement);
	*/

	//Fast time
	XMLElement * p_part = doc.NewElement("FastTime");
	p_part->SetText(fast_time);
	root->InsertEndChild(p_part);


	for (auto it = parts.cbegin(); it != parts.cend(); ++it) {
		if (it->Hidden() == false) {
			XMLElement * p_part = it->MakeXML(doc);
			root->InsertEndChild(p_part);
		}
	}


	for (auto it = variant_carriers.cbegin(); it != variant_carriers.cend(); ++it) {
		XMLElement * p_part = it->MakeXML(doc);
		root->InsertEndChild(p_part);
	}


	doc.SaveFile(file_name.c_str());

};


void MapEditor::RemakeUi() {
	{
		std::vector<int> a;
		a.push_back(selected_tool);//int parameter[0]: Which spell is hovered
		std::vector<float> b;
		std::vector<string> c;

		UiElementParameter new_para(a, b, c);
		tool_select_bar.RemakeIfDifferent("../Data/Ui/LevelEditor_ToolSelector.xml", new_para);
	}
	//


	if (selected_tool == 0) {
		//Default look: Nothing selected
		std::vector<int> a;
		std::vector<float> b;
		std::vector<string> c;
		UiElementParameter new_para_options(a, b, c);
		tool_option_bar.RemakeIfDifferent("../Data/Ui/LevelEditor_ToolOptions_Draw.xml", new_para_options);
	}

	if (selected_tool == 1) {
		//Default look: Nothing selected
		std::vector<int> a;
		a.push_back(currently_selected_instance);//int parameter[0]: Which object is selected
		std::vector<float> b;
		std::vector<string> c;
		if (currently_selected_instance > -1) {
			c.push_back(parts[currently_selected_instance].TypeName());
			c.push_back(parts[currently_selected_instance].UniqueName());
		} else {
			c.push_back("");
			c.push_back("");
		}
		UiElementParameter	new_para_options(a, b, c);
		tool_option_bar.RemakeIfDifferent("../Data/Ui/LevelEditor_ToolOptions_Select.xml", new_para_options);

	}


	if (selected_tool == 2) {
		//Default look: Nothing selected
		std::vector<int> a;
		a.push_back(currently_selected_instance);//int parameter[0]: Which object is selected
		std::vector<float> b;
		std::vector<string> c;

		UiElementParameter	new_para_options(a, b, c);
		tool_option_bar.RemakeIfDifferent("../Data/Ui/LevelEditor_ToolOptions_Connect.xml", new_para_options);

	}




	if (selected_tool == 4) {
		//Default look: Nothing selected
		std::vector<int> a;
		std::vector<float> b;
		std::vector<string> c;

		UiElementParameter	new_para_options(a, b, c);
		tool_option_bar.RemakeIfDifferent("../Data/Ui/LevelEditor_ToolOptions_Global.xml", new_para_options);

	}

};

#endif