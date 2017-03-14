#include "LabyrinthPartDefinition.h"

#include "Log.h"

#include <algorithm>

using namespace std;
using namespace tinyxml2;

LabyrinthPartDefinition::LabyrinthPartDefinition(tinyxml2::XMLElement * xml_root)
	:internal_name("Name not initialized"),model_loaded(false),model_def(), can_appear_in_editor(true),colliders(),tags(),logic_step_scripts()// <- is that enough to initialize collider to null?
	, spawn_with_unique_name(false)
{
	if(xml_root == nullptr){
		Log("Error","LabyrinthPartDefinition::LabyrinthPartDefinition xml_root is null");
		return;
	}

 	{ //Look up internal name
		XMLElement * xml_child = xml_root-> FirstChildElement("InternalName");
		if(xml_child != nullptr){
			internal_name = xml_child -> GetText();
		}
	}
 
	Log("Data","Making LabyrinthPartDefinition with internal_name "+internal_name);


 	{ //Look up a model in the node
		XMLElement * xml_child = xml_root-> FirstChildElement("Art3D");
		if(xml_child != nullptr){
			model_def = make_shared<Definition3D>(xml_child);
		}
	}
 

 	{ //Look up a collider
		XMLElement * xml_child = xml_root-> FirstChildElement("CircleCollider");
		if(xml_child != nullptr){
			colliders.push_back(Collider2D(0,xml_child));
			xml_child = xml_child->NextSiblingElement("CircleCollider");
		}
	}
	{ //Look up a collider
		XMLElement * xml_child = xml_root->FirstChildElement("RectangleCollider");
		while (xml_child != nullptr) {
			colliders.push_back(Collider2D(1, xml_child));
			xml_child = xml_child->NextSiblingElement("RectangleCollider");
		}
	}

	{ //
		XMLElement * xml_child = xml_root->FirstChildElement("LogicStepScript");
		while(xml_child != nullptr) {
			logic_step_scripts.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("LogicStepScript");
		}
	}

	{ //
		XMLElement * xml_child = xml_root->FirstChildElement("Tag");
		while (xml_child != nullptr) {
			tags.push_back(xml_child->GetText());
			xml_child = xml_child->NextSiblingElement("Tag");
		}
	}


	{ //
		XMLElement * xml_child = xml_root->FirstChildElement("CanAppearInEditor");
		if (xml_child != nullptr) {
			string a = xml_child->GetText();
			if (a == "0") {
				can_appear_in_editor = false;
			}
		}
	}

	{ //
		XMLElement * xml_child = xml_root->FirstChildElement("SpawnWithUniqueName");
		if (xml_child != nullptr) {
			string a = xml_child->GetText();
			if (a == "1" || a == "true" || a== "True") {
				spawn_with_unique_name = true;
			}
		}
	}

};


void LabyrinthPartDefinition::ReadyForInstantiation() {
	if (model_loaded == false) {
		model_def->attemptToLoadModel();
		model_loaded = true;
	}
}

bool LabyrinthPartDefinition::HasTag(const std::string& name) const
{
	auto a = std::find(tags.cbegin(), tags.cend(), name);
	if (a == tags.cend()) { return false; }
	return true;
};