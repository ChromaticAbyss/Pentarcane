#pragma once

#include <vector>
#include <string>


#include "tinyxml2.h"

class Monster;
class PlayerState;
class Fight;

#include "ConditionToPlayer.h"

class CastCondition {
public:
	 enum struct TypeEnum{nothing, alive, full_life, not_full_life,dead, high_spell_points,arena_not_full};
	 enum struct TargetEnum {nothing, self, all_monsters, any_monster, player};

	CastCondition();
	CastCondition(const tinyxml2::XMLElement * );

	bool evaluate(int my_id, const Fight* fight) const;


private:
	bool CheckConditionOnMonster(const Monster&) const;

	TypeEnum type;

	TargetEnum target;


	std::vector<ConditionToPlayer> conditions_to_player;
};
