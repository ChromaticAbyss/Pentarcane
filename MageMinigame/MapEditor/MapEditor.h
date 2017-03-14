#pragma once

#include "../BuildHeader.h"

#include "../LabyrinthPartInstance.h"
#include "MapEditorTileSelector.h"
#include <memory>
#include <vector>
#include <string>

#include "VariantCarrier.h"

#include "../Ui/UiElement.h"


#include "../Key.h"

class LabyrinthPartDefinition_Manager;
class OpenGLContainer;


class MapEditor {
public:
	MapEditor(LabyrinthPartDefinition_Manager* part_def_manager_);
	~MapEditor();

	void Run(OpenGLContainer*);
	void Render(OpenGLContainer*);
	void Logic(OpenGLContainer*,float,float);
	void SaveDialogue();
	void Save(std::string);

	glm::mat4 GetViewMatrixForEditor() const;
	void whereDoesMouseRayCutGround(OpenGLContainer*,float& cutX, float& cutY);
private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);
	bool Tool_Draw(OpenGLContainer*,float, float,int);
	bool Tool_Select(OpenGLContainer*, float, float, int);
	bool Tool_Connect(OpenGLContainer*, float, float, int);
	bool Tool_GlobalTool(OpenGLContainer*, float, float, int);
	bool LoadFile(std::string);

	void TestMap(OpenGLContainer*);

	UiElement tool_select_bar;
	UiElement tool_option_bar;
	void RemakeUi();

	int selected_tool;//0 draw, 1 select, 2 connection
	std::string name_of_current_file;

	std::vector<LabyrinthPartInstance> parts;
	std::vector<VariantCarrier> variant_carriers;

	LabyrinthPartDefinition_Manager* part_def_manager;


	//Draw tool things
	std::string currently_selected_part_type;

	//Select tool things
	int currently_selected_instance; // Also used by many other tools
	std::vector<int> list_of_other_instances_nearby;

	//bool ignore_esc_until_released;//Allows ignoring the esc key when returning from a sub object, esc must be released and pressed again to terminate the editor
	Key key_esc;

	int fast_time;
};