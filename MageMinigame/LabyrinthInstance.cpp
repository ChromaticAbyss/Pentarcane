
#include "LabyrinthInstance.h"
#include "LabyrinthPartDefinition_Manager.h"
#include "tinyxml2.h"
#include "OpenGLContainer.h"

#include "StringUtility.h"
#include "Log.h"

#include "ScriptBridge.h"
#include "HashingForTags.h"

#include <algorithm>

#include "Ui/TimerManager.h"

using namespace std;
using namespace tinyxml2;

LabyrinthInstance::LabyrinthInstance(std::string file_name, int spell_, int monster_id_, LabyrinthPartDefinition_Manager* part_def_manager_) 
	:spell(spell_),monster_id(monster_id_),bonus(),goal(0),key(0),inert_parts(),spawn_list(),messages()
	,part_def_manager(part_def_manager_)
    ,calls_with_no_player_objects(0),timer_manager(make_shared<TimerManager>())
	,frame(0),finish_frame(-1),fast_frame(300)
{
	

	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {

		XMLElement* xml_root = doc.FirstChildElement("Labyrinth");
		if (xml_root != 0) {
			InitializeFromXml(xml_root);

			//Test
			timer_manager->AddBar(1, fast_frame, "../Data/Ui/TimerBar_Test.xml"); //id, duration, file path
		}//xml_root found

		//Log("Debug", "Newly created LabyrinthInstance has this many stationary objects: " + intToString(inert_parts.size()));

	} else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}
};


void LabyrinthInstance::InitializeFromXml(tinyxml2::XMLElement * xml_root) {
	//Must be strictly additive to allow recursion!

	{ //Fast time
		XMLElement * xml_child = xml_root->FirstChildElement("FastTime");
		while (xml_child != 0) {
			fast_frame = 30 * stoi(xml_child->GetText()); //30 frames per second

			xml_child = xml_child->NextSiblingElement("FastTime");
		}//while children are found
	}


	{ //Look up parts
		XMLElement * xml_child = xml_root->FirstChildElement("Part");
		while (xml_child != 0) {

			inert_parts.push_back(part_def_manager->GetInstanceFromXml(xml_child));

			xml_child = xml_child->NextSiblingElement("Part");
		}//while children are found
	}//look up parts


	{ //Variants
		XMLElement * xml_child = xml_root->FirstChildElement("Variant");
		while (xml_child != 0) {
			int variant = rand() % 100; //A random seed to use for variants
			
			XMLElement * xml_variant = xml_child->FirstChildElement("Var");
			while (xml_variant != 0) {
				int min_val = 0;
				xml_variant->QueryIntAttribute("Min",&min_val);
				int max_val = 100;
				xml_variant->QueryIntAttribute("Max",&max_val);
				if (variant >= min_val && variant <= max_val) {
					InitializeFromXml(xml_variant); //Add the contents of this variant
				}
				xml_variant = xml_variant->NextSiblingElement("Var");
			}

			xml_child = xml_child->NextSiblingElement("Variant");
		}
	}


};



void LabyrinthInstance::Render(Transform base_transform, OpenGLContainer* open_gl) const {
	//Log("Debug", "Render: Number of inerts to render: " + intToString(inert_parts.size()));
	//Log("Debug", "Render: Number of interactive parts to render: " + intToString(interactive_parts.size()));
	static int hash_of_player_tag = HashingForTags::tags.Get("Player");

	//A light that dims when the player is dead
	glUniform4f(open_gl->LightColourIntensity, 50, 50, 230, 0.7*((32 - calls_with_no_player_objects)/32.0f));

	//Position light on player
	for (auto it = inert_parts.begin(); it != inert_parts.end(); ++it) {
		if (it->Disabled() == false) {
			if (it->HasTag(hash_of_player_tag)) {

				glUniform3f(open_gl->light_id, it->X(), it->Y(), 5);
				break;
			}
		}
	}
	
	for_each(inert_parts.cbegin(), inert_parts.cend(), [base_transform, open_gl](const LabyrinthPartInstance& a) {a.Render(base_transform.MakeModelMatrix(), open_gl); });

};


void LabyrinthInstance::Render2D(OpenGLContainer* open_gl) const {

	timer_manager->Render(open_gl);

}


LabyrinthResult LabyrinthInstance::LogicStep(std::vector<float> inputs) {
	frame++;
	static int hash_of_player_tag = HashingForTags::tags.Get("Player");
	int count_player_objects = 0;

	if (finish_frame<=-1) {
		timer_manager->Progress();
	}

	{
		int current_id = -1;
		vector<ScriptReturn> results;

		ScriptBridge::labyrinth_instance = this;//Set a pointer for the scripts to follow
		for (vector<LabyrinthPartInstance>::iterator it = inert_parts.begin(); it != inert_parts.end(); ++it) {
			current_id++;
			ScriptBridge::script_owner = current_id;
			ScriptBridge::input_x = inputs[0];
			ScriptBridge::input_y = inputs[1];
			ScriptBridge::script_parameters_floats = it->ScriptParametersFloat();//potentially expensive, maybe do this only once for each script? In any case keep the sizes small!
			ScriptBridge::script_parameters_strings = it->ScriptParametersString();

			vector<ScriptReturn> result_tmp = it->LogicStep();

			if (it->Disabled() == false) {
				if (it->HasTag(hash_of_player_tag)) {
					count_player_objects++;
				}
			}
		}
	}
	//Push back all members of the spawn list to the list of parts. They are now ready to be rendered and used next turn
	inert_parts.insert(inert_parts.end(), spawn_list.begin(), spawn_list.end());

	spawn_list.clear();

	//Handle Messages (FIFO!)	
	//TODO: This could be a queue/list or something instead of a vector since we only push/pop on one spot
	while (!messages.empty()) {
		InterObjectMessage temp = messages.front();
		int id = temp.recipient;
		if (id >= 0 && id < inert_parts.size()) {
			ScriptBridge::script_owner = id;
			ScriptBridge::input_x = 0;
			ScriptBridge::input_y = 0;
			ScriptBridge::message_parameters_strings =temp.content;//Context is the vector of the message

			inert_parts[id].RecieveMessage();
		}

		messages.pop();
	}


	ScriptBridge::labyrinth_instance = 0;
	ScriptBridge::script_owner = -1;


	//Figure out when to end the labrinths execution
	if (count_player_objects <= 0) {
		calls_with_no_player_objects++;
		if (calls_with_no_player_objects > 30) {
			//Done sequence 
			if (goal > 0) {
				LabyrinthResult temp(LabyrinthResult::state_keys::success, monster_id, goal, bonus);
				if (finish_frame < fast_frame) {
					temp.fast = true;
				}
				return temp;//Still a success
			} else {
				return LabyrinthResult(LabyrinthResult::state_keys::failure, monster_id, goal, bonus);//A failure
			}
		}
	}


	return LabyrinthResult(LabyrinthResult::state_keys::running,monster_id,goal,bonus);
	//otherwise return such an object with done=false
}




bool LabyrinthInstance::TestPartCollisionsWithTag(int part, int tag_as_int, float offset_x, float offset_y,float offset_z){
	int counter = 0;
	for (vector<LabyrinthPartInstance>::const_iterator it = inert_parts.cbegin(); it != inert_parts.cend(); ++it) {
		if (counter != part) {
			if(it->HasTag(tag_as_int)){
				if (inert_parts[part].CheckCollision(*it,offset_x,offset_y,offset_z)) {
					return true;
				}
			}
		}
		counter++;
	}
	return false;
};


int LabyrinthInstance::GetIdFromUniqueName(std::string unique_name) {
	for (unsigned int i = 0; i < inert_parts.size();i++) {
		if (inert_parts[i].UniqueName() == unique_name) {
			return i;
		}
	}

	//Log("Error(Lua)", "Unique name does not exist: " + unique_name);
	return -1;
};

int LabyrinthInstance::Spawn(std::string name, float x, float y, float z, string unique_name) {
	//You can not use the id of the newly created entity until the next call to frame(). WILL crash otherwise
	LabyrinthPartInstance temp_inst = part_def_manager->GetInstance(name);
	Transform temp(x, y, z, 1.0f);
	temp_inst.SetTransform(temp);
	temp_inst.SetUniqueName(unique_name);

	spawn_list.push_back(temp_inst);

	return  inert_parts.size() + spawn_list.size() -1;//the "to-be" id
};

int LabyrinthInstance::CollidingWithTag(int id_1, int tag_int, float offset_x, float offset_y, float offset_z) {
	int counter = 0;
	for (auto it = inert_parts.begin(); it != inert_parts.end(); ++it) {
		if (counter != id_1 && it->Disabled() == false) {
			if (it->HasTag(tag_int)) {
				if (inert_parts[id_1].CheckCollision(*it, offset_x, offset_y, offset_z)) {
					return counter;
				}//Actually colliding
			}//has tag
		}//not self and not disabled
		counter++;
	}

	return -1;
}

float LabyrinthInstance::GetDistanceFromTag(int id_1, int tag_int)
{
	float temp = 100000000.0f;
	for (auto it = inert_parts.begin(); it != inert_parts.end(); ++it) {
		if (it->Disabled() == false) {
			if (it->HasTag(tag_int)) {
				float dx = it->X() - inert_parts[id_1].X();
				float dy = it->Y() - inert_parts[id_1].Y();
				float d = dx*dx+dy*dy;
				if (d < temp){
					temp = d;
				}//Actually colliding
			}//has tag
		}//not self and not disabled
	}

	return sqrt(temp);
}

void LabyrinthInstance::GrantBonus(std::string bonus_)
{
	if (bonus_ == "ExtraTime") {
		int new_val = fast_frame + 250;//Extend timer by 250 frames, at least by 250 from now!
		if (new_val - frame < 250) {
			new_val = frame + 250;
		}
		fast_frame = new_val;
		timer_manager->AddBar(1, fast_frame, "../Data/Ui/TimerBar_Test.xml"); //id, duration, file path
	}

	bonus.push_back(bonus_);
}
void LabyrinthInstance::GrantGoal()
{
	goal++;
	if (goal == 1) {
		finish_frame = frame;
	}
}
;


bool LabyrinthInstance::UseKey() {
	if (key <= 0) {
		return false;
	}
	key--;
	return true;
};


bool LabyrinthInstance::Disable(int id) {
	if (id < 0 || id >= inert_parts.size()) {
		Log("Error", "LabyrinthInstance::Disable : invalid id");
		throw "LabyrinthInstance::Disable : invalid id";
	}
	if (inert_parts[id].Disabled()) {
		return true; //Already disabled, nothing is gonna happen
	}
	//Can't set LuaBridge variables, must be completely transparent!

	//bool result_t = inert_parts[id].Intercept_Disable();
	//if (result_t == false) {
	//	cout << "Did not prevent disable" << endl;
		inert_parts[id].Disable();
		return true;
	//}
	//cout << "Did prevent disable" << endl;
	//return false;
}

