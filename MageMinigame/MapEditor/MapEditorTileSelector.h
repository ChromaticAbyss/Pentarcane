#pragma once

#include "../LabyrinthPartInstance.h"
#include <memory>
#include <vector>
#include <string>

#include "../Ui/UiElement.h"


class LabyrinthPartDefinition_Manager;
class OpenGLContainer;


class MapEditorTileSelector {
public:
	MapEditorTileSelector(OpenGLContainer* open_gl_,LabyrinthPartDefinition_Manager* part_def_manager_);

	void Update();//Updates the selection of avaliable tiles with the filters
	std::string Run();
	std::string Logic(float, float);


	glm::mat4 GetViewMatrixForEditor() const;
	void whereDoesMouseRayCutGround(float& cutX, float& cutY);

private:
	void Render();
	void RenderUi();


	std::vector<std::string> filter_yes;
	std::vector<std::string> filter_no;
	std::vector<std::string> filter_cand;
	UiElement filter_ui_yes;
	UiElement filter_ui_no;
	void RemakeUi();


	std::vector<LabyrinthPartInstance> representatives;

	LabyrinthPartDefinition_Manager* part_def_manager;

	OpenGLContainer* open_gl;
};