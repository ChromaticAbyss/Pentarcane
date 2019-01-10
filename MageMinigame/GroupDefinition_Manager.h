#pragma once


#include "GroupDefinition.h"

#include <vector>
#include <string>

#include "Fight/FightData.h"

class GroupDefinition_Manager {
public:
	explicit GroupDefinition_Manager(std::vector<std::string> files);

	std::vector<std::string> GrabFittingGroup(std::vector<FightDataCondition>,std::vector<std::string>); //Last entry in string: group name


private:
	std::vector<GroupDefinition> groups;

};