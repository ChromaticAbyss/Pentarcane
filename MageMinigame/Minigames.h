#pragma once

#include <vector>
#include <string>

#include "LabyrinthInstance.h"

#include "LabyrinthResult.h"

#include <glm/glm.hpp>

class LabyrinthPartDefinition_Manager;
class OpenGLContainer;

class Minigames {
public:
	Minigames(std::vector<std::string> labyrinth_names,int spell, std::vector<int> monster_ids, LabyrinthPartDefinition_Manager* part_def_manager);

	std::vector<LabyrinthResult> Resolve(OpenGLContainer* open_gl);//Return type?

	std::vector<float> ViewportPosSize(int nr, int nr_max, OpenGLContainer* open_gl);

private:
	glm::mat4 GetViewMatrixForMinigame() const;
	glm::mat4 GetProjectionMatrixForMinigame() const;

	//Data members
	bool done;
	std::vector<LabyrinthInstance> labyrinths;
	std::vector<LabyrinthResult> labyrinth_results;

};