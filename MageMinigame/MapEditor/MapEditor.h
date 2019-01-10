#pragma once

#include "../BuildHeader.h"
class LabyrinthPartDefinition_Manager;
class OpenGLContainerWith3D;
class DrawMode;

#include <memory>
#include <vector>
#include <string>

#include "VariantCarrier.h"
#include "Labyrinth/LabyrinthPartInstance.h"
#include "MapEditorTileSelector.h"

#include "../Key.h"

#include "MapEditor\Modes\MapEditorMode.h"



class MapEditor {
public:
	MapEditor(LabyrinthPartDefinition_Manager* part_def_manager_, OpenGLContainerWith3D* open_gl_in);
	~MapEditor();

	void run();
	void Render();
	void Logic(float,float);
	void SaveDialogue();
	void Save(std::string);

	glm::mat4 GetViewMatrixForEditor() const;
	void whereDoesMouseRayCutGround(OpenGLContainer*,float& cutX, float& cutY);

private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);
	bool LoadFile(std::string);
	void reset_tools();

	void TestMap(OpenGLContainer*);
	void remake_ui();
	void remake_tool_select_bar();

	OpenGLContainerWith3D* open_gl;
	std::unique_ptr<PolymorphicUiElementReduced> tool_select_bar;


	std::unique_ptr<DrawMode> draw_mode;
	MapEditorMode* current_mode;


	std::string name_of_current_file;

	std::vector<std::unique_ptr<LabyrinthPartInstance>> parts;
	std::vector<std::unique_ptr<LabyrinthPartInstance>> deco_parts;


	//std::vector<VariantCarrier> variant_carriers;
	LabyrinthPartDefinition_Manager* part_def_manager;

	//bool ignore_esc_until_released;//Allows ignoring the esc key when returning from a sub object, esc must be released and pressed again to terminate the editor
	Key key_esc;
	Key key_r;

	int fast_time;
};