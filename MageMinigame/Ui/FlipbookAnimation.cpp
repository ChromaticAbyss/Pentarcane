#include "FlipbookAnimation.h"

#include "../Log.h"

using namespace std;
using namespace tinyxml2;

FlipbookAnimation::FlipbookAnimation()
	:Animation(),transf(),images()
{
}


FlipbookAnimation::FlipbookAnimation(std::string file_name, Transform2D transf_)
	:Animation(), transf(transf_),images()
{

	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {

		XMLElement* xml_root = doc.FirstChildElement("FlipbookAnimation");
		if (xml_root != nullptr) {
			Animation::InitializeFromXml(xml_root);
			InitializeFromXml(xml_root);
		}
		else {
			Log("Error(Data)", file_name + " is found but contains no FlipbookAnimation");
			return;
		}

	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}

}

bool FlipbookAnimation::Progress()
{
	bool res = DefaultProgress();
	if (res == false) { //Animation still going
		//RebuildUiElement();
	}

	return res;
}

void FlipbookAnimation::Render(OpenGLContainer * open_gl) const
{

	int id = images.size() * Frame() / MaxFrame();
	if (id >= images.size()) { 
		return; //Problems with MaxFrame
	}


	images[id].Render(transf.MakeModelMatrix(open_gl),open_gl);
}

void FlipbookAnimation::InitializeFromXml(tinyxml2::XMLElement * xml_root)
{
	Animation::InitializeFromXml(xml_root); //Everything for the base class it can grab by itself

	{
		XMLElement * xml_child = xml_root->FirstChildElement("Transform2D");
		if (xml_child != nullptr) {
			Transform2D transf_temp = Transform2D(xml_child, UiElementParameter());
			transf = transf.Merge(transf_temp);
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("FileName");
		while (xml_child != nullptr) {
			string file_name = xml_child->GetText();

			images.emplace_back(Instance2D(file_name,Transform2D()));
			xml_child = xml_child->NextSiblingElement("FileName");
		}
	}

}
