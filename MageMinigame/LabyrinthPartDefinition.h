#pragma once

#include "BuildHeader.h"

#include <vector>
#include <string>
#include <memory>

#include "Collider2D.h"
#include "Definition3D.h"

#include "tinyxml2.h"

class LabyrinthPartDefinition{
	friend class LabyrinthPartInstance;
public:
	LabyrinthPartDefinition(tinyxml2::XMLElement *);

	std::string InternalName() const {return internal_name;}; 

	bool CanAppearInEditor() const { return can_appear_in_editor; };
	void ReadyForInstantiation();

	bool HasTag(const std::string&) const;
#ifdef MAGEMINIGAME_MAPEDITOR
	std::vector<std::string> Tags() const { return tags; };
#endif 

private:
	std::string internal_name;
	bool model_loaded;
	std::shared_ptr<Definition3D> model_def;
	bool can_appear_in_editor;

	std::vector<Collider2D> colliders;

	std::vector<std::string> tags;

	std::vector<std::string> logic_step_scripts;

	bool spawn_with_unique_name;//Editor: Makes the entity always spawn with a unique name
};

