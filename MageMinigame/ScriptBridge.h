#pragma once

#include "LabyrinthInstance.h"

#include <vector>
#include <string>

namespace ScriptBridge {

	extern int script_owner; //-1 means no script running
	extern LabyrinthInstance* labyrinth_instance;
	extern float input_x;
	extern float input_y;
	extern std::vector<float> script_parameters_floats;
	extern std::vector<std::string> script_parameters_strings;

	extern std::vector<std::string> message_parameters_strings;

	void NotEnoughArgumentsDeliveredError(lua_State * Lua);

	int TestLua(lua_State * Lua);
	int GetMyId(lua_State * Lua);
	int GetInputs(lua_State * Lua);
	int GetIdFromUniqueName(lua_State * Lua);
	int Spawn(lua_State * Lua);
	int Hide(lua_State * Lua);
	int Unhide(lua_State * Lua);
	int IsHidden(lua_State * Lua);
	int Disable(lua_State * Lua);
	int GetIsDisabled(lua_State * Lua);
	int GetPosition(lua_State * Lua);
	int Move(lua_State * Lua);
	int MoveTo(lua_State * Lua);
	int CheckCollisionWithTag(lua_State * Lua);
	int GetDistanceFromTag(lua_State * Lua);
	int GetScriptParameterString(lua_State * Lua);
	int GetScriptParameterFloat(lua_State * Lua);
	int GetMessageParameterString(lua_State * Lua);
	int Sqrt(lua_State * Lua);
	int GrantLabyrinthBonus(lua_State * Lua);
	int GrantLabyrinthGoal(lua_State * Lua);
	int GrantLabyrinthKey(lua_State * Lua);
	int UseLabyrinthKey(lua_State * Lua);
	int GetSpellType(lua_State * Lua);
	int SendMessage(lua_State * Lua);
	int PlaySound(lua_State * Lua);

};


