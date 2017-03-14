#pragma once


#include <string>
#include <vector>
#include <memory>


#include "PlayerState.h"

#include "OpenGLContainer.h"

#include "FightData.h"
#include "Stats.h"

#include "LabyrinthPartDefinition_Manager.h"
#include "GroupDefinition_Manager.h"
#include "MonsterDefinition_Manager.h"

class Sequence {
public:
	Sequence(std::string);

	bool Resolve(OpenGLContainer* open_gl, std::tr1::shared_ptr<GroupDefinition_Manager>, std::tr1::shared_ptr<MonsterDefinition_Manager>, LabyrinthPartDefinition_Manager* part_def_manager);



private:

	int progress;
	Stats sequence_modifiers;

	std::vector<FightData> fight_data;
	std::vector<std::string> groups_we_already_had;//To prevent repetitions

	std::string sound_file;
};