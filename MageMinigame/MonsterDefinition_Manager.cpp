#include "MonsterDefinition_Manager.h"

#include "Log.h"

#include "tinyxml2.h"
using namespace tinyxml2;

using namespace std;

MonsterDefinition_Manager::MonsterDefinition_Manager(vector<string> files)
	:definitions()
{
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
		XMLDocument doc;
		doc.LoadFile(it->c_str());
		if (doc.ErrorID() == 0) {
			//Log a message, if loading crashes due to a malformed file we can trace it
			string a = it->c_str();
			Log("Data", "Reading file "+ a );

			XMLElement* p_monster_definitions = doc.FirstChildElement("MonsterDefinitions");
			if(p_monster_definitions != 0){

				//Now we loop over the actual monster definitions contained until we run out
				XMLElement* p_monster_definition = p_monster_definitions->FirstChildElement("MonsterDefinition");
				while (p_monster_definition != 0) {
					definitions.push_back(MonsterDefinition(p_monster_definition));

					p_monster_definition = p_monster_definition->NextSiblingElement("MonsterDefinition");
				}

			}
			else {
				Log("Error", "A file sent to MonsterDefinition_Manager contains no monster data");
			}
		}
		else {
			cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: "<<it->c_str() << endl;
		}
		//doc.Clear();
	}

};


Monster MonsterDefinition_Manager::MakeMonster(std::string name, const Transform2D& tr){
	for(auto it = definitions.begin(); it != definitions.end(); ++it){
		if(it->GiveName() == name){
			it->ReadyForInstantiation();

			return Monster(tr,it._Ptr);//Since MonsterDefinition vektor does not change during the game, the pointer each instance takes can never become invalid!

		}
	}

	//Nothing found? Return empty Monster
	//Log() the problem
	Log("Debug","Monster Definition not found! internal_name: "+name);
	return Monster();
};