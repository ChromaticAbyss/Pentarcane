#pragma once

#include "Animation.h"

class OpenGLContainer;

#include "../Ui/UiElement.h"
#include <string>

#include "../tinyxml2.h"

class UiAnimation : public Animation {
public:
	explicit UiAnimation();
	explicit UiAnimation(std::string);

	UiAnimation(const UiAnimation&) = delete;
	UiAnimation(UiAnimation&&) = default;

	virtual bool Progress() override;

	virtual void Render(OpenGLContainer*) const;

private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);

	void RebuildUiElement();

	//Data members
	std::string file_name;
	float position_x;
	float position_y;
	UiElement ui_element;

};