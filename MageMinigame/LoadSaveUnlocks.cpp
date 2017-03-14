#include "LoadSaveUnlocks.h"

#include "tinyxml2.h"

#include "Log.h"

using namespace std;
using namespace tinyxml2;

std::vector<std::string> LoadUnlocks(const std::string& file_name)
{
	std::vector<std::string> temp;



	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {

		XMLElement* xml_root = doc.FirstChildElement("Unlocks");
		if (xml_root != 0) {
			

			{
				XMLElement * xml_child = xml_root->FirstChildElement("Unlock");
				while (xml_child != 0) {
					temp.push_back(xml_child->GetText());
					xml_child = xml_child->NextSiblingElement("Unlock");
				}//while children are found
			}

		}//xml_root found
	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}



	return temp;
}

void SaveUnlocks(const std::string& file_name, const std::vector<std::string>& unlocks)
{

	XMLDocument doc;
	//doc.NewDeclaration();

	XMLNode* root = doc.NewElement("Unlocks");
	doc.InsertFirstChild(root);


	for (auto it = unlocks.cbegin(); it != unlocks.cend(); ++it) {
		XMLElement * unlock_element = doc.NewElement("Unlock");
		unlock_element->SetText(it->c_str());
		root->InsertEndChild(unlock_element);
	}


	doc.SaveFile(file_name.c_str());


}
