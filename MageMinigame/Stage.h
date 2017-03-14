#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Ui/Transform2D.h"
#include "Definition3D.h"
#include "Instance3D_New.h"
#include "Ui/Element2D.h"

class OpenGlContainer;


class Stage {
public:
	explicit Stage();
	explicit Stage(std::string);

	Transform2D MonsterPosition(int);

	void Render(OpenGLContainer* open_gl) const;

	void RenderBackground(OpenGLContainer*) const;

private:
	//std::vector<std::shared_ptr<Definition3D>> model_defs;
	std::vector<Instance3D_New> model_instances;
	
	Instance2D background;

	std::vector<Transform2D> monster_positions;
};