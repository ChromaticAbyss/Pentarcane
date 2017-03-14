#include "ScriptBridge.h"

#include "Log.h"

#include "IrrklangSingleton.h"


#include "HashingForTags.h"

using namespace std;

namespace ScriptBridge {

	int script_owner = -1; //-1 means no script running
	LabyrinthInstance* labyrinth_instance = 0;
	float input_x = 0;
	float input_y = 0;
	vector<float> script_parameters_floats;
	vector<string> script_parameters_strings;
	std::vector<std::string> message_parameters_strings;

	void NotEnoughArgumentsDeliveredError(lua_State * Lua){ //Makes a short error message
		lua_getglobal(Lua, "script_name");
		string script_name;
		if (lua_isstring(Lua, lua_gettop(Lua))) {
			script_name = lua_tostring(Lua, lua_gettop(Lua));
		}
		Log("Error(Script:" + script_name + ")", "Not enough arguments delivered");
	};


	int TestLua(lua_State * Lua) {
		cout << "Lua working" << endl;
		return 0;
	};


	int GetMyId(lua_State * Lua) {
		lua_pushnumber(Lua, script_owner);
		return 1;
	};

	int GetInputs(lua_State * Lua) {
		lua_pushnumber(Lua,input_x);
		lua_pushnumber(Lua,input_y);
		return 2;
	};

	int GetIdFromUniqueName(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		//First Parameter
		string name;
		if (lua_isstring(Lua, -args + 0)) {
			name = lua_tostring(Lua, -args + 0);//1st argument
		}

		lua_pushnumber(Lua, labyrinth_instance->GetIdFromUniqueName(name));
		return 1;
	};



	int Spawn(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 4) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}


		string name;
		if (lua_isstring(Lua, -args + 0)) {
			name = lua_tostring(Lua, -args + 0);//1st argument

		}
		float x=0;
		if (lua_isstring(Lua, -args + 1)) {
			x = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		float y=0;
		if (lua_isstring(Lua, -args + 2)) {
			y = (float)lua_tonumber(Lua, -args + 2);//3rd argument
		}
		float z=0;
		if (lua_isstring(Lua, -args + 3)) {
			z = (float)lua_tonumber(Lua, -args + 3);//4th argument
		}
		string unique_name = "";
		if (lua_isstring(Lua, -args + 4)) {
			unique_name = lua_tostring(Lua, -args + 4);//5th argument
		}

		Log("Debug", "Spawned: unique name: " + unique_name);

		int new_id = labyrinth_instance -> Spawn(name,x,y,z,unique_name);

		lua_pushnumber(Lua, new_id);
		return 1;
	};


	int Hide(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		labyrinth_instance->Hide(id);
		return 0;
	};
	int Unhide(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		labyrinth_instance->Unhide(id);
		return 0;
	};
	int IsHidden(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		bool result = labyrinth_instance->IsHidden(id);
		if (result) {
			lua_pushnumber(Lua, 1);
			return 1;
		}
		lua_pushnumber(Lua, 0);
		return 1;
	};


	int Disable(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		labyrinth_instance->Disable(id);
		return 0;
	};


	int GetPosition(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		lua_pushnumber(Lua, labyrinth_instance->GetPosX(id));
		lua_pushnumber(Lua, labyrinth_instance->GetPosY(id));
		lua_pushnumber(Lua, labyrinth_instance->GetPosZ(id));
		return 3;
	};


	int Move(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 4) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		float x = 0;
		if (lua_isstring(Lua, -args + 1)) {
			x = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		float y = 0;
		if (lua_isstring(Lua, -args + 2)) {
			y = (float)lua_tonumber(Lua, -args + 2);//3rd argument
		}
		float z = 0;
		if (lua_isstring(Lua, -args + 3)) {
			z = (float)lua_tonumber(Lua, -args + 3);//4th argument
		}

		labyrinth_instance->Move(id, x, y, z);
		return 0;
	};

	int MoveTo(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 4) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		float x = 0;
		if (lua_isstring(Lua, -args + 1)) {
			x = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		float y = 0;
		if (lua_isstring(Lua, -args + 2)) {
			y = (float)lua_tonumber(Lua, -args + 2);//3rd argument
		}
		float z = 0;
		if (lua_isstring(Lua, -args + 3)) {
			z = (float)lua_tonumber(Lua, -args + 3);//4th argumemnt
		}

		labyrinth_instance->MoveTo(id, x, y, z);
		return 0;
	};


	int CheckCollisionWithTag(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 5) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id_1;
		if (lua_isstring(Lua, -args + 0)) {
			id_1 = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		string tag;
		if (lua_isstring(Lua, -args + 1)) {
			tag = lua_tostring(Lua, -args + 1);//2nd argument
		}
		float x = 0;
		if (lua_isstring(Lua, -args + 2)) {
			x = (float)lua_tonumber(Lua, -args + 2);//3rd argument
		}
		float y = 0;
		if (lua_isstring(Lua, -args + 3)) {
			y = (float)lua_tonumber(Lua, -args + 3);//4th argument
		}
		float z = 0;
		if (lua_isstring(Lua, -args + 4)) {
			z = (float)lua_tonumber(Lua, -args + 4);//5th argument
		}

		int tag_int = HashingForTags::tags.Get(tag);
		int colliding_with = labyrinth_instance->CollidingWithTag(id_1, tag_int, x, y, z);

		lua_pushnumber(Lua, colliding_with); //Push 2 when not colliding
		return 1;
	}

	int GetDistanceFromTag(lua_State * Lua)
	{

		int args = lua_gettop(Lua);
		if (args < 2) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id_1;
		if (lua_isstring(Lua, -args + 0)) {
			id_1 = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		string tag;
		if (lua_isstring(Lua, -args + 1)) {
			tag = lua_tostring(Lua, -args + 1);//2nd argument
		}

		int tag_int = HashingForTags::tags.Get(tag);

		//int colliding_with = labyrinth_instance->CollidingWithTag(id_1, tag_int, x, y, z);
		float dist = labyrinth_instance->GetDistanceFromTag(id_1, tag_int);

		lua_pushnumber(Lua, dist); //Push 2 when not colliding
		return 1;
	}
	;

	int GetScriptParameterString(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		//TODO: Safety check
		if (id<0 || id>=script_parameters_strings.size()) {
			//Log("Error", "GetScriptParameterString: Invalid id supplied, Vector doesnt have the correct size!");
			return 0; //Script will just get nil
		}
		lua_pushstring(Lua, script_parameters_strings[id].c_str());
		return 1;
	};

	int GetMessageParameterString(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		//TODO: Safety check
		if (id<0 || id >= message_parameters_strings.size()) {
			//Log("Error", "GetMessageParameterString: Invalid id supplied, Vector doesnt have the correct size!");
			return 0; //Script will just get nil
		}
		lua_pushstring(Lua, message_parameters_strings[id].c_str());
		return 1;
	};

	int GetScriptParameterFloat(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0; 
		}

		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		//TODO: Safety check
		if (id<0 || id>=script_parameters_floats.size()) {
			//Log("Error", "GetScriptParameterFloat: Invalid id supplied, Vector doesnt have the correct size!");
			return 0; //Script will just get nil
		}
		lua_pushnumber(Lua, script_parameters_floats[id]);
		return 1;
	};


	int Sqrt(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		float n=1.0f;
		if (lua_isstring(Lua, -args + 0)) {
			n = (float)lua_tonumber(Lua, -args + 0);//1st argument
		}

		if (n < 0) {
			Log("Error(Lua)", "Lua trying to Sqrt negative numbers");
			return 0;
		}

		lua_pushnumber(Lua,sqrt(n));
		return 1;
	};


	int GrantLabyrinthBonus(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		string n = "";
		if (lua_isstring(Lua, -args + 0)) {
			n = lua_tostring(Lua, -args + 0);//1st argument
		}

		labyrinth_instance->GrantBonus(n);
		return 0;
	};

	int GrantLabyrinthGoal(lua_State * Lua) {
		labyrinth_instance->GrantGoal();
		return 0;
	};

	int GrantLabyrinthKey(lua_State * Lua) {
		labyrinth_instance->GrantKey();
		return 0;
	};

	int UseLabyrinthKey(lua_State * Lua) {
		bool result = labyrinth_instance->UseKey();
		if (result) {
			lua_pushnumber(Lua,1);
			return 1;
		}
		lua_pushnumber(Lua, 0);
		return 1;
	}

	int GetSpellType(lua_State * Lua) {
		lua_pushnumber(Lua,labyrinth_instance->GetSpellType());
		return 1;
	};

	int GetIsDisabled(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		int id = -1;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		bool res = labyrinth_instance->IsDisabled(id);
		if(res){ lua_pushnumber(Lua, 1); }
		else { lua_pushnumber(Lua, 0); }
		return 1;
	};





	int SendMessage(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 2) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		int id = 0;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		int type = 0;
		if (lua_isstring(Lua, -args + 1)) {
			type = (int)lua_tonumber(Lua, -args + 1);//2nd argument
		}

		InterObjectMessage temp;
		temp.recipient = id;
		for (int pos = 2; pos < args; pos++) { //All remaining arguments are string content
			if (lua_isstring(Lua, -args + 2)) {
				string a = lua_tostring(Lua, -args + pos);//3+ argument
				//cout << a << endl;
				temp.content.push_back(a);
			}
		}


		labyrinth_instance->AddMessage(temp);

		return 0;
	}



	int PlaySound(lua_State * Lua)
	{
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		string name = "";
		if (lua_isstring(Lua, -args + 0)) {
			name = lua_tostring(Lua, -args + 0);//1st argument
		}
		if (name[0] != '.') {
			name = "../Data/Sounds/" + name;
		}

		IrrklangSingleton().Get()->play2D(name.c_str(), false);
		return 0;
	}


};//End of namespace


