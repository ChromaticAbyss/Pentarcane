#include "UiAnimation.h"
#include "../OpenGLContainer.h"

#include "../Log.h"



using namespace std;
using namespace tinyxml2;

UiAnimation::UiAnimation()
	:Animation(),file_name("../Data/Ui/UiAnimationTest.xml"),position_x(0.0f),position_y(0.0f),ui_element()
{
	cout << "UiAnimation constructor" << endl;
	RebuildUiElement();
	cout << "out" << endl;
}

UiAnimation::UiAnimation(std::string name_of_animation)
	:Animation(), file_name(name_of_animation), position_x(0.0f), position_y(0.0f), ui_element()
{
	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {


		XMLElement* xml_root = doc.FirstChildElement("UiAnimation");
		if (xml_root != 0) {
			Animation::InitializeFromXml(xml_root);
			InitializeFromXml(xml_root);
		}
		else {
			Log("Error(Data)", file_name + " is found but contains no UiElement (UiAnimation)");
			return;
		}


	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}


}

bool UiAnimation::Progress()
{
	bool res = DefaultProgress();
	if (res == false) { //Animation still going
		RebuildUiElement();
	}


	return res;
}

void UiAnimation::Render(OpenGLContainer* open_gl) const
{

	if(Running()){
		ui_element.Render(glm::mat4(), open_gl);
	}
}

void UiAnimation::InitializeFromXml(tinyxml2::XMLElement * xml_root)
{
	Animation::InitializeFromXml(xml_root); //Everything for the base class it can grab by itself
	{
		XMLElement * xml_child = xml_root->FirstChildElement("FileName");
		if (xml_child != 0) {
			file_name = xml_child->GetText();
		}
	}

	//TODO: Maybe this contains instructions for how to compile parameters? 

	RebuildUiElement();
}

void UiAnimation::RebuildUiElement()
{
	std::vector<int> a;
	a.push_back(Frame());  //int parameter[0]: Which frame are we in
	std::vector<float> b;
	b.push_back(0.0f); // float p[0]: position x
	b.push_back(0.0f); // float p[1]: position y
	if (MaxFrame() > 0) {
		b.push_back(Frame() / (float)MaxFrame()); //[2] animation progress
		b.push_back(1 - Frame() / (float)MaxFrame()); //[3] animation progress reverse
	} else {
		b.push_back(0);
		b.push_back(0);
	}


	std::vector<string> c;

	UiElementParameter new_para(a, b, c);
	ui_element.RemakeIfDifferent("../Data/Ui/UiAnimationTest.xml", new_para);
}
