#include "UiElement.h"
#include "../OpenGLContainer.h"

#include "../Log.h"

using namespace std;
using namespace tinyxml2;

UiElement::UiElement() 
	:marked_for_cleanup(true),hidden(true),transform(),type("Empty"),file_name("Empty"),parameters()
	,elements2D(),sub_elements()
	,layer(-1),intercept_mouse(false),effects()
{

};

UiElement::UiElement(std::string file_name_, UiElementParameter param)
	:marked_for_cleanup(false), hidden(false), transform(), type("Empty"), file_name(file_name_), parameters()
	, elements2D(), sub_elements()
	, layer(-1), intercept_mouse(false), effects()
{

	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {


		XMLElement* xml_root = doc.FirstChildElement("UiElement");
		if (xml_root != 0) {
			InitializeFromXml(xml_root, param);
		} else {
			Log("Error(Data)", file_name + "Is found but contains no UiElement");
			return;
		}


	} else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}
};



UiElement::UiElement(tinyxml2::XMLElement * xml_root, UiElementParameter param)
	:marked_for_cleanup(false), hidden(false), transform(), type("Empty"), file_name(), parameters(param)
	, elements2D(), sub_elements()
	, layer(-1), intercept_mouse(false), effects()
{
	InitializeFromXml(xml_root, param);

};



UiElement::UiElement(Transform2D transform_, std::string text, float font_size) 
	:marked_for_cleanup(false), hidden(false), transform(), type("Empty"), parameters()
	, elements2D(), sub_elements()
	, layer(-1), intercept_mouse(false), effects() 
{

};

bool UiElement::RemakeIfDifferent(std::string file_name_new, UiElementParameter param) {
	if (param == parameters && file_name == file_name_new) {//We don't need to remake if the parameters are the same
		return false; //Was not remade
	}

	file_name = file_name_new;
	marked_for_cleanup = false;
	hidden = false;
	//Keep transform or not?
	parameters = param;
	elements2D.clear();
	sub_elements.clear();

	//Re load with the new parameters
	XMLDocument doc;
	doc.LoadFile(file_name.c_str());
	if (doc.ErrorID() == 0) {
		XMLElement* xml_root = doc.FirstChildElement("UiElement");
		if (xml_root != 0) {
			InitializeFromXml(xml_root, param);
		}
		else {
			Log("Error(Data)", file_name + "Is found but contains no UiElement");
			return false;
		}
	}
	else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << file_name << endl;
	}


	return true; //Was remade
};


void UiElement::InitializeFromXml(tinyxml2::XMLElement * xml_root, UiElementParameter param) {

	{ //Look up internal name
		XMLElement * xml_child = xml_root->FirstChildElement("Type");
		if (xml_child != nullptr) {
			type = xml_child->GetText();
		}
	}

	{ //Look up transform
		XMLElement * xml_child = xml_root->FirstChildElement("Transform2D");
		if (xml_child != nullptr) {
			transform = Transform2D(xml_child, param);
		}
	}


	{ //Look up an image
		XMLElement * xml_child = xml_root->FirstChildElement("Image");
		while (xml_child != nullptr) {
			elements2D.push_back(Instance2D(xml_child));
			xml_child = xml_child->NextSiblingElement("Image");
		}
	}



	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("TextField");
		while (xml_child != nullptr) {

			string text;
			XMLElement * xml_child_text = xml_child->FirstChildElement();
			if (xml_child_text != nullptr) {
				text = xml_child_text->GetText();
			}

			int nr = 0;
			XMLElement * xml_child_nr = xml_child->FirstChildElement("StringParameterIndex");
			if (xml_child_nr != nullptr) {
				nr = stoi(xml_child_nr->GetText());
				text = param.String_Value(nr);
			}
			

			Transform2D transform(0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			{ //Look up transform
				XMLElement * xml_child_tf = xml_child->FirstChildElement("Transform2D");
				if (xml_child_tf != nullptr) {
					transform = Transform2D(xml_child_tf,param);
				}
			}

			OffsetElement offset;
			{
				XMLElement * xml_child_2 = xml_child->FirstChildElement("Offset");
				if (xml_child_2 != nullptr) {
					offset = OffsetElement(xml_child_2);
				}
			}

			float max_box_size_x = -1;
			{
				XMLElement * xml_child_2 = xml_child->FirstChildElement("MaxBoxSizeX");
				if (xml_child_2 != nullptr) {
					max_box_size_x = stof(xml_child_2->GetText());
				}
			}


			Instance2D temp(0, text, transform,offset,max_box_size_x);
			elements2D.push_back(temp);

			xml_child = xml_child->NextSiblingElement("TextField");
		}
	}
	



	{
		XMLElement * xml_child = xml_root->FirstChildElement("UiElement");
		while (xml_child != nullptr) {
			sub_elements.emplace_back(make_unique<UiElement>(xml_child,param));

			xml_child = xml_child->NextSiblingElement("UiElement");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Effect");
		while (xml_child != nullptr) {
			effects.push_back(UiElementEffect(xml_child,param));
			intercept_mouse = true;
			xml_child = xml_child->NextSiblingElement("Effect");
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Conditional");
		while (xml_child != nullptr) {	
			bool res = param.EvaluateCondition(xml_child);

			if (res) {
				InitializeFromXml(xml_child, param);
			}
			xml_child = xml_child->NextSiblingElement("Conditional");
		}
	}

};

void UiElement::Render(glm::mat4 parent_matrix,OpenGLContainer* open_gl) const{
	//Log("TODO", "Render is not yet implemented for UiElement");
	//TODO: Parent matrix not yet used. Only positions are needed from there, not the scaling because that results in weirdness

	auto my_matrix = parent_matrix * transform.MakeModelMatrix(open_gl);
	for (auto it = elements2D.begin(); it != elements2D.end(); ++it) {
		it->Render(my_matrix,open_gl);
	}

	my_matrix = parent_matrix * transform.MakeTranslationMatrix(); //Children only inherit the translation, not the scale
	//Render sub elements over out own images!
	for (auto it = sub_elements.begin(); it != sub_elements.end(); ++it) {
		(*it)->Render(my_matrix, open_gl);
	}

};


vector<UiElementEffect> UiElement::CheckInterception(double modified_mx, double modified_my, int mode, OpenGLContainer* open_gl) const{
	modified_mx -= transform.X();//Accomodate for the shift!
	modified_my -= transform.Y();

	for (unsigned int i = 0; i < sub_elements.size(); i++) {
		vector<UiElementEffect> res = sub_elements[i]->CheckInterception(modified_mx, modified_my, mode, open_gl);
		if (res.size()>0) {
			res[0].trace.push_back(i);
			return res;
		}
	}


	//Check self last
	if (CheckHitBox(modified_mx, modified_my, open_gl)) {
		vector<UiElementEffect> a;
		for (auto it = effects.cbegin(); it != effects.cend(); ++it) {
			if (it->for_mode == mode) {	//Collect all the entries for this mode
				a.push_back(*it);
			}
		}

		return a;
	}

	return vector<UiElementEffect>();
};

bool UiElement::CheckHitBox(double mx, double my, OpenGLContainer * open_gl) const{
	if ((intercept_mouse == false && effects.size() == 0) || hidden == true || marked_for_cleanup == true) { return false; }

	if (mx >= - transform.ScaleX() && mx <=  + transform.ScaleX()) {
		if (my >= - transform.ScaleYModded(open_gl) && my <=  transform.ScaleYModded(open_gl)) {
			//cout << "(Hit Box)" <<endl;
			return true;
		}
	}

	return false;
};
