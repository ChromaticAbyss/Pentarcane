#include "Sequence.h"

#include "Log.h"

#include "tinyxml2.h"

#include "PlayerState.h"
#include "Fight.h"

using namespace std;
using namespace tinyxml2;

#include "IrrklangSingleton.h"


Sequence::Sequence(std::string file_name) 
	:progress(0),sequence_modifiers(),groups_we_already_had(),sound_file("../Data/Sound/Music/Myst on the Moor.mp3")
{
	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {

		XMLElement* xml_root = doc.FirstChildElement("Sequence");
		if (xml_root != 0) {
		
			{
				XMLElement * xml_child = xml_root->FirstChildElement("Fight");
				while(xml_child != nullptr) {
					fight_data.emplace_back(xml_child);
					xml_child = xml_child->NextSiblingElement("Fight");
				}
			}


			{ //Bonus stats for all monsters in this sequence
				XMLElement * xml_child = xml_root->FirstChildElement("SequenceModifiers");
				if(xml_child != nullptr) {
					sequence_modifiers = Stats(xml_child);
				}
			}

			{ //Bonus stats for all monsters in this sequence
				XMLElement * xml_child = xml_root->FirstChildElement("Soundtrack");
				if (xml_child != nullptr) {
					sound_file = (xml_child->GetText());
					sound_file = "../Data/Sounds/Music/" + sound_file;
				}
			}

		}
		else {
			Log("Error(Data)", file_name + " is found but contains no Sequence");
			return;
		}

	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}


};



bool Sequence::Resolve(OpenGLContainer* open_gl, std::shared_ptr<GroupDefinition_Manager> group_def_manager, std::shared_ptr<MonsterDefinition_Manager> monster_def_manager, LabyrinthPartDefinition_Manager* part_def_manager){
	irrklang::ISound* music = IrrklangSingleton().Get()->play2D(sound_file.c_str(), true, false, true);

	PlayerState player_state;//Start a sequence at full stats

	if (fight_data.size() <= 0) {
		Log("Error", "Sequence has no entries!");
	}

	while (player_state.HitPoints() > 0) {

		//It is now the sequences job to grab the monster group
		vector<Monster> monsters;
		vector<string> monster_names_for_this_fight = group_def_manager->GrabFittingGroup(fight_data[progress].encounter_conditions,groups_we_already_had);

		groups_we_already_had.push_back(monster_names_for_this_fight[monster_names_for_this_fight.size() - 1]);
		monster_names_for_this_fight.pop_back();

		int a = 0;
		for (vector<string>::iterator it = monster_names_for_this_fight.begin(); it < monster_names_for_this_fight.end(); it++) {
			auto temp = monster_def_manager->MakeMonster(*it, Transform2D());
			//temp.ApplyBonusStats(fight_data.);
			monsters.push_back(temp);		
			a++;
		}


		Fight round(open_gl,player_state, group_def_manager,monster_def_manager,part_def_manager, fight_data[progress],monsters);
		player_state = round.Resolve();

		progress++;
		if (progress >= fight_data.size()) { break; }
	}

	IrrklangSingleton().Get()->removeAllSoundSources();

	if (player_state.HitPoints() > 0) {
		Log("TODO", "Sequence successfully beaten!");
		return true;
	}

	Log("TODO", "Player Dead/Sequence Aborted");
	return false;
}

