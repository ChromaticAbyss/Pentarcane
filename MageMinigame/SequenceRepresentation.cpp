#include "SequenceRepresentation.h"

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


SequenceRepresentation::SequenceRepresentation(const std::string& file_name)
	:internal_name(file_name), display_name(), description(), unlock_requirements(), complete_requirement(), unlock_state(0), rotation(0.0f)
{

	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {

		XMLElement* xml_root = doc.FirstChildElement("Sequence");

		if (xml_root == nullptr) {
			Log("Error", "SequenceRepresentation xml_root is null");
			return;
		}

		{ //Look up internal name
			XMLElement * xml_child = xml_root->FirstChildElement("InternalName");
			if (xml_child != nullptr) {
				internal_name = xml_child->GetText();
			}
		}

		Log("Data", "Making LabyrinthPartDefinition with internal_name " + internal_name);


		{
			XMLElement * xml_child = xml_root->FirstChildElement("DisplayName");
			if (xml_child != 0) {
				display_name = xml_child->GetText();
			}
		}

		{
			XMLElement * xml_child = xml_root->FirstChildElement("Description");
			while (xml_child != nullptr) {
				description.push_back(xml_child->GetText());
				xml_child = xml_child->NextSiblingElement("Description");
			}
		}


		{
			XMLElement * xml_child = xml_root->FirstChildElement("UnlockRequirement");
			while (xml_child != nullptr) {
				unlock_requirements.push_back(xml_child->GetText());
				xml_child = xml_child->NextSiblingElement("UnlockRequirement");
			}
		}


		{
			XMLElement * xml_child = xml_root->FirstChildElement("Reward");
			while (xml_child != nullptr) {
				rewards.push_back(xml_child->GetText());
				xml_child = xml_child->NextSiblingElement("Reward");
			}
		}


		{
			XMLElement * xml_child = xml_root->FirstChildElement("CompleteRequirement");
			if (xml_child != nullptr) {
				complete_requirement = xml_child->GetText();
			}
		}


		{ //Look up a model in the node
			XMLElement * xml_child = xml_root->FirstChildElement("Art3D");
			while (xml_child != nullptr) {

				string model_name = "../Data/3D/MissingModelMarker.obj";
				auto xml_child_m = xml_child->FirstChildElement("ModelFile");
				if (xml_child_m != nullptr) {
					model_name = xml_child_m->GetText();
				}

				string texture_name = "../Data/2D/MissingTexture.png";
				auto xml_child_tex = xml_child->FirstChildElement("TextureFile");
				if (xml_child_tex != nullptr) {
					texture_name = xml_child_tex->GetText();
				}

				Transform tra;
				auto xml_child_transf = xml_child->FirstChildElement("Transform");
				if (xml_child_transf != nullptr) {
					tra = Transform(xml_child_transf);
				}


				models.push_back(Instance3D_New::MakeRogueModel(tra,model_name,texture_name));

				xml_child = xml_child->NextSiblingElement("Art3D");
			}
		}

	}
	else
	{
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
		display_name = "XML-File not found";
	}

}


std::vector<std::string> SequenceRepresentation::Flavour() const
{
	vector<string> ret_val;
	if (!Unlocked()) {
		ret_val.push_back("(locked)");
		return ret_val;
	}

	if (Completed())
	{
		ret_val.push_back("(completed)");
	}

	//Grab the first 6 lines of description
	for (int i = 0; i < min(int(description.size()), 6); i++) {
		ret_val.push_back(description[i]);
	}

	return ret_val;
}


void SequenceRepresentation::Render(OpenGLContainer * open_gl)
{
	auto m_s = models.size();
	if (m_s <= 0) { return; }

	if (Unlocked() || m_s == 1) {
		rotation += 0.5f;
		auto rot_mat = glm::rotate(glm::mat4(), rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		models[0].Render(rot_mat, open_gl);
		return;
	}

	if (m_s > 1 || !Unlocked()) {
		models[1].Render(glm::mat4(), open_gl);
		return;
	}



}


void SequenceRepresentation::SetUnlockState(const std::vector<std::string>& names)
{
	//TODO: Check completed requirements in the same way
	for (unsigned int j = 0; j < names.size(); j++) {
		if (names[j] == complete_requirement) {
			unlock_state = 2; //Completed
			return;
		}
	}

	for (unsigned int i = 0; i < unlock_requirements.size(); i++) {
		if (!Fulfilled(i, names)) {
			return; //return without unlocking
		}
	}

	unlock_state = 1;
	return;
}

bool SequenceRepresentation::Fulfilled(int i, const std::vector<std::string>& names) const
{
	for (unsigned int j = 0; j < names.size(); j++) {
		if (names[j] == unlock_requirements[i]) {
			return true;
		}
	}

	return false;
}



std::vector<std::string> SequenceRepresentation::GetRewards()
{
	return rewards;
}

void SequenceRepresentation::CompleteScreen(OpenGLContainer * open_gl)
{
	Log("Debug", "CompleteScreen");
	Key key_esc(GLFW_KEY_ESCAPE);
	Key key_enter(GLFW_KEY_ENTER);

	std::vector<float> b;
	std::vector<std::string> c;
	c.push_back("Sequence Completed!");
	c.push_back(display_name);
	c.push_back("Rewards:");
	c.insert(c.end(), rewards.begin(), rewards.end());

	//for (int i = 0; i < rewards.size(); i++) {
	//}

	std::vector<int> a;
	a.push_back(c.size());//[0] Size Maximum possible size ever is 7


	UiElementParameter new_para(a, b, c);
	UiElement display("../Data/Ui/SequenceFinish.xml", new_para);
		

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//View and Projection matrix are fixed for Minigames
	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f); // GetProjectionMatrixForMinigame();

	//open_gl->ViewMatrix = GetViewMatrix();

	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way
														  //Light Position
	glm::vec3 lightPos = glm::vec3(5, 0, 200);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);


	//Render:
	float min_time_between_renders = 1.0f / 60.0f; //60 fps, possibly user setting
	float time_last_render = 0.0f;
	bool done = false;
	while (done == false) {
		float current_time = float(glfwGetTime());
		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			glfwPollEvents();
			time_last_render = current_time;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//Rendering  calls go here


			//UI:
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(open_gl->program_id_interface_shader);
			//View and Projection matrix are fixed for Minigames
			//Actual rendering
			display.Render(glm::mat4(), open_gl);



			glfwSwapBuffers(open_gl->window);
		}

		
		bool close_key = false;
		close_key = close_key || key_esc.Update(open_gl->window);
		close_key = close_key || key_enter.Update(open_gl->window);


		// Check if the ESC key was pressed or the window was closed
		if (close_key || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			glFlush();
			return;
		}
	}


}
