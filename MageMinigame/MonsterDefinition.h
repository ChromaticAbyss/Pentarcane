
#ifndef MONSTER_DEF_H
#define MONSTER_DEF_H

#include <string>
#include <vector>
#include <memory>
#include "Stats.h"

#include "Definition3D.h"

#include "tinyxml2.h"

#include "MonsterCast.h"

#include "Ui/Transform2D.h"

class MonsterDefinition{
	friend class Monster;
public:
	explicit MonsterDefinition();
	explicit MonsterDefinition(tinyxml2::XMLElement* p_monster_definition);

	void ReadyForInstantiation();
	std::string GiveName() const{return internal_name;};
	std::string GiveDisplayName() const { return display_name; };
	std::string RandomLabyrinth() const;

	std::vector<MonsterCast> GiveListOfCasts() const { return casts; };
	inline bool HasNoCasts() const { return casts.empty(); };

	inline std::string DefaultAttackAnimation() const { return default_attack_animation; };

	std::vector<MonsterCast> GiveCastWhenDamageTaken() const;
private:
	Stats stats;
	std::string internal_name;
	std::string display_name;
	std::vector<MonsterCast> casts;
	std::vector<MonsterCast> cast_when_damage_taken;

	std::vector<std::string> labyrinths;

	bool model_loaded;
	std::shared_ptr<Definition3D> model_def;

	std::string image_name;
	Transform2D image_transform;
	std::string default_attack_animation;
};


#endif