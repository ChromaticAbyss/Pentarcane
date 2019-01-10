#include "CastCondition.h"

#include "../Monster/Monster.h"
#include "../PlayerState.h"
#include "Fight\Fight.h"

#include <algorithm>

using namespace std;

CastCondition::CastCondition()
	:type(TypeEnum::nothing),target()
{
}

CastCondition::CastCondition(const tinyxml2::XMLElement * xml_root)
	:type(TypeEnum::nothing),target(TargetEnum::nothing)
{


	{
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != nullptr) {
			string t = xml_child->GetText();
			if (t == "Alive") {
				type = TypeEnum::alive;
			}
			if (t == "Dead") {
				type = TypeEnum::dead;
			}
			if (t == "FullLife") {
				type = TypeEnum::full_life;
			}
			if (t == "NotFullLife") {
				type = TypeEnum::not_full_life;
			}
			if (t == "HighSpellPoints") {
				type = TypeEnum::high_spell_points;
			}
			if (t == "ArenaNotFull") {
				type = TypeEnum::arena_not_full;
			}
		}
	}


	{
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Target");
		if (xml_child != nullptr) {
			string t = xml_child->GetText();
			if (t == "Self") {
				target = TargetEnum::self;
			}
			if (t == "AllMonsters") {
				target = TargetEnum::all_monsters;
			}
			if (t == "AnyMonster") {
				target = TargetEnum::any_monster;
			}
			if (t == "Player") {
				target = TargetEnum::player;
			}
		}
	}


	if(target== TargetEnum::player){ //If we target the player, out sub-conditions are of the kind that target the player, duh
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Condition");
		if (xml_child != nullptr) {
			conditions_to_player.emplace_back(xml_child);
		}
	}



}

bool CastCondition::evaluate(int my_id, const Fight* fight) const
{

	if (type == TypeEnum::arena_not_full) {
		int living_monsters = 0;
		for (const auto& mon : fight->monsters) {
			if (mon.Alive()) {
				++living_monsters;
			}
		}
		if (living_monsters < fight->stage.NrPositions()) {
			return true;
		}
		return false;
	}

	if (target == TargetEnum::player) {
		for (const auto& s : conditions_to_player) { if (!s.Evaluate(fight->player_state)) return false; }
		return true;
	}


	if (target == TargetEnum::self) {
		return CheckConditionOnMonster(fight->monsters[my_id]);
	}
	if (target == TargetEnum::all_monsters) {
		for (const Monster& m : fight->monsters) {
			if (!CheckConditionOnMonster(m)) {
				return false;
			}
		}
		return true;
	}
	if (target == TargetEnum::any_monster) {
		for (const Monster& m : fight->monsters) {
			if (CheckConditionOnMonster(m)) {
				return true;
			}
		}
		return false;
	}


	return false;
}

bool CastCondition::CheckConditionOnMonster(const Monster& mon) const
{
	Stats mon_stats = mon.ReadStats();

	switch (type) {
	case TypeEnum::alive:
			return mon.Alive();
	case TypeEnum::full_life:
			return mon.Alive() && mon_stats.health == mon_stats.max_health;
	case TypeEnum::not_full_life:
			return mon.Alive() && mon_stats.health< mon_stats.max_health;
	case TypeEnum::dead:
			return !mon.Alive();
	case TypeEnum::high_spell_points:
		return mon.ReadStats().spell_points >= 5;
	}
	return false;
}

