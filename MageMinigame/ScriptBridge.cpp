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

	void NotEnoughArgumentsDeliveredError(lua_State * Lua) { //Makes a short error message
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
		lua_pushnumber(Lua, input_x);
		lua_pushnumber(Lua, input_y);
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
		string unique_name = "";
		if (lua_isstring(Lua, -args + 4)) {
			unique_name = lua_tostring(Lua, -args + 4);//5th argument
		}

		//Log("Debug", "Spawned: unique name: " + unique_name);

		int new_id = labyrinth_instance->Spawn(name, x, y, z, unique_name);

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
		int part = -1;
		if (args >= 2 && lua_isstring(Lua, -args + 1)) {
			part = (int)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		labyrinth_instance->Hide(id, part);
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
		int part = -1;
		if (args >= 2 && lua_isstring(Lua, -args + 1)) {
			part = (int)lua_tonumber(Lua, -args + 1);//2nd argument
		}

		labyrinth_instance->Unhide(id, part);
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

		int part = -1;
		if (args >= 5 && lua_isstring(Lua, -args + 4)) {
			part = (int)lua_tonumber(Lua, -args + 4);//5th argument
		}


		labyrinth_instance->Move(id, x, y, z, part);
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

		int part = -1;
		if (args >= 5 && lua_isstring(Lua, -args + 4)) {
			part = (int)lua_tonumber(Lua, -args + 4);//5th argument
		}

		labyrinth_instance->MoveTo(id, x, y, z, part);
		return 0;
	}

	//Only Z axis for now
	int RotateZ(lua_State * Lua)
	{
		int args = lua_gettop(Lua);
		if (args < 2) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		float rz = 0;
		if (lua_isstring(Lua, -args + 1)) {
			rz = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}

		int part = -1;
		if (args >= 3 && lua_isstring(Lua, -args + 2)) {
			part = (int)lua_tonumber(Lua, -args + 2);//3rd argumemnt
		}



		labyrinth_instance->Rotate(id, 0, 0, rz, part);


		return 0;
	}

	int FaceDirection(lua_State * Lua)
	{
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
		float z = 0;		//Ignore z for now
		if (lua_isstring(Lua, -args + 3)) {
			z = (float)lua_tonumber(Lua, -args + 3);//4th argumemnt
		}

		int part = -1;
		if (args >= 5 && lua_isstring(Lua, -args + 4)) {
			part = (int)lua_tonumber(Lua, -args + 4);//5th argumemnt
		}


		//Normalise
		float length = sqrt(x*x + y*y);
		if (length == 0) {
			return 0;
		}
		x = x / length;
		y = y / length;

		float rz = 0.0f;

		if (x == 0) {
			if (y > 0) {
				rz = 270;
			}
			else {
				rz = 90;
			}
		}
		else {
			if (y == 0) { y = 0.0001f; }

			if (x > 0) {
				rz = 360 / (3.1415f * 2) * atan(y / x) + 180;
			}
			if (x < 0) {
				rz = 360 / (3.1415f * 2) * atan(y / x);
			}
		}

		labyrinth_instance->RotateTo(id, 0, 0, rz, -1);


		return 0;
	}


	int SetScale(lua_State * Lua)
	{
		int args = lua_gettop(Lua);
		if (args < 2) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}
		float sc = 0;
		if (lua_isstring(Lua, -args + 1)) {
			sc = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		if (sc < 0.001) {
			sc = 0.001;
		}

		int part = -1;
		if (args >= 3 && lua_isstring(Lua, -args + 2)) {
			part = (int)lua_tonumber(Lua, -args + 2);//3rd argumemnt
		}


		labyrinth_instance->SetScale(id, sc, part);

		return 0;
	}


	int SetColor(lua_State * Lua) {

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
		float r = 0;
		if (lua_isstring(Lua, -args + 1)) {
			r = (float)lua_tonumber(Lua, -args + 1);//2nd argument
		}
		float g = 0;
		if (lua_isstring(Lua, -args + 2)) {
			g = (float)lua_tonumber(Lua, -args + 2);
		}
		float b = 0;
		if (lua_isstring(Lua, -args + 3)) {
			b = (float)lua_tonumber(Lua, -args + 3);
		}

		Color col(r, g, b);

		int part = -1;
		if (args >= 3 && lua_isstring(Lua, -args + 4)) {
			part = (int)lua_tonumber(Lua, -args + 4);//3rd argumemnt
		}



		labyrinth_instance->SetColor(id, col, part);

		return 0;

	}



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
		if (id < 0 || id >= script_parameters_strings.size()) {
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
		if (id < 0 || id >= message_parameters_strings.size()) {
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
		if (id < 0 || id >= script_parameters_floats.size()) {
			//Log("Error", "GetScriptParameterFloat: Invalid id supplied, Vector doesnt have the correct size!");
			return 0; //Script will just get nil
		}
		lua_pushnumber(Lua, script_parameters_floats[id]);
		return 1;
	}

	int GetNamedScriptParameter(lua_State * Lua)
	{
		int args = lua_gettop(Lua);
		if (args < 2) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		int id;
		if (lua_isstring(Lua, -args + 0)) {
			id = (int)lua_tonumber(Lua, -args + 0);//1st argument
		}

		string name = "";
		if (lua_isstring(Lua, -args + 1)) {
			name = lua_tostring(Lua, -args + 1);//2nd argument
		}

		int offset = 0; //In case there can be more than one such element
		if (args >= 3 && lua_isstring(Lua, -args + 2)) {
			id = (int)lua_tonumber(Lua, -args + 2);//3rd argument
		}

		//TODO: Safety check
		if (id < 0) {
			//Log("Error", "GetScriptParameterString: Invalid id supplied, Vector doesnt have the correct size!");
			return 0; //Script will just get nil
		}
		lua_pushstring(Lua, labyrinth_instance->GetNamedParameter(id, name).c_str());
		return 1;
	}
	;


	int Sqrt(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		float n = 1.0f;
		if (lua_isstring(Lua, -args + 0)) {
			n = (float)lua_tonumber(Lua, -args + 0);//1st argument
		}

		if (n < 0) {
			Log("Error(Lua)", "Lua trying to Sqrt negative numbers");
			return 0;
		}

		lua_pushnumber(Lua, sqrt(n));
		return 1;
	};


	int GrantLabyrinthBonus(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 3) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}

		//Get Parameters
		string n = "";
		if (lua_isstring(Lua, -args + 0)) {
			n = lua_tostring(Lua, -args + 0);//1st argument
		}

		string n_name_addon = "";
		if (lua_isstring(Lua, -args + 1)) {
			n_name_addon = lua_tostring(Lua, -args + 1);//2nd argument
		}

		string n_line_addon = "";
		if (lua_isstring(Lua, -args + 2)) {
			n_line_addon = lua_tostring(Lua, -args + 2);//3rd argument
		}


		labyrinth_instance->GrantBonus(n, n_name_addon, n_line_addon);
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
			lua_pushnumber(Lua, 1);
			return 1;
		}
		lua_pushnumber(Lua, 0);
		return 1;
	}

	int GetSpellType(lua_State * Lua) {
		//See Spell.h for the keys
		lua_pushnumber(Lua, (int)labyrinth_instance->get_spell_type().code);
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
		if (res) { lua_pushnumber(Lua, 1); }
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


	int StartAnimation(lua_State * Lua){
		int args = lua_gettop(Lua);
		if (args < 4) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		string name;
		if (lua_isstring(Lua, -args + 0)) {
			name = lua_tostring(Lua, -args + 0);//1st argument
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

		float rot_z = rand()*360;
		if (lua_isstring(Lua, -args + 4)) {
			rot_z = (float)lua_tonumber(Lua, -args + 4);//5th argumemnt
		}

		vector<string> other_params;
		int ii = 5;
		while (lua_isstring(Lua, -args + ii)) { //Capture rest of arguments
			other_params.push_back(lua_tostring(Lua, -args + ii));
			++ii;
		}


		labyrinth_instance->get_particle_manager()->add_animation(name,x,y,z,rot_z,other_params);
	}


	int ScreenShake(lua_State * Lua)
	{
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		float intensity = 0;
		if (lua_isnumber(Lua, -args + 0)) {
			intensity = lua_tonumber(Lua, -args + 0);//1st argument
		}

		labyrinth_instance->shake_screen(intensity);
	}

	int HasSpecialEffect(lua_State * Lua) {
		int args = lua_gettop(Lua);
		if (args < 1) {
			NotEnoughArgumentsDeliveredError(Lua);
			return 0;
		}
		//Get Parameters
		string name;
		if (lua_isstring(Lua, -args + 0)) {
			name = lua_tostring(Lua, -args + 0);//1st argument
		}

		//Return result
		lua_pushboolean(Lua, labyrinth_instance->has_labyrinth_effect(name));
		return 1;
	}



	int EnemyHit(lua_State * Lua) {
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

		labyrinth_instance->EnemyHit(name);
		return 0;
	}
			
			
			
			
	



};//End of namespace


