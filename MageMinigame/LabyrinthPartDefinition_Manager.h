#pragma once

#include "BuildHeader.h"

#include <vector>
#include <string>
#include <memory>
#include <tuple>

#include "LabyrinthPartDefinition.h"
#include "LabyrinthPartInstance.h"

#include "tinyxml2.h"

class LabyrinthPartDefinition_Manager{
public:
	LabyrinthPartDefinition_Manager(std::vector<std::string>);

	LabyrinthPartInstance GetInstance(std::string);
	LabyrinthPartInstance GetInstanceFromXml(tinyxml2::XMLElement *);
#ifdef MAGEMINIGAME_MAPEDITOR
	std::tuple<std::vector<LabyrinthPartInstance>,std::vector<std::string>> GrabInstancesByFilter(const std::vector<std::string>&, const std::vector<std::string>&);
#endif 
private:
	std::vector<LabyrinthPartDefinition> definitions;
};

