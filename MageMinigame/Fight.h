#pragma once

class GroupDefinition_Manager;
class MonsterDefinition_Manager;
class OpenGLContainer;

#include "Monster.h"

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "Stage.h"
#include "PlayerState.h"

#include "Ui/UiElement.h"
#include "Ui/UiAnimation.h"
#include "Ui/AnimationManager.h"
#include "Ui/Element2D.h"

#include "LabyrinthResult.h"

#include "FightData.h" 

class LabyrinthPartDefinition_Manager;

class Fight {
public:
	enum States { player_target_select, player_action_resolution, moster_counter_resolution, monster_pre_action, monster_action_resolution};

	Fight(OpenGLContainer* open_gl_,PlayerState,std::shared_ptr<GroupDefinition_Manager>, std::shared_ptr<MonsterDefinition_Manager>, LabyrinthPartDefinition_Manager* part_def_manager_,FightData fight_data, std::vector<Monster>); 

	PlayerState Resolve();//Return value: 0 Player lost, 1 Player won
	//TODO: Sepcial object containing the result of the battle: w/l, new hp, loot whatever

private:
	void RenderUi();
	std::vector<float> PollInputs();
	void ChangeHoveredEnemy(int);
	bool AreEnemiesAlive() const;
	void RemakeUi();

	bool LogicStep(const std::vector<float>&);
	bool LogicStep_MonsterActionChoice();//Returns if there is another monster that can do something
	bool LogicStep_MonsterActionResolution();//Returns wether there is more to do  
	void LogicStep_ApplySingleSegment(MonsterCastSegment segment);

	void ApplyResult(const LabyrinthResult& , int spell);
	void AnimationStep();
	void ChangePlayerHp(int);
	glm::mat4 GetViewMatrixForFight() const;
	glm::mat4 GetProjectionMatrixForFight() const;


	//TODO: pointer to GLEW/OpenGL containing object
	std::tr1::shared_ptr<GroupDefinition_Manager> group_definition_manager;
	std::tr1::shared_ptr<MonsterDefinition_Manager> monster_definition_manager;
	LabyrinthPartDefinition_Manager* part_def_manager;
	std::vector<Monster> monsters;

	PlayerState player_state;

	//Monster casting etc.
	MonsterCast monster_action_currently_being_resolved;
	std::vector<MonsterCast> monster_actions_pending;
	void ChooseAndStartMonsterActionResult(std::vector<MonsterCast> new_cast_candiates);


	int hovered_spell;
	int hovered_enemy;
	int entity_turn;//-1 = Player turn
	AnimationManager animations_running;
	States logic_state;

	std::vector<LabyrinthResult> labyrinth_results;


	Instance2D selection_marker;


	GLuint program_id_3d_shader;

	UiElement sidebar;
	UiElement monster_info;

	Stage stage;

	OpenGLContainer* open_gl;

	int end_delay;
	bool fight_over;
};

