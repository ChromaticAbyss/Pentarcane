#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Transform2D.h"
#include "Definition3D.h"
#include "Instance3D_New.h"
#include "Instance2D.h"
#include "PolymorphicUiElement.h"

class OpenGlContainer;


class Stage {
public:
	explicit Stage();
	explicit Stage(std::string);

	Transform2D MonsterPosition(int) const;

	void Render(OpenGLContainer* open_gl) const;

	void RenderBackground(OpenGLContainer*) const;

	int NrPositions() const { return monster_positions.size(); }
private:
	//std::vector<std::shared_ptr<Definition3D>> model_defs;
	//std::vector<Instance3D_New> model_instances;
	
	std::unique_ptr<PolymorphicUiElement> background;

	std::vector<Transform2D> monster_positions;
};