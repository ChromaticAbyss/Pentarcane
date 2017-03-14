#pragma once

#include <vector>
#include <string>


#include "../tinyxml2.h"

class Monster;

class CastCondition {
public:
	enum TypeEnum{nothing, condition_to_monster};

	CastCondition();
	CastCondition(tinyxml2::XMLElement * );

	bool Evaluate(std::vector<Monster> monsters,int self); //+ player data


private:
	void InitializeAsConditionToMonster(tinyxml2::XMLElement * xml_root);


	TypeEnum type;

	std::string target;

	std::vector<std::string> conditions_to_monster;
};
