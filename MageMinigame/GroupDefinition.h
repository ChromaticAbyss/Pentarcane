#pragma once

#include <vector>
#include <string>

#include "tinyxml2.h"

#include "Fight/FightData.h"

class GroupDefinition {
public:
	explicit GroupDefinition(const tinyxml2::XMLElement * pointer_xml);

	int Likelihood() const { return likelihood; };
	std::string InternalName() const { return internal_name; };
	bool HasTag(std::string) const; //TODO: Fullfills condition, with AND,OR etc., remove this function once thats done
	std::vector<std::string> MonsterNames() const { return monsters; };

	bool FulfillsCondition(const std::vector<FightDataCondition>& ) const;
	bool FulfillsCondition(const FightDataCondition& ) const;
	bool ValueCondition(const FightDataCondition&,float) const;


private:
	std::string internal_name;
	std::vector<std::string> monsters;
	std::vector<std::string> tags;
	int likelihood;
	int power_rating;
};