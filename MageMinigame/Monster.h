#ifndef MONSTER_H_
#define MONSTER_H_

#include <string>
#include <memory>
#include <vector>

#include "Stats.h"
#include "Instance3D_New.h"
#include "Transform.h"

#include "MonsterCast.h"

#include "Ui/Element2D.h"

class MonsterDefinition;

class OpenGLContainer;

class Monster{
public:
	explicit Monster();
	explicit Monster(Transform2D, const MonsterDefinition*);

	std::string InternalName() const {return internal_name;};

	void Render(OpenGLContainer* open_gl) const;
	void Render2D(OpenGLContainer* open_gl) const;


	void SetPosition(const Transform2D& t) { transform = t; };
	//void ChangeTransform(const Transform2D& t) { transform = transform.Merge(t); };


	void SetLabyrinth(const std::string& new_lab) { my_labyrinth = new_lab; };
	void ApplyBonusStats(const Stats& s) { stats = stats + s; };
	std::vector<MonsterCast> DealDamage(int damage,bool ignore_armor=false);//Returns a monster retaliation cast
	void Heal(int amount) { stats = stats.Heal(amount); };
	bool Alive() const { return (stats.Health()>0); };
	const Stats ReadStats() const { return stats; };
	std::string DisplayName() const;
	void PassTurn();
	void ApplyEffect_Freeze() { effect_block_attack = true; };

	std::vector<MonsterCast> MonsterAction() const;

	std::string Labyrinth() const { return my_labyrinth; };
private:
	const MonsterDefinition* definition; //Has a pointer to its definition. Can not change its definition!
	//Because MonsterDefinition vector does not change during the game, the pointer each instance takes can never become invalid!

	Stats stats;
	bool effect_block_attack;//Can't attack next turn


	std::string internal_name;
	std::string my_labyrinth;
	Instance3D_New model;
	Transform2D transform;


	Instance2D image;
};





#endif