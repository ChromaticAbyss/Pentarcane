#include "MonsterDefinition.h"

#include "Log.h"


using namespace tinyxml2;

using namespace std;

MonsterDefinition::MonsterDefinition()
	:stats(1,1,0,0),internal_name("DefaultMonsterDefinition"),display_name("DefaultMonsterDefinition"),casts(),labyrinths(),model_loaded(false),image_name()
	,default_attack_animation("../Data/Animations/Attack_Bite.xml")
{
};


MonsterDefinition::MonsterDefinition(XMLElement* p_monster_definition)
	:stats(1, 1, 0, 0), internal_name("DefaultMonsterDefinition"), display_name("DefaultMonsterDefinition"), casts(), labyrinths(),model_loaded(false),image_name()
	,default_attack_animation("../Data/Animations/Attack_Bite.xml")
{
	if (p_monster_definition == 0) {
		return; //If we somehow got a nullpointer we can't extract any information and need to return
	}
	internal_name = (p_monster_definition->FirstChildElement("InternalName"))->GetText();
	Log("Data", "Loading MonsterDefinition with internalname: " + internal_name);

	stats = Stats(p_monster_definition->FirstChildElement("Stats"));

	{ //Display Name
		XMLElement * xml_child = p_monster_definition->FirstChildElement("Name");
		if (xml_child != nullptr) {
			display_name = xml_child->GetText();
		}
	}



	{
		XMLElement* p_lab_entry = p_monster_definition->FirstChildElement("Labyrinth");
		while (p_lab_entry != nullptr) {
			labyrinths.push_back(p_lab_entry->GetText());
			//cout << "Labyrinth: " << labyrinths[labyrinths.size() - 1] << endl;
			p_lab_entry = p_lab_entry->NextSiblingElement("Labyrinth");
		}
	}

	
	{
		XMLElement* p_cast_entry = p_monster_definition->FirstChildElement("Cast");
		while (p_cast_entry != nullptr) {
			casts.emplace_back(p_cast_entry);
			p_cast_entry = p_cast_entry->NextSiblingElement("Cast");
		}
	}
	
	{
		XMLElement* p_cast_entry = p_monster_definition->FirstChildElement("CastWhenDamageTaken");
		while (p_cast_entry != nullptr) {
			cast_when_damage_taken.emplace_back(p_cast_entry);
			p_cast_entry = p_cast_entry->NextSiblingElement("CastWhenDamageTaken");
		}
	}

	{ //Look up a model in the node
		XMLElement * xml_child = p_monster_definition->FirstChildElement("Art3D");
		if (xml_child != nullptr) {
			model_def = make_shared<Definition3D>(xml_child);
		} else {
			model_def = make_shared<Definition3D>();
		}
	}


	//Image things
	{
		XMLElement* p_child = p_monster_definition->FirstChildElement("Image");
		if (p_child != nullptr) {
			image_name = p_child->GetText();
		}
	}
	{ 
		XMLElement * xml_child = p_monster_definition->FirstChildElement("ImageTransform");
		if (xml_child != nullptr) {
			image_transform = Transform2D(xml_child,UiElementParameter());
		}
	}


	{
		XMLElement* p_child = p_monster_definition->FirstChildElement("DefaultAttackAnimation");
		if (p_child != nullptr) {
			default_attack_animation = p_child->GetText();
		}
	}


	//TODO: Add an "image transform" field


	Log("Data", "Done loading MonsterDefinition with internalname: " + internal_name);
};


std::string MonsterDefinition::RandomLabyrinth() const {
	int s = labyrinths.size();
	int r = rand() % s;
	return labyrinths[r];
}
std::vector<MonsterCast> MonsterDefinition::GiveCastWhenDamageTaken() const
{
	int amo = cast_when_damage_taken.size();
	if (amo > 0) {
		int b = rand() % amo;
		vector<MonsterCast> res;
		res.push_back(cast_when_damage_taken[b]);
		return res;
	}

	return vector<MonsterCast>();
}
;

void MonsterDefinition::ReadyForInstantiation() {
	if (model_loaded == false) {
		model_def->attemptToLoadModel();
		model_loaded = true;
	}
};