#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Element2D.h"

#include "glm/glm.hpp"

#include "Transform2D.h"

#include "UiElementEffect.h"
#include "UiElementParameter.h"

#include "../tinyxml2.h"

class OpenGLContainer;

class UiElement{
public:
	UiElement();
	UiElement(std::string file_name, UiElementParameter param);
	UiElement(tinyxml2::XMLElement * xml_root,UiElementParameter param);
	UiElement(Transform2D transform_, std::string text, float font_size); //TODO: factory?


	UiElement(const UiElement&) = delete;
	UiElement(UiElement&&) = default;
	//virtual ~UiElement() {};

	bool RemakeIfDifferent(std::string file_name, UiElementParameter param);

	std::vector<UiElementEffect> CheckInterception(double, double, int, OpenGLContainer* open_gl) const;
	bool CheckHitBox(double, double, OpenGLContainer *) const; //Does the mouse hit any of the elements?

	void Render(glm::mat4,OpenGLContainer* open_gl) const;


private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root, UiElementParameter param);

	bool marked_for_cleanup;
	bool hidden;

	Transform2D transform;
	std::string type; //What type of ui element is this? TODO: THis could be done polymorphically
	std::string file_name;
	UiElementParameter parameters;

	//Instances to be displayed
	std::vector<Instance2D> elements2D; //Todo: treat them polymorphically too

	std::vector<std::unique_ptr<UiElement>> sub_elements;

	int layer;
	bool intercept_mouse; //If true it can intercept mouse hovers / collisions

	std::vector<UiElementEffect> effects; //

};


