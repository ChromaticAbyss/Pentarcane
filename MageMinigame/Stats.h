#pragma once
#include "tinyxml2.h"

class Stats{
public:
	explicit Stats();
	explicit Stats(int health,int attack,int armor,int spell_points);
	explicit Stats(const tinyxml2::XMLElement* p_xml_stats);

	void operator+=(const Stats&);
	const Stats operator+(const Stats&) const;

	Stats DealDamage(int,bool) const;
	Stats Heal(int) const;

	int health;
	int max_health;
	int attack;
	int armor;
	int spell_points;

};

