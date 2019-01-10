#include "BuildHeader.h"

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <ctime>
#include "tinyxml2.h"

#include "Log.h"
#include "OpenGLContainerWith3D.h"
#include "Monster/MonsterDefinition_Manager.h"
#include "GroupDefinition_Manager.h"
#include "Labyrinth/LabyrinthPartDefinition_Manager.h"
#include "Sequence/Sequence.h"
#include "Fight/Fight.h"
#include "Labyrinth/LabyrinthInstance.h"
#include "Sequence/SequenceSelect.h"
#include "Particles/ParticleDefinition.h"
#include "LoadSaveUnlocks.h"

#include "Tests/Tests.h"

#ifdef MAGEMINIGAME_MAPEDITOR
#include "MapEditor/MapEditorTileSelector.h"
#include "MapEditor/MapEditor.h"
#endif

using namespace std;

//Irrklang
#include "IrrklangSingleton.h" 

#include "instance2dtext.h"



int main() {
	//TODO: Only run tests in debug builds
	run_pentarcane_tests();

	srand(time(0));
		unique_ptr<OpenGLContainerWith3D> opengl_container = make_unique<OpenGLContainerWith3D>(); //make_unique<OpenGLContainer>(); //Works so far, opens a window!
		Instance2DText::SetDefaultFontImage("../Data/2D/font_noborders_new.png");//Needs to be loaded after OpenGLContainer

		vector<string> monster_def_file_names;
		vector<string> group_def_file_names;
		vector<string> part_def_file_names;
		vector<string> sequence_file_names;
		vector<string> particle_file_names;

		tinyxml2::XMLDocument doc;
		doc.LoadFile("../Data/Datafiles.xml");
		if (doc.ErrorID() == 0) {
			tinyxml2::XMLElement* xml_root = doc.FirstChildElement("Datafiles");
			if (xml_root != nullptr) {

				{
					tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("GroupDefinitions");
					while (xml_child != 0) {
						string a = xml_child->GetText();
						group_def_file_names.push_back("../Data/" + a);
						xml_child = xml_child->NextSiblingElement("GroupDefinitions");
					}
				}

				{
					tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("MonsterDefinitions");
					while (xml_child != 0) {
						string a = xml_child->GetText();
						monster_def_file_names.push_back("../Data/" + a);
						xml_child = xml_child->NextSiblingElement("MonsterDefinitions");
					}
				}

				{
					tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("LabyrinthPartDefinitions");
					while (xml_child != 0) {
						std::cout << part_def_file_names.size() << "\n";
						string a = xml_child->GetText();
						part_def_file_names.push_back("../Data/" + a);
						xml_child = xml_child->NextSiblingElement("LabyrinthPartDefinitions");
					}
				}


				{
					tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Sequence");
					while (xml_child != 0) {
						string a = xml_child->GetText();
						sequence_file_names.push_back("../Data/" + a);
						xml_child = xml_child->NextSiblingElement("Sequence");
					}
				}

				{
					tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Particles");
					while (xml_child != 0) {
						string a = xml_child->GetText();
						particle_file_names.push_back("../Data/" + a);
						xml_child = xml_child->NextSiblingElement("Particles");
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

		unique_ptr<LabyrinthPartDefinition_Manager> part_defs = make_unique<LabyrinthPartDefinition_Manager>(part_def_file_names);//make_unique<LabyrinthPartDefinition_Manager>(part_def_file_names);

		for (auto pdef : particle_file_names) {
			doc.LoadFile(pdef.c_str());
			if (doc.ErrorID() == 0) {
				tinyxml2::XMLElement* xml_root = doc.FirstChildElement("Particles");
				if (xml_root != nullptr) {
					ParticleDefinition::parse(xml_root);
				}
			}
		}


#ifdef MAGEMINIGAME_MAPEDITOR
		MapEditor map_ed(part_defs.get(), opengl_container.get());
		map_ed.run();
		return 0;
#endif

#ifndef MAGEMINIGAME_MAPEDITOR

		std::vector<std::string> unlocks;
		//For test purposes don't read from file but push manually here:

		unlocks = LoadUnlocks("../Data/Unlocks.xml");
		//unlocks.push_back("Mages_Certificate");

		SequenceSelect seq_sel(opengl_container.get(), sequence_file_names, unlocks);
		std::string sequence_file = seq_sel.Run();

		while (sequence_file != "") {
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
				SaveUnlocks("../Data/Unlocks.xml", unlocks);

				just_completed.CompleteScreen(opengl_container.get());

				seq_sel.update_unlocks(sequence_file_names, unlocks);
			}

			//Show selection again, maybe the player wants to hit another level right after
			sequence_file = seq_sel.Run();
		}


#endif 

}