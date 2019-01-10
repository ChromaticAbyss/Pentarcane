#include "Stage.h"
#include "tinyxml2.h"
#include "Log.h"

#include "OpenGLContainer.h"
#include "Log.h"
#include "Utility\ParseTransform.h"
#include "BasicUiElement.h"

using namespace std;
;

Stage::Stage() 
	:background(),monster_positions()
{
}


Stage::Stage(string filename)
	:background(),monster_positions()
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	if (doc.ErrorID() == 0) {
		tinyxml2::XMLElement* xml_root = doc.FirstChildElement("Stage");
		if (xml_root != 0) {
			{ //Look up a model in the node
				tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("MonsterPosition");
				while (xml_child != nullptr) {
					monster_positions.push_back(ParseTransform(xml_child));
					xml_child = xml_child->NextSiblingElement("MonsterPosition");
				}
			}

			{ 
				tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Background");
				if (xml_child != nullptr) {
					auto text = xml_child->GetText();
					background = BasicUiElement::Make_BasicUiElement_UP(
						Transform2D(
							TransformPosition(0.0f, 0.0f),
							TransformScale(2.0f, 2.0f)
						)
						, text
					);

				}
			}


		}//Has correct root
	}else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << filename << endl;
	}
}


Transform2D Stage::MonsterPosition(int id) const{
	if (id < 0 || id >= monster_positions.size()) {
		return Transform2D();
	}
	return monster_positions[id];
};

void Stage::Render(OpenGLContainer* open_gl) const {
	//for (auto it = model_instances.cbegin(); it != model_instances.cend(); ++it) {
	//	it->Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f,1.0f)),open_gl);
	//}
	Log("Debug", "Stage::Render() doesn't need ot be called anymore");
}

void Stage::RenderBackground(OpenGLContainer * open_gl) const
{
	background->Render(glm::mat4(),open_gl);
}
;