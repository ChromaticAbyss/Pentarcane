#pragma once

#include "Labyrinth/LabyrinthPartInstance.h"
#include <memory>
#include <vector>
#include <string>

#include "PolymorphicUiElement.h"


class LabyrinthPartDefinition_Manager;
class OpenGLContainerWith3D;


class MapEditorTileSelector {
public:
	MapEditorTileSelector(OpenGLContainerWith3D* open_gl_,LabyrinthPartDefinition_Manager* part_def_manager_, bool active_parts_ = true);

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
	std::unique_ptr<PolymorphicUiElement> filter_ui_yes;
	std::unique_ptr<PolymorphicUiElement> filter_ui_no;
	void RemakeUi();


	std::vector<LabyrinthPartInstance> representatives;

	LabyrinthPartDefinition_Manager* part_def_manager;

	OpenGLContainerWith3D* open_gl;

	bool active_parts;
};