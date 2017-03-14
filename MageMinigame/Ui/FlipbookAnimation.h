#pragma once


#include "Animation.h"

class OpenGLContainer;

#include "../Ui/UiElement.h"
#include <string>
#include <vector>

#include "../tinyxml2.h"

class FlipbookAnimation : public Animation {
public:
	explicit FlipbookAnimation();

	explicit FlipbookAnimation(std::string file_name, Transform2D transf_);

	virtual bool Progress() override;

	virtual void Render(OpenGLContainer*) const;

private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);

	//Data members

	Transform2D transf;
	std::vector<Instance2D> images;

};