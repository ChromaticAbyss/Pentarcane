#include "LabyrinthPartInstance.h"
#include "LabyrinthPartDefinition.h"
#include "OpenGLContainer.h"

#include "Log.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "ScriptBridge.h"
#include "HashingForTags.h"

using namespace std;
using namespace tinyxml2;

LabyrinthPartInstance::LabyrinthPartInstance(const Transform& tra, const LabyrinthPartDefinition& definition)
	:my_type(definition.InternalName()),unique_name(),hidden(false),disabled(false),transform(tra), colliders(definition.colliders), model(definition.model_def)
	,logic_step_script_names(definition.logic_step_scripts),script_parameters_float(),script_parameters_string(),tags_as_ints()
	,script_state(0)
{
	for (auto it = definition.tags.cbegin(); it != definition.tags.cend(); ++it) {
		int a = HashingForTags::tags.Get(*it);
		//cout << "-------------------------------------tag added: " << a << endl;
		tags_as_ints.push_back(a);
	}

	if (definition.spawn_with_unique_name) {
		SetUniqueNameRandom();
	}

};

LabyrinthPartInstance::LabyrinthPartInstance(tinyxml2::XMLElement * xml_root, const LabyrinthPartDefinition & definition)
	:my_type(definition.InternalName()), unique_name(), hidden(false), disabled(false), transform(), colliders(definition.colliders), model(definition.model_def)
	, logic_step_script_names(definition.logic_step_scripts), script_parameters_float(), script_parameters_string(), tags_as_ints()
	, script_state(0)
{
	//Read additional data from the XML

	{
		XMLElement * xml_child_type = xml_root->FirstChildElement("UniqueName");
		if (xml_child_type != 0) {
			unique_name = xml_child_type->GetText();
		}
	}


	{//Get transform info from the child
		XMLElement * xml_child_transform = xml_root->FirstChildElement("Transform");
		if (xml_child_transform != 0) {
			transform = Transform(xml_child_transform);
		}
		else {
			Log("Error", "PartInstance data incomplete, missing Transform");
		}
	}



	{
		XMLElement * xml_child = xml_root->FirstChildElement("ScriptParameterFloat");
		while (xml_child != 0) {
			script_parameters_float.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("ScriptParameterFloat");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Float");
		while (xml_child != 0) {
			script_parameters_float.push_back(stof(xml_child->GetText()));
			xml_child = xml_child->NextSiblingElement("Float");
		}
	}



	{
		XMLElement * xml_child = xml_root->FirstChildElement("ScriptParameterString");
		while (xml_child != 0) {
			script_parameters_string.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("ScriptParameterString");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("String");
		while (xml_child != 0) {
			script_parameters_string.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("String");
		}
	}


	for (auto it = definition.tags.cbegin(); it != definition.tags.cend(); ++it) {
		int a = HashingForTags::tags.Get(*it);
		//cout << "-------------------------------------tag added: " << a << endl;
		tags_as_ints.push_back(a);
	}

};



LabyrinthPartInstance::LabyrinthPartInstance()
	:my_type(),unique_name(),hidden(true),disabled(true),transform(), colliders(), model()
	,logic_step_script_names(), script_parameters_float(), script_parameters_string(), tags_as_ints(), script_state(0)
{

};


void LabyrinthPartInstance::Render(glm::mat4 outer_matrix, OpenGLContainer* open_gl) const {
	if (hidden || disabled) { 
		return; 
	} else {
		//Log("Debug", "LabyrinthPartInstance::Render called");
		glm::mat4 new_matrix = outer_matrix * transform.MakeModelMatrix();
		model.Render(new_matrix, open_gl);
	}
};

void LabyrinthPartInstance::SetTransform(Transform t) {
	transform = t;
};

void LabyrinthPartInstance::Move(float dx, float dy, float dz) {
	transform.Move(dx, dy, dz);
};

vector<ScriptReturn> LabyrinthPartInstance::LogicStep() {
	vector<ScriptReturn> result;
	if (logic_step_script_names.size()==0 || disabled || script_state <= -1) { return result; }

	if (script_state == 0) {
		script_state = 1;
		setup_lua_script();//may set script state to -1 on failure
	}
	if (lua_script) {

		//run script
		lua_getglobal(lua_script, "frame");
		if (lua_isfunction(lua_script, 1) == 1) {
			lua_pcall(lua_script, 0, 0, 0);//Number arguments: #Parameters, #Returns, Error handler function
		} else {
			lua_settop(lua_script, 0); //As to not fuck up other lua function calls!
			Log("Error(Lua)", "frame() missing / file not loaded " + logic_step_script_names[logic_step_script_names.size() - 1]);
		}


	}else {
		Log("Error(Lua)", "Lua script not active even though it should be");
	}

	return result;
};


bool LabyrinthPartInstance::CheckCollision(LabyrinthPartInstance other,float o_x,float o_y, float o_z){
	//Loop over colliders of each type with each type of the other side and see if any overlap

	Transform shifted_transform(transform);//Make a copy of my transform and shift it with the offset
	shifted_transform.Move(o_x, o_y, o_z);

	for (auto it = colliders.cbegin(); it != colliders.cend(); ++it) {
		for (auto it2 = other.colliders.cbegin(); it2 != other.colliders.cend(); ++it2) {
			if (it->CheckCollision(shifted_transform,*it2,other.transform)) {
				return true;
			}
		}
	}

	return false;
};


//TODO: SOmetihng in this function is terribly slow!
bool LabyrinthPartInstance::HasTag(int tag_as_int) const{
	for (int i = 0; i < tags_as_ints.size();i++) {
		if (tags_as_ints[i] == tag_as_int) {
			//cout << "has tag" << tag_as_int << endl;
			return true;
		}
	}
	return false;
}


tinyxml2::XMLElement* LabyrinthPartInstance::MakeXML(XMLDocument & doc) const {
	XMLElement * return_element = doc.NewElement("Part");
		
	XMLElement * type_element = doc.NewElement("Type");
	type_element->SetText(my_type.c_str());
	return_element->InsertEndChild(type_element);

	XMLElement * transform_element = transform.MakeXML(doc);
	return_element->InsertEndChild(transform_element);

	if (unique_name != "") {
		XMLElement * unique_name_element = doc.NewElement("UniqueName");
		unique_name_element->SetText(unique_name.c_str());
		return_element->InsertEndChild(unique_name_element);
	}

	for (auto it = script_parameters_string.cbegin(); it != script_parameters_string.cend(); ++it) {
		XMLElement * element = doc.NewElement("String");
		element->SetText(it->c_str());
		return_element->InsertEndChild(element);
	}

	for (auto it = script_parameters_float.cbegin(); it != script_parameters_float.cend(); ++it) {
		XMLElement * element = doc.NewElement("Float");
		element->SetText(*it);
		return_element->InsertEndChild(element);
	}

	return return_element;
}
void LabyrinthPartInstance::SetUniqueNameRandom()
{
	static int blah = 0;
	string new_name = TypeName();
	new_name += " " + intToString(blah++);
	new_name += " " + intToString(rand() % 1000);
	SetUniqueName(new_name);
}
;


bool LabyrinthPartInstance::setup_lua_script() {
	if (logic_step_script_names.size() == 0) {
		return false;//No script found to set up(inert parts like walls have no scripts)
	}
	lua_script = lua_open();
	if (lua_script) {
		Log("Debug", "Setting up lua script");

		//lua_script=lua_open();
		luaL_openlibs(lua_script);

		//Register script bridge functions for use in the script
		lua_register(lua_script, "TestLua", ScriptBridge::TestLua);
		lua_register(lua_script, "GetMyId", ScriptBridge::GetMyId);
		lua_register(lua_script, "GetInputs", ScriptBridge::GetInputs);
		lua_register(lua_script, "GetIdFromUniqueName", ScriptBridge::GetIdFromUniqueName);
		lua_register(lua_script, "Spawn", ScriptBridge::Spawn);
		lua_register(lua_script, "Hide", ScriptBridge::Hide);
		lua_register(lua_script, "Unhide", ScriptBridge::Unhide);
		lua_register(lua_script, "IsHidden", ScriptBridge::IsHidden);
		lua_register(lua_script, "Disable", ScriptBridge::Disable);
		lua_register(lua_script, "GetIsDisabled", ScriptBridge::GetIsDisabled);
		lua_register(lua_script, "GetPosition", ScriptBridge::GetPosition);
		lua_register(lua_script, "Move", ScriptBridge::Move);
		lua_register(lua_script, "MoveTo", ScriptBridge::MoveTo);
		lua_register(lua_script, "CheckCollisionWithTag", ScriptBridge::CheckCollisionWithTag);
		lua_register(lua_script, "GetDistanceFromTag", ScriptBridge::GetDistanceFromTag);
		lua_register(lua_script, "GetScriptParameterString", ScriptBridge::GetScriptParameterString);
		lua_register(lua_script, "GetMessageParameterString", ScriptBridge::GetMessageParameterString);
		lua_register(lua_script, "GetScriptParameterFloat", ScriptBridge::GetScriptParameterFloat);
		lua_register(lua_script, "Sqrt", ScriptBridge::Sqrt);
		lua_register(lua_script, "GrantLabyrinthBonus", ScriptBridge::GrantLabyrinthBonus);
		lua_register(lua_script, "GrantLabyrinthGoal", ScriptBridge::GrantLabyrinthGoal);
		lua_register(lua_script, "GrantLabyrinthKey", ScriptBridge::GrantLabyrinthKey);
		lua_register(lua_script, "UseLabyrinthKey", ScriptBridge::UseLabyrinthKey);
		lua_register(lua_script, "GetSpellType", ScriptBridge::GetSpellType);
		lua_register(lua_script, "SendMessage", ScriptBridge::SendMessage);
		lua_register(lua_script, "PlaySound", ScriptBridge::PlaySound);

		for (unsigned int s = 0; s<logic_step_script_names.size(); s++) {
			string file = logic_step_script_names[s];
			luaL_dofile(lua_script, file.c_str());
		}
		//luaL_dofile(lua_script, logic_step_script_name.c_str());		
		//Call the setup function
		lua_getglobal(lua_script, "setup");
		if (lua_isfunction(lua_script, 1) == 1) {
			lua_pcall(lua_script, 0, 0, 0);//Number arguments: #Parameters, #Returns, Error handler function
		}
		else {
			lua_settop(lua_script, 0);
			Log("Error(Lua)", "setup missing / file not loaded" + logic_step_script_names[logic_step_script_names.size() - 1]);
			script_state = -1;
			return false;
		}

		//Log("Debug","Finished setting up this lua script");
	}
	return true;

}
#ifdef MAGEMINIGAME_MAPEDITOR
void LabyrinthPartInstance::SetScriptParameterFloat(int nr, float val)
{
	while ((int)script_parameters_float.size() - 1 <= nr) {
		if ((int)script_parameters_float.size() - 2 <= nr) { cout << "Missing a previous parameter, set to 1.0f!" <<endl; }
		script_parameters_float.push_back(1.0f);
	}
	script_parameters_float[nr] = val;
}
;
#endif

//Weird spam has nothing to do with this function, 
void LabyrinthPartInstance::RecieveMessage() {
	//cout << "Recieve message " << ScriptBridge::message_parameters_strings[0] << endl;

	if (logic_step_script_names.size() == 0) {
		Log("Error", "Sending messages to objects without scripts is pointless! Recipient unique_name: "+unique_name);
		return;//No script found to set up(inert parts like walls have no scripts)
	}
	if (logic_step_script_names.size() == 0 || disabled || script_state <= -1) { return; }



	if (script_state == 0) {
		script_state = 1;
		setup_lua_script();//may set script state to -1 on failure
	}


	if (lua_script) {
		//run script
		lua_getglobal(lua_script, "message");
		if (lua_isfunction(lua_script, 1) == 1) {
			lua_pcall(lua_script, 0, 0, 0);//Number arguments: #Parameters, #Returns, Error handler function
		}
		else {
			lua_settop(lua_script, 0); //Flush stack, as to not fuck up other lua function calls!
			Log("Error(Lua)", "message() missing / file not loaded " + logic_step_script_names[logic_step_script_names.size() - 1]);
		}


	} else {
		Log("Error(Lua)", "Lua script not active even though it should be");
	}


	return;
};