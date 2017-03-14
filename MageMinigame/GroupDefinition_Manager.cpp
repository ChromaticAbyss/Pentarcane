#include "GroupDefinition_Manager.h"
#include "Log.h"
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;



GroupDefinition_Manager::GroupDefinition_Manager(std::vector<std::string> files)
	:groups()
{
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
		XMLDocument doc;
		doc.LoadFile(it->c_str());

		if (doc.ErrorID() == 0) {
			//Log a message, if loading crashes due to a malformed file we can trace it
			string a = it->c_str();
			Log("Data", "Reading file " + a);

			XMLElement* p_definitions_root = doc.FirstChildElement("GroupDefinitions");
			if (p_definitions_root != 0) {
				//Now we loop over the actual monster definitions contained until we run out
				XMLElement* p_definition = p_definitions_root->FirstChildElement("GroupDefinition");
				while (p_definition != 0) {
					groups.push_back(GroupDefinition(p_definition));
					p_definition = p_definition->NextSiblingElement("GroupDefinition");
				}
			}
			else {
				Log("Error", "A file sent to GroupDefinition_Manager contains no group data");
			}
		}
		else {
			cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << it->c_str() << endl;
		}
		//doc.Clear();
	}
}

//Note never make a condition that grabs multiple 0- weight and 1 or more positive weight groups, in that case only the positive weight is picked.ma
std::vector<std::string> GroupDefinition_Manager::GrabFittingGroup(vector<FightDataCondition> conditions, vector<string> dont_use_again) {
	//List all relevant groups and add up their weights, keep track where it belongs too
	int weight_sum = 0;
	vector<int> weights;
	vector<int> pos;
	int ii = 0;
	for (auto it = groups.begin(); it != groups.end(); ++it) {
		bool blocked = false;//Blocks groups from appearing more than once in each sequence
		string name = it->InternalName();
		for(auto it2 = dont_use_again.cbegin(); it2 != dont_use_again.cend(); ++it2){
			if (*it2 == name) { 
				blocked = true; 
			}
		}

		if(it->FulfillsCondition(conditions) && blocked==false){
			//cout << "Condition fulfilled for " << it->InternalName() << endl;
			weight_sum += it->Likelihood();
			weights.push_back(weight_sum);
			pos.push_back(ii);
		}
		ii++;
	}
	if (weights.size() == 0) {
		Log("Error", "No Groups fit condition!");
		return vector<string>();
	}
	if (weight_sum == 0) {
		Log("Debug", "All entries have weight zero, chosing with equal weight (most likely a bossfight)");
		int random_nr = rand() % weights.size();
		int actual_group_pos = pos[random_nr];
		return groups[actual_group_pos].MonsterNames();;
	}

	//generate a completely random number between 0 and weight_sum and use that to determine a fitting position
	int totally_random_number = rand() % weight_sum;
	int jj = 0;
	for (vector<int>::iterator it = weights.begin(); it != weights.end(); ++it) {
		if (*it > totally_random_number) {
			break;
		}
		++jj;
	}
	int actual_group_pos = pos[jj];

	auto temp = groups[actual_group_pos].MonsterNames();
	temp.push_back(groups[actual_group_pos].InternalName()); //Last entry in the vector is the group internal name!

	//Get the group to give us the list of monster names we want
	return temp;
};