#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Transform.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tinyxml2.h"

class LabyrinthPartDefinition;
class OpenGLContainer;

class PseudoInstance {
public:
	PseudoInstance();
	PseudoInstance(tinyxml2::XMLElement * xml_root);


	tinyxml2::XMLElement* MakeXML(tinyxml2::XMLDocument&) const;


	std::vector<float> ScriptParametersFloat() { return script_parameters_float; };
	std::vector<std::string> ScriptParametersString() { return script_parameters_string; };
private:
	std::string my_type;
	std::string unique_name;

	Transform transform;

	std::vector<float> script_parameters_float;
	std::vector<std::string> script_parameters_string;

};

