//#include "stdafx.h"
//#include "CppUnitTest.h"

#include "Stats.h"

#include "Log.h"
#include "StringUtility.h"

;

using namespace std;

Stats::Stats()
	:health(0),max_health(0),attack(0),armor(0),spell_points(0)
{
}

Stats::Stats(int h,int at,int ar,int ca)
	:health(h),max_health(h),attack(at),armor(ar), spell_points(ca)
{
}

Stats::Stats(const tinyxml2::XMLElement* p_xml_stats) 
	: health(0), max_health(0),attack(0), armor(0), spell_points(0)
{

	if (p_xml_stats == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}

	//its completely legal to omit certain stats and use default values in the xml so we have to nil check each one!
	const tinyxml2::XMLElement* p_location = p_xml_stats->FirstChildElement("Health");
	if (p_location) {
		health = stoi(p_location -> GetText());
		max_health = health;
		//cout << "Health: " << health << endl;
	}

	p_location = p_xml_stats->FirstChildElement("Attack");
	if (p_location) {
		attack = stoi(p_location->GetText());
	}


	p_location = p_xml_stats->FirstChildElement("Armor");
	if (p_location) {
		armor = stoi(p_location->GetText());
	}

	p_location = p_xml_stats->FirstChildElement("SpellPoints");
	if (p_location) {
		spell_points = stoi(p_location->GetText());
	}

};

void Stats::operator+=(const Stats& rhs){
	health += rhs.health;
	max_health += rhs.max_health;
	attack += rhs.attack;
	armor += rhs.armor;
	spell_points += rhs.spell_points;
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
		Stats a(health - dmg_after_armor, attack, armor, spell_points);
		a.max_health = health;
		return a;
	}
	return Stats(health, attack, armor, spell_points);
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
