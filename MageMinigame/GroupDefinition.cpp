#include "GroupDefinition.h"
#include "Log.h"

using namespace tinyxml2;

using namespace std;

#include <algorithm>


GroupDefinition::GroupDefinition(XMLElement * pointer_xml) 
	:internal_name("GroupDef internal_name not set"),monsters(),tags(),likelihood(1000),power_rating(100)
{
	if (pointer_xml == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}
	internal_name = (pointer_xml->FirstChildElement("InternalName"))->GetText();
	Log("Data", "Loading GroupDefinition with internal name: " + internal_name);

	//Load all monster names for that group
	{
		XMLElement* p_lab_entry = pointer_xml->FirstChildElement("Monster");
		while (p_lab_entry != 0) {
			monsters.push_back(p_lab_entry->GetText());
			//cout << "Labyrinth: " << labyrinths[labyrinths.size() - 1] << endl;
			p_lab_entry = p_lab_entry->NextSiblingElement("Monster");
		}
	}

	//Load all Tags for this group
	{
		XMLElement* p_cast_entry = pointer_xml->FirstChildElement("Tag");
		while (p_cast_entry != 0) {
			tags.push_back(p_cast_entry->GetText());
			p_cast_entry = p_cast_entry->NextSiblingElement("Tag");
		}
	}

	//Look for likelihood tag
	{
		XMLElement* p_entry = pointer_xml->FirstChildElement("Likelihood");
		if (p_entry != 0) {
			likelihood = stoi(p_entry->GetText());
		}
	}

	//Look for power rating tag
	{
		XMLElement* p_entry = pointer_xml->FirstChildElement("PowerRating");
		if (p_entry != 0) {
			power_rating = stoi(p_entry->GetText());
		}
	}


	Log("Data", "Done loading GroupDefinition with internal name: " + internal_name);

};


bool GroupDefinition::HasTag(string look_for) const{
	return any_of(tags.cbegin(), tags.cend(), [&look_for](const string& tag) {return tag == look_for; });
}

bool GroupDefinition::FulfillsCondition(const std::vector<FightDataCondition>&  cond) const {
	return !any_of(cond.cbegin(), cond.cend(), [this](const FightDataCondition& d) {return !FulfillsCondition(d); });
}

bool GroupDefinition::FulfillsCondition(const FightDataCondition& cond) const {
	if (cond.c_type == cond.AND) {
		return FulfillsCondition(cond.sub_conditions);
	}
	if (cond.c_type == cond.OR) {
		for (auto it = cond.sub_conditions.begin(); it != cond.sub_conditions.end(); ++it) {
			if (FulfillsCondition(*it) == true) { return true; }
		}
		return false;
	}
	if (cond.c_type == cond.NOT) { //One sub condition true -> construct false!
		for (auto it = cond.sub_conditions.begin(); it != cond.sub_conditions.end(); ++it) {
			if (FulfillsCondition(*it) == true) { return false; }
		}
		return true;
	}


	//Value based
	if (cond.c_type == cond.Difficulty) {
		return ValueCondition(cond,power_rating);
	}

	if (cond.c_type == cond.ExactName) {
		//cout << internal_name << " vs " << cond.string_values[0] << endl;
		return (internal_name == cond.string_values[0]);
	}

	if (cond.c_type == cond.HasTag) { //Slow, but then again this is called only on monster spawn, so whatever
		string tag = cond.string_values[0];
		for (auto it = tags.begin(); it != tags.end(); ++it) {
			if (*it == tag) { 
				return true; 
			}
		}
		return false;
	}


	Log("TODO", intToString(cond.c_type) + "not a valid condition" );
	return false;
}


//If this ever gets used somewhere else consider making it a function of the Condition object itself
bool GroupDefinition::ValueCondition(const FightDataCondition&  cond,float value) const {
	//cout <<"v "<< value << endl;
	//cout << "c " << cond.values[0] << endl;
	//Value operation
	if (cond.c_mod == cond.Less) {
		return (value <	cond.values[0]);
	}
	if (cond.c_mod == cond.LessOrEqual) {
		return (value <= cond.values[0]);
	}
	if (cond.c_mod == cond.More) {
		return (value >	cond.values[0]);
	}
	if (cond.c_mod == cond.MoreOrEqual) {
		return (value >= cond.values[0]);
	}
	if (cond.c_mod == cond.Equal) { //TODO: Small tolerance!
		//Log("TODO", "Group Definition Cond.Equal: Tolerance!");
		return (value == cond.values[0]);
	}
	if (cond.c_mod == cond.Inequal) { //TODO: Small tolerance!
		//Log("TODO", "Group Definition Cond.Inequal: Tolerance!");
		return (value != cond.values[0]);
	}
	Log("Error", "Invalid value condition");
	return false;
}