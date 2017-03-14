#include "LabyrinthPartDefinition_Manager.h"

#include "Log.h"

#include "tinyxml2.h"

#include "StringUtility.h"

#include <algorithm>

using namespace std;
using namespace tinyxml2;

LabyrinthPartDefinition_Manager::LabyrinthPartDefinition_Manager(vector<string> file_names)
	:definitions()
{
	for(vector<string>::iterator it = file_names.begin(); it != file_names.end(); ++it){
		XMLDocument doc;
		doc.LoadFile(it->c_str());
		if (doc.ErrorID() == 0) {
			//Log a message, if loading crashes due to a malformed file we can trace it
			string a = it->c_str();
			Log("Data", "LabyrinthPartDefinition_Manager: Reading file " + a);

			XMLElement* p_definitions = doc.FirstChildElement("LabyrinthPartDefinitions");
			if (p_definitions != 0) {

				//Now we loop over the actual monster definitions contained until we run out
				XMLElement* p_definition = p_definitions->FirstChildElement("LabyrinthPartDefinition");
				while (p_definition != 0) {
					definitions.push_back(LabyrinthPartDefinition(p_definition));
					p_definition = p_definition->NextSiblingElement("LabyrinthPartDefinition");
				}

			}
			else {
				Log("Error", "A file sent to LabyrinthPartDefinition_Manager contains no LabyrinthParts data");
			}
		}
		else {
			cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << it->c_str() << endl;
		}
		//doc.Clear();		

	}
};


LabyrinthPartInstance LabyrinthPartDefinition_Manager::GetInstance(string name){
	for(vector<LabyrinthPartDefinition>::iterator it = definitions.begin(); it != definitions.end(); ++it){
		if(it->InternalName() == name){
			it->ReadyForInstantiation();
			return LabyrinthPartInstance(Transform(),*it);
		}
	}

	Log("Error", "LabyrinthPartDefinition_Manager::GetInstance can't find part with internal_name = "+name);
	return LabyrinthPartInstance(Transform(),definitions[0]);
}

LabyrinthPartInstance LabyrinthPartDefinition_Manager::GetInstanceFromXml(tinyxml2::XMLElement * xml_root){
	string type_to_spawn = "GetInstanceFromXml: element contains no typeinfo";

	{//Get type info from the child
		XMLElement * xml_child_type = xml_root->FirstChildElement("Type");
		if (xml_child_type != 0) {
			type_to_spawn = xml_child_type->GetText();
		}
		else {
			Log("Error", "Part data incomplete, missing type");
		}
	}



	for (vector<LabyrinthPartDefinition>::iterator it = definitions.begin(); it != definitions.end(); ++it) {
		if (it->InternalName() == type_to_spawn) {
			it->ReadyForInstantiation();
			LabyrinthPartInstance tmp(xml_root, *it);
			return tmp;
		}
	}



	Log("Error", "LabyrinthPartDefinition_Manager::GetInstance can't find part with internal_name = " + type_to_spawn);
	return LabyrinthPartInstance();
};

#ifdef MAGEMINIGAME_MAPEDITOR

std::tuple<std::vector<LabyrinthPartInstance>, std::vector<std::string>> LabyrinthPartDefinition_Manager::GrabInstancesByFilter( const vector<std::string>& tags_yes, const vector<string>& tags_no){
	vector<LabyrinthPartInstance> temp;

	vector<tuple<string, int>> tags_and_counts;

	auto count_selected = 0;

	for (auto it = definitions.begin(); it != definitions.end(); ++it) {
		if (it->CanAppearInEditor()) {
			//TODO: Check for the tag condition!
			auto cond = true;
			//For the entire yes-list check each tag
			for (auto it_ty = tags_yes.cbegin(); it_ty!=tags_yes.cend();++it_ty)
			{
				if (it->HasTag(*it_ty) == false) {
					cond = false;
					break;
				}
			}
			for (auto it_tn = tags_no.cbegin(); it_tn != tags_no.cend(); ++it_tn)
			{
				if (it->HasTag(*it_tn) == true) {
					cond = false;
					break;
				}
			}

			if (cond) {
				count_selected++;
				//Grab all tags and count them THIS MAY BE TERRIBLY SLOW
				auto tags = it->Tags();
				for (size_t ii = 0; ii < tags.size(); ii++)
				{ //For each tag we have here

					auto found = false;
					for (size_t jj = 0; jj < tags_and_counts.size(); jj++)
					{
						if (tags[ii] == std::get<0>(tags_and_counts[jj])) {
							found = true;
							std::get<1>(tags_and_counts[jj])++;
							break;
						}

					}
					if (!found) { 
						tags_and_counts.push_back(make_tuple(tags[ii], 1));
					}


				}//For each tag we have here


				//Actually do stuff with the objects
				it->ReadyForInstantiation();
				temp.push_back(LabyrinthPartInstance(Transform(), *it));
			}
		}
	};




	//Sort by distance of counts from halfway point
	std::sort(tags_and_counts.begin(), tags_and_counts.end(), 
		[count_selected](tuple<string,int> el1, tuple<string, int> el2) { //Lambda
			//int dist1 = std::get<1>(el1); //This doesn't work? C++11 VS c++14?
			//if (dist1 < 0) 	dist1 = -dist1;
			//int dist2 = std::get<1>(el2);
			//if (dist2 < 0) 	dist2 = -dist2;
			return (abs(count_selected/2 - std::get<1>(el1)) < abs(count_selected/2 - std::get<1>(el2)));
		}
	);


	vector<string> new_candidate_tags;
	auto k = 0;
	for (size_t i = 0; i < tags_and_counts.size(); i++)
	{
		//cout << std::get<0>(tags_and_counts[i]) << "|" << std::get<1>(tags_and_counts[i]) << endl;
		if (k<6) //up to 7 values should be enough to choose from
		{
			new_candidate_tags.push_back(std::get<0>(tags_and_counts[i]));
		}
		k++;
	}


	return make_tuple(temp, new_candidate_tags);
};

#endif