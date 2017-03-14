#pragma once

#include <vector>
#include <string>

#include "tinyxml2.h"

#include "Conditions/CastCondition.h"

class Monster;

class MonsterCastSegment {
	friend class Fight;//Fight class may use this and its contents
public: 
	explicit MonsterCastSegment();
	explicit MonsterCastSegment(int damage);
	explicit MonsterCastSegment(std::string); //Test-Animation
	explicit MonsterCastSegment(tinyxml2::XMLElement* p_definition);

	std::vector<int> SelectTargetsFrom(const std::vector<Monster>&); //Can't select player!

	void SetCasterID(int a) { caster_id = a; };

private:
	std::string type;

	std::vector<int> values;
	std::vector<std::string> string_values;
	std::string targets;

	int caster_id;
};



class MonsterCast {
public:
	explicit MonsterCast(int damage,std::string animation_name);
	explicit MonsterCast(tinyxml2::XMLElement* p_definition);
	explicit MonsterCast();

	MonsterCastSegment Progress();
	bool Finished() const { return (execution_position>=segments.size()); };
	inline std::string Name() const { return internal_name; };
	void SetCasterID(int a);
	int GetCasterID() const{ return caster_id; };
private:
	int execution_position;
	std::string internal_name;

	std::vector<MonsterCastSegment> segments;
	
	std::vector<std::string> conditions;

	std::vector<CastCondition> cast_conditions;

	int caster_id;

};