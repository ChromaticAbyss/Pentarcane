//#include "stdafx.h"
//#include "CppUnitTest.h"

#include "Stats.h"

#include "Log.h"
#include "StringUtility.h"

using namespace tinyxml2;

using namespace std;

Stats::Stats()
	:health(0),max_health(0),attack(0),armor(0),cast_chance(0)
{
}

Stats::Stats(int h,int at,int ar,int ca)
	:health(h),max_health(h),attack(at),armor(ar),cast_chance(ca)
{
}

Stats::Stats(tinyxml2::XMLElement* p_xml_stats) 
	: health(0), max_health(0),attack(0), armor(0), cast_chance(0)
{

	if (p_xml_stats == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}

	//its completely legal to omit certain stats and use default values in the xml so we have to nil check each one!
	XMLElement* p_location = p_xml_stats->FirstChildElement("Health");
	if (p_location != 0) {
		health = stoi(p_location -> GetText());
		max_health = health;
		//cout << "Health: " << health << endl;
	}

	p_location = p_xml_stats->FirstChildElement("Attack");
	if (p_location != 0) {
		attack = stoi(p_location->GetText());
	}


	p_location = p_xml_stats->FirstChildElement("Armor");
	if (p_location != 0) {
		armor = stoi(p_location->GetText());
	}

	p_location = p_xml_stats->FirstChildElement("CastChance");
	if (p_location != 0) {
		cast_chance = stoi(p_location->GetText());
	}

};

void Stats::operator+=(const Stats& rhs){
	health += rhs.health;
	max_health += rhs.max_health;
	attack += rhs.attack;
	armor += rhs.armor;
	cast_chance += rhs.cast_chance;
}


const Stats Stats::operator+(const Stats &rhs) const {
  Stats result = *this;     // Make a copy of myself.  Same as Stats result(*this);
  result += rhs;            // Use += to add other to the copy.
  return result;              // All done!
}

Stats Stats::DealDamage(int dmg,bool ignore_armor) const {
	int dmg_after_armor = dmg - armor;
	if (ignore_armor) {
		dmg_after_armor = dmg;
	}

	if (dmg_after_armor > 0) {
		Stats a(health - dmg_after_armor, attack, armor, cast_chance);
		a.max_health = health;
		return a;
	}
	return Stats(health, attack, armor, cast_chance);
}

Stats Stats::Heal(int amount) const
{

	Stats result = *this;
	result.health += amount;
	if (result.health > result.max_health) {
		result.health = result.max_health;
	}

	return result;
}
