#ifndef MONSTERDEF_MANAGER_H
#define MONSTERDEF_MANAGER_H

#include <vector>
#include <string>

#include "MonsterDefinition.h"
#include "Monster.h"
#include "Transform.h"

class MonsterDefinition_Manager{
public:
	explicit MonsterDefinition_Manager(std::vector<std::string>);

	Monster MakeMonster(std::string name, const Transform2D & tr);


private:
	std::vector<MonsterDefinition> definitions;

};

#endif