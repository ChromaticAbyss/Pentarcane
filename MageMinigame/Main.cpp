#include "BuildHeader.h"

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <ctime>

#include "Log.h"
#include "OpenGLContainer.h"
#include "MonsterDefinition_Manager.h"
#include "GroupDefinition_Manager.h"
#include "LabyrinthPartDefinition_Manager.h"
#include "Sequence.h"
#include "Fight.h"
#include "LabyrinthInstance.h"
#include "Minigames.h"
#include "SequenceSelect.h"

#include "LoadSaveUnlocks.h"

#ifdef MAGEMINIGAME_MAPEDITOR
#include "MapEditor/MapEditorTileSelector.h"
#include "MapEditor/MapEditor.h"
#endif

#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;


//Irrklang
#include "IrrklangSingleton.h" 
using namespace irrklang;



int main() {
	srand(time(0));

	unique_ptr<OpenGLContainer> opengl_container = make_unique<OpenGLContainer>(); //make_unique<OpenGLContainer>(); //Works so far, opens a window!

	vector<string> monster_def_file_names;
	vector<string> group_def_file_names;
	vector<string> part_def_file_names;
	vector<string> sequence_file_names;

	XMLDocument doc;
	doc.LoadFile("../Data/Datafiles.xml");
	if (doc.ErrorID() == 0) {
		XMLElement* xml_root = doc.FirstChildElement("Datafiles");
		if (xml_root != 0) {

			{
				XMLElement * xml_child = xml_root->FirstChildElement("GroupDefinitions");
				while (xml_child != 0) {
					string a = xml_child->GetText();
					group_def_file_names.push_back("../Data/" + a);
					xml_child = xml_child->NextSiblingElement("GroupDefinitions");
				}
			}

			{
				XMLElement * xml_child = xml_root->FirstChildElement("MonsterDefinitions");
				while (xml_child != 0) {
					string a = xml_child->GetText();
					monster_def_file_names.push_back("../Data/" + a);
					xml_child = xml_child->NextSiblingElement("MonsterDefinitions");
				}
			}

			{
				XMLElement * xml_child = xml_root->FirstChildElement("LabyrinthPartDefinitions");
				while (xml_child != 0) {
					string a = xml_child->GetText();
					part_def_file_names.push_back("../Data/" + a);
					xml_child = xml_child->NextSiblingElement("LabyrinthPartDefinitions");
				}
			}


			{
				XMLElement * xml_child = xml_root->FirstChildElement("Sequence");
				while (xml_child != 0) {
					string a = xml_child->GetText();
					sequence_file_names.push_back("../Data/" + a);
					xml_child = xml_child->NextSiblingElement("Sequence");
				}
			}
			

		}
		else {
			Log("Error(Data)", "Datafiles.xml is found but contains nothing");
			return 1; //Error
		}

	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << "Datafiles.xml" << endl;
	}



#ifndef MAGEMINIGAME_MAPEDITOR


	shared_ptr<MonsterDefinition_Manager> monster_defs = make_shared<MonsterDefinition_Manager>(monster_def_file_names);

	shared_ptr<GroupDefinition_Manager> group_defs = make_shared<GroupDefinition_Manager>(group_def_file_names);

#endif 

	unique_ptr<LabyrinthPartDefinition_Manager> part_defs = make_unique<LabyrinthPartDefinition_Manager>(part_def_file_names) ;//make_unique<LabyrinthPartDefinition_Manager>(part_def_file_names);


#ifdef MAGEMINIGAME_MAPEDITOR
	MapEditor map_ed(part_defs.get());
	map_ed.Run(opengl_container.get());
	return 0;
#endif

#ifndef MAGEMINIGAME_MAPEDITOR
#ifdef Testing_Labyrinth
	while (true) {
		vector<string> laby_names;
		laby_names.push_back(TESTING_LABYRINTH_NAME);
		vector<int> monster_ids;
		monster_ids.push_back(1);
		Minigames min(laby_names, 1, monster_ids, part_defs);
		min.Resolve(opengl_container.get());
	}
#endif
#ifndef Testing_Labyrinth

	std::vector<std::string> unlocks;
	//For test purposes don't read from file but push manually here:

	unlocks = LoadUnlocks("../Data/Unlocks.xml");
	//unlocks.push_back("Mages_Certificate");

	SequenceSelect seq_sel(opengl_container.get(),sequence_file_names, unlocks);
	std::string sequence_file = seq_sel.Run();

	while(sequence_file != "") {
		cout << sequence_file << endl;
		Sequence seq(sequence_file);
		bool result = seq.Resolve(opengl_container.get(), group_defs, monster_defs, part_defs.get());
		if (result == true) {
			//Success
			SequenceRepresentation just_completed(sequence_file);
			auto rewards = just_completed.GetRewards();
			//Add all the new unlocks (but don't add things twice!
			for (auto it = rewards.cbegin(); it != rewards.cend(); it++) {
				auto res = find(unlocks.begin(), unlocks.end(), *it);
				if (res == unlocks.end()) {
					unlocks.push_back(*it);//Not found, add!
				}
			}
			SaveUnlocks("../Data/Unlocks.xml",unlocks);


			just_completed.CompleteScreen(opengl_container.get());
		}

		//Show selection again, maybe the player wants to hit another level right after?
		sequence_file = seq_sel.Run();
	}

	//Save file of unlocks


#endif
#endif 
	//system("pause");
}