#pragma once
#include "tinyxml2.h"

class Stats{
public:
	explicit Stats();
	explicit Stats(int,int,int,int);
	explicit Stats(tinyxml2::XMLElement* p_xml_stats);

	void operator+=(const Stats&);
	const Stats operator+(const Stats&) const;

	int Health() const{return health;};
	int Attack() const{return attack;};
	int Armor() const{return armor;};
	int Cast_Chance() const{return cast_chance;};

	Stats DealDamage(int,bool) const;
	Stats Heal(int) const;

private:
	int health;
	int max_health;
	int attack;
	int armor;
	int cast_chance;

};

