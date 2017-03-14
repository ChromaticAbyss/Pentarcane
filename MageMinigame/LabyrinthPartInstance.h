#pragma once

#include "BuildHeader.h"

#include <vector>
#include <string>
#include <memory>

#include "Collider2D.h"
#include "Instance3D_New.h"
#include "Transform.h"
#include "ScriptReturn.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tinyxml2.h"

class LabyrinthPartDefinition;
class OpenGLContainer;

class LabyrinthPartInstance {
public:
	LabyrinthPartInstance();
	LabyrinthPartInstance(const Transform &, const LabyrinthPartDefinition &);
	LabyrinthPartInstance(tinyxml2::XMLElement * xml_root, const LabyrinthPartDefinition &);

	void Render(glm::mat4, OpenGLContainer*) const;

	std::vector<ScriptReturn> LogicStep();
	void RecieveMessage();
	inline bool Intercept_Disable() {
		return false;
	};

	void SetTransform(Transform);
	void Move(float dx, float dy, float dz = 0.0f);
	void MoveTo(float tx, float ty, float tz = 0.0f) { Move(tx - transform.X(), ty - transform.Y(), tz - transform.Z()); };


	bool CheckCollision(LabyrinthPartInstance other, float, float, float);
	bool HasTag(int) const;

	void Hide() { hidden = true; };
	void Unhide() { hidden = false; };
	void Disable() { hidden = true; disabled = true; };
	bool Hidden() const { return hidden; };
	bool Disabled() const { return disabled; };

	std::string UniqueName() { return unique_name; };
	//Editor functionality
	float X() const { return transform.X(); };
	float Y() const { return transform.Y(); };
	float Z() const { return transform.Z(); };
	std::string TypeName() const { return my_type; };
	tinyxml2::XMLElement* MakeXML(tinyxml2::XMLDocument&) const;
	void SetUniqueName(std::string name) { unique_name = name; };
	void SetUniqueNameRandom();


	bool setup_lua_script();

	std::vector<float> ScriptParametersFloat() { return script_parameters_float; };
	std::vector<std::string> ScriptParametersString() { return script_parameters_string; };

#ifdef MAGEMINIGAME_MAPEDITOR
	void SetScriptParameterFloat(int nr, float val);
#endif 


#ifndef MAGEMINIGAME_MAPEDITOR //Everything is public in map editor
private:
#endif 
	std::string my_type;
	std::string unique_name;
	bool hidden;
	bool disabled;

	Transform transform;

	std::vector<Collider2D> colliders;


	Instance3D_New model;

	std::vector<std::string> logic_step_script_names;

	std::vector<float> script_parameters_float;
	std::vector<std::string> script_parameters_string;

	std::vector<int> tags_as_ints;

	int script_state;
	lua_State *lua_script;
};

