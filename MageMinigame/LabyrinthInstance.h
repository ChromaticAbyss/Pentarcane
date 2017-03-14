#pragma once

#include "LabyrinthPartInstance.h"
#include <memory>
#include <vector>
#include <string>
#include <queue>

class LabyrinthPartDefinition_Manager;
class OpenGLContainer;

#include "LabyrinthResult.h"
#include "InterObjectMessage.h"
#include "tinyxml2.h"

class TimerManager;

class LabyrinthInstance {
public:
	LabyrinthInstance(std::string file_name,int spell_, int monster_id_, LabyrinthPartDefinition_Manager* part_def_manager_);


	void Render(Transform, OpenGLContainer*) const; //Can be scaled and repositioned optically(used for fireball)
	void Render2D(OpenGLContainer*) const;
	LabyrinthResult LogicStep(std::vector<float> inputs);


	bool TestPartCollisionsWithTag(int,int,float,float,float);

	//For scripts
	int GetIdFromUniqueName(std::string);
	int Spawn(std::string, float, float, float,std::string = "");
	void Hide(int id) { inert_parts[id].Hide(); };
	void Unhide(int id) { inert_parts[id].Unhide(); };
	bool IsHidden(int id) const { return inert_parts[id].Hidden(); };
	bool Disable(int id);
	bool IsDisabled(int id){return inert_parts[id].Disabled();}
	float GetPosX(int id) { return inert_parts[id].X(); };
	float GetPosY(int id) { return inert_parts[id].Y(); };
	float GetPosZ(int id) { return inert_parts[id].Z(); };
	void Move(int id, float x, float y, float z) { inert_parts[id].Move(x, y, z); };
	void MoveTo(int id, float x, float y, float z) { inert_parts[id].MoveTo(x, y, z); };
	int CollidingWithTag(int id_1, int tag_int, float offset_x, float offset_y, float offset_z);
	float GetDistanceFromTag(int id_1, int tag_int);
	void GrantBonus(std::string);
	void GrantGoal();
	void GrantKey() { key++; };
	bool UseKey();
	int GetSpellType() { return spell; };

	void AddMessage(InterObjectMessage a) { messages.push(a);  };
private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);

	//Something to hold the player entity
	int spell;
	int monster_id;

	std::vector<std::string> bonus; //What bonus effects were collected by the player?

	int goal; //How many player objects are in the goal?
	int key; //How many keys do we have

	std::vector<LabyrinthPartInstance> inert_parts;
	std::vector<LabyrinthPartInstance> spawn_list;
	std::queue<InterObjectMessage> messages;

	//std::vector<LabyrinthPartInstance> spawn_list; //list of things spawned during scripts. At the end of the logic step addenate to regular list

	LabyrinthPartDefinition_Manager* part_def_manager;

	int calls_with_no_player_objects;

	std::shared_ptr<TimerManager> timer_manager;

	int frame;
	int finish_frame;
	int fast_frame;
};