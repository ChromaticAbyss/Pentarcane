#include "Stage.h"
#include "tinyxml2.h"
#include "Log.h"

#include "OpenGLContainer.h"

using namespace std;
using namespace tinyxml2;

Stage::Stage() 
	:background(),monster_positions()
{
}


Stage::Stage(string filename)
	:background(),monster_positions()
{
	XMLDocument doc;
	doc.LoadFile(filename.c_str());
	if (doc.ErrorID() == 0) {
		XMLElement* xml_root = doc.FirstChildElement("Stage");
		if (xml_root != 0) {
			{ //Look up a model in the node
				XMLElement * xml_child = xml_root->FirstChildElement("MonsterPosition");
				while (xml_child != nullptr) {
					monster_positions.push_back(Transform2D(xml_child,UiElementParameter()));
					xml_child = xml_child->NextSiblingElement("MonsterPosition");
				}
			}

			{ //Look up a model in the node
				XMLElement * xml_child = xml_root ->FirstChildElement("Art3D");
				while (xml_child != nullptr) {
					shared_ptr<Definition3D> temp = make_shared<Definition3D>(xml_child);
					temp->attemptToLoadModel();
					model_instances.push_back( Instance3D_New(temp));
					xml_child = xml_child->NextSiblingElement("Art3D");
				}
			}

			{ 
				XMLElement * xml_child = xml_root->FirstChildElement("Background");
				if (xml_child != nullptr) {
					auto offset_for_sidebar = 0.15f;
					auto text = xml_child->GetText();
					background = Instance2D(text,Transform2D(-offset_for_sidebar ,0.0f , 1.0f-offset_for_sidebar, 1.0f, false));
				}
			}


		}//Has correct root
	}else {
		cout << "XML Error ID(0=okay): " << doc.ErrorID() << " \t Filename: " << filename << endl;
	}
}


Transform2D Stage::MonsterPosition(int id) {
	if (id < 0 || id >= monster_positions.size()) {
		return Transform2D();
	}
	return monster_positions[id];
};

void Stage::Render(OpenGLContainer* open_gl) const {
	for (auto it = model_instances.cbegin(); it != model_instances.cend(); ++it) {
		it->Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f,1.0f)),open_gl);
	}
}

void Stage::RenderBackground(OpenGLContainer * open_gl) const
{
	background.Render(glm::mat4(),open_gl);
}
;