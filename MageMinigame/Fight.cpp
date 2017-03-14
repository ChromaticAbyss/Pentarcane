#include "Fight.h"

#include "GroupDefinition_Manager.h"
#include "MonsterDefinition_Manager.h"
#include "LabyrinthPartDefinition_Manager.h"
#include "OpenGLContainer.h"
#include "Minigames.h"

#include "common\shader.hpp"
#include "common\quaternion_utils.hpp"
#include "Log.h"
#include <algorithm>

#include "Ui\Element2D.h"

#include "IrrklangSingleton.h"

using namespace std;

Fight::Fight(OpenGLContainer* open_gl_,PlayerState player_state_,std::tr1::shared_ptr<GroupDefinition_Manager> gr_def_man, std::tr1::shared_ptr<MonsterDefinition_Manager> mon_def_man,LabyrinthPartDefinition_Manager* part_def_manager_,FightData fight_data,vector<Monster> monsters_)
	:group_definition_manager(gr_def_man),monster_definition_manager(mon_def_man),monsters(monsters_),player_state(player_state_), monster_action_currently_being_resolved()
	,part_def_manager(part_def_manager_),hovered_enemy(0),hovered_spell(1),entity_turn(-1),sidebar(),animations_running(),logic_state(States::player_target_select)
	,selection_marker("../Data/2D/Ui/SelectionMarker.png",Transform2D(0.0f,0.0f,1.35f,1.35f,false))
	,monster_info(),stage(fight_data.stage_name),open_gl(open_gl_),end_delay(50),fight_over(false)
{
	//Set up combat group
	int a = 0;
	for (auto it = monsters.begin(); it != monsters.end(); ++it) {
		it->SetPosition(stage.MonsterPosition(a));
		it->ApplyBonusStats(fight_data.monster_stat_modifier);
		a++;
	}

	RemakeUi();
}



PlayerState Fight::Resolve() {
	//Loop and make timed calls to: Rendering(up to upper limit set in config), Animation&Input(Lets say 30 a second to make things fluid but the same on all machines), 
	float min_time_between_renders = 1.0f / 60.0f; //60 fps, possibly user setting
	float time_last_render = float(glfwGetTime());
	float min_time_game_logic = 1.0f / 60.0f;
	float time_last_game_logic = float(glfwGetTime()) - 0.5 / 60.0f;

	//------------------------OpenGL setup-------------------------------------
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//TODO: This is probably not even needed [As of now it is... see OpenGlContainer TO DO]
	program_id_3d_shader = LoadShaders("StandardShading.vertexshader", "SimplifiedShader.fragmentshader");


	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//-------------------------------------done-------------------------------------------
	bool done = false;
	while (done == false) {
		float current_time = float(glfwGetTime());

		//Rendering
		if (current_time - time_last_render > min_time_between_renders) {
			time_last_render = current_time;
			//float measure_full_render_time = current_time;
			open_gl->UpdateWindowSize();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Render3D();
			//Render animations  (2D)
			RenderUi();

			glfwSwapBuffers(open_gl->window);
			//cout << "Total render time: " << (float(glfwGetTime()) - measure_full_render_time) << endl;
		}

		//Logic
		if (current_time - time_last_game_logic > min_time_game_logic) {
			time_last_game_logic = current_time;
			//float measure_full_logic_time = current_time;

			vector<float> inputs = PollInputs();


			AnimationStep();

			if (!fight_over) {
				LogicStep(inputs);
			}

			if (player_state.HitPoints() <= 0) {
				//Lose fight
				end_delay--;
				fight_over = true;
				if (end_delay <= 0) {
					return player_state;
				}
			}

			if (AreEnemiesAlive() == false) {
				//Win fight!
				end_delay--;
				fight_over = true;
				if (end_delay <= 0) {
					return player_state;
				}
			}
			//cout << "Total logic time: " << (float(glfwGetTime()) - measure_full_logic_time) << endl;
		}

		// Check if the ESC key was pressed or the window was closed
		if (glfwGetKey(open_gl->window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(open_gl->window) != 0) {
			done = true;
			return PlayerState(-1);

			//Do some other stuff with the labrinths that are still running(fail them!)
		}
	}


	return player_state;
}

vector<float> Fight::PollInputs() {
	static int key_right_down = 0;
	static int key_left_down = 0;
	static int key_enter_down = 0;
	static int key_up_down = 0;
	static int key_down_down = 0;

	glfwPollEvents();
	vector<float> inputs;
	for (int ii = 0; ii < 5; ii++) {
		inputs.push_back(0.0f);
	}

	//Input device = keyboard
	if (glfwGetKey(open_gl->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		key_right_down = 1;
	}
	else {
		if (key_right_down == 1) {
			key_right_down = 0;
			inputs[0] += 1.0f;
		}
	}


	if (glfwGetKey(open_gl->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		key_left_down = 1;
	}
	else {
		if (key_left_down == 1) {
			key_left_down = 0;
			inputs[0] -= 1.0f;
		}
	}


	if (glfwGetKey(open_gl->window, GLFW_KEY_UP) == GLFW_PRESS) {
		key_up_down = 1;
	}
	else {
		if (key_up_down == 1) {
			key_up_down = 0;
			inputs[1] += 1.0f;
		}
	}
	if (glfwGetKey(open_gl->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		key_down_down = 1;
	}
	else {
		if (key_down_down == 1) {
			key_down_down = 0;
			inputs[1] -= 1.0f;
		}
	}


	if (glfwGetKey(open_gl->window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		key_enter_down = 1;
	}
	else {
		if (key_enter_down == 1) {
			key_enter_down = 0;
			inputs[2] = 1.0f;
		}
	}


	double mouseX, mouseY;
	glfwGetCursorPos(open_gl->window, &mouseX, &mouseY);
	inputs[3] = (float)mouseX / open_gl->window_x * 2 - 1;
	inputs[4] = -((float)mouseY / open_gl->window_y * 2 - 1);

	if (glfwGetMouseButton(open_gl->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		//inputs[2] = 1.0f;
		cout << "mouse has been disabled for now" << endl;
	}

	return inputs;
}




void Fight::RenderUi() {
	//Set up opengl for 2 dimensional rendering
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glUseProgram(open_gl->program_id_interface_shader);
	//View and Projection matrix are fixed for Minigames

	glm::mat4 ProjectionMatrix = GetProjectionMatrixForFight();
	open_gl->ViewMatrix = GetViewMatrixForFight();
	open_gl->PV = ProjectionMatrix * open_gl->ViewMatrix; //Right way														  //Light Position
	glm::vec3 lightPos = glm::vec3(0, 0, 20);
	glUniform3f(open_gl->light_id, lightPos.x, lightPos.y, lightPos.z);


	stage.RenderBackground(open_gl);
	for_each(monsters.crbegin(), monsters.crend(), [this](const Monster& m) {if (m.Alive()) { m.Render2D(open_gl); } });


	if (!fight_over) {
		selection_marker.Rotate(0.1f);
		selection_marker.Render(stage.MonsterPosition(hovered_enemy).MakeModelMatrix(open_gl), open_gl);
	}

	sidebar.Render(glm::scale(glm::mat4(),glm::vec3(1.0f,1.0f,1.0f)),open_gl);

	if (!fight_over) {
		monster_info.Render(glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f)), open_gl);
	}

	animations_running.Render(open_gl);
}


void Fight::ChangeHoveredEnemy(int dir) {
	if (dir == 0) { return; }
	if (AreEnemiesAlive()==false) {
		return;
	}

	bool alive = false;
	while (alive == false) {
		hovered_enemy += dir;
		if (dir > 0) {
			if (hovered_enemy >= (int)monsters.size()) {
				hovered_enemy = 0;
			}
		}
		if (dir < 0) {
			if (hovered_enemy < 0) {
				hovered_enemy = (int)monsters.size() - 1;
			}
		}
		alive = monsters[hovered_enemy].Alive();
	}

	RemakeUi();
};

bool Fight::LogicStep(const vector<float>& inputs) {
	if (logic_state == States::player_target_select) {
		//Keys
		if (inputs[1] < -0.5) {
			hovered_spell++;
			if (hovered_spell > 4) {
				hovered_spell = 1;
			}
			RemakeUi();
		}
		if (inputs[1] > 0.5) {
			hovered_spell--;
			if (hovered_spell < 1) {
				hovered_spell = 4;
			}
			RemakeUi();
		}
		if (inputs[0] < -0.5) { //Left key
			ChangeHoveredEnemy(-1);
		}
		if (inputs[0] > 0.5) { //Right key
			ChangeHoveredEnemy(1);
		}
		//Enter
		if (inputs[2] >= 1.0f) {
			vector<int> monster_ids;
			vector<string> labyrinth_names;
			if (hovered_spell > 1) { //Not fireball
				monster_ids.push_back(hovered_enemy);
				labyrinth_names.push_back(monsters[monster_ids[0]].Labyrinth());
			}
			else { //Fireball
				   //TODO: Change
				for (size_t i = 0; i < monsters.size(); i++) //Target ALL living monsters
				{
					if (monsters[i].Alive()) {
						monster_ids.push_back(i);
						labyrinth_names.push_back(monsters[i].Labyrinth());
					}
				}

			}
			Minigames minigame(labyrinth_names, hovered_spell, monster_ids, part_def_manager);//Take spell id from command
			labyrinth_results = minigame.Resolve(open_gl);

			logic_state = States::player_action_resolution;

		}
		else {
			//Tooltips, once they are implemented
			//sidebar.CheckInterception(inputs[3], inputs[4], 0);
		}
	} //Players turn


	if (logic_state == States::player_action_resolution) {
		for_each(labyrinth_results.begin(), labyrinth_results.end(), [this](LabyrinthResult& a) {ApplyResult(a, hovered_spell); });
		//Start animations for the spell. These are blocking but always follow the actual application of the effect, unlike monster casts which may have them in front.
		labyrinth_results.clear();//Delete all entries so we skip them enxt time

		//The animations, if any are started ARE blocking, so we wait for them.
		if (animations_running.Blocking()) { //Animation handling
			return false;
		}

		//Animations played, damage deal? Go on
		if (monster_actions_pending.empty()) {
			logic_state = States::monster_pre_action;
		} else {
			monster_action_currently_being_resolved = MonsterCast();
			logic_state = States::moster_counter_resolution; //Add an extra step where the cast from the monster is resolved
		}
		entity_turn = -1;
	}



	if(logic_state == States::monster_pre_action){

		if (LogicStep_MonsterActionChoice() == true) {
			//We have another monster that wants to do something and the respective action is up for processing
			logic_state = States::monster_action_resolution;

		}else{
			//All monsters are finished, its the players turn again
			player_state.BeginningOfTurn();
			logic_state = States::player_target_select;
			entity_turn = -1;
		}
	
	}


	if (logic_state == States::moster_counter_resolution) {
		if (monster_action_currently_being_resolved.Finished() == true) {
			if (monster_actions_pending.empty() == false) {
				monster_action_currently_being_resolved = monster_actions_pending.back();
				monster_actions_pending.pop_back();//Start the next counterattack
			}else {
				logic_state = States::monster_pre_action;
				monster_actions_pending.clear();
				monster_action_currently_being_resolved = MonsterCast();
				return false;
			}
		}
		else {
			//auto a = monster_action_currently_being_resolved.Progress();
			LogicStep_MonsterActionResolution();
		}

	}



	if (logic_state == States::monster_action_resolution) {
		if (LogicStep_MonsterActionResolution()) {
			//Finished with this monster
			monsters[entity_turn].PassTurn(); //Do all things that happen to a monster after if finishes its turn

			logic_state = States::monster_pre_action; //Grab next monster 
		}
		else {
			//Play animations and look pretty I guess.
			//LogicStep_MonsterActionResolution(); //<<<<<<<
		}


	}

	return false;
}



//Handles ONLY the selection of and starting of an animation for that monster
bool Fight::LogicStep_MonsterActionChoice() {
	int size = monsters.size();
	while (entity_turn < size) {
		entity_turn++;
		if (entity_turn == size) {
			//monster_action_currently_being_resolved = MonsterCast();
			return false;
		}

		if (monsters[entity_turn].Alive() == true) {
			break;
		}
	}

	//Grab all possible spells or just an attack
	//TODO: Make the first/last entry the default attack, in case all spells fail their condition 
	auto result = monsters[entity_turn].MonsterAction();

	//Set the proper caster for all casts
	for_each(result.begin(), result.end(), [cast_id = entity_turn](MonsterCast& mc) {mc.SetCasterID(cast_id);});

	//If cast: Select a spell from those that fulfill the conditions. TODO: Or the default attack should all spells fail
	ChooseAndStartMonsterActionResult(result);
	
	return true;
}

//Handles ONLY the resolution of last monsters cast
bool Fight::LogicStep_MonsterActionResolution()
{
	//If we have animations running we wait for them to finish, only then start the next step!
	if (animations_running.Blocking()) { //Animation handling
		return false;
	}

	//Apply the next segment
	LogicStep_ApplySingleSegment(monster_action_currently_being_resolved.Progress());


	if (monster_action_currently_being_resolved.Finished() == true) {
		Log("Debug", "Finished " + monster_action_currently_being_resolved.Name());

		return true;
	}

	return false;
}

void Fight::LogicStep_ApplySingleSegment(MonsterCastSegment segment) {
	//Separate target and effect

	if (segment.type == "Finish") {
		Log("Debug", "Finish");
		monster_action_currently_being_resolved = MonsterCast();
		return;
	}


	if (segment.type == "SpawnMonster") {
		Log("Debug", "Finish");
		bool spawn_stunned = (segment.values[0] > 0);

		auto res = monster_definition_manager->MakeMonster(segment.string_values[0], Transform2D(0.0f, 0.0f, 1.0f, 1.0f, true));

		//TODO: What do we do if no more slots are free?
		int pos = (find_if(monsters.cbegin(), monsters.cend(), not1(mem_fun_ref(&Monster::Alive))) - monsters.begin());
		if (pos == monsters.size()) {
			monsters.emplace_back(std::move(res));
		} else {
			monsters[pos] = std::move(res);
		}

		monsters[pos].SetPosition(stage.MonsterPosition(pos));
		//monsters[pos].ChangeTransform(stage.MonsterPosition(pos));


		//Optional spawn animation (flipbook)
		if (segment.string_values.size() > 1) {
			FlipbookAnimation temp = FlipbookAnimation(segment.string_values[1], stage.MonsterPosition(pos));
			animations_running.AddAnimation(move(temp)); //run an animation
		}

		return;
	}



	//Easy case as there is only one player
	if (segment.targets == "Player") {
		if (segment.type == "Damage") {
			ChangePlayerHp(-segment.values[0]);
		}

		if (segment.type == "UiAnimation") {
			//A ui-animation is always in a static position
			animations_running.AddAnimation(UiAnimation(segment.string_values[0])); 
			return;
		}
		if (segment.type == "FlipbookAnimation") {
			//Plays a blocking animation for a few frames
			//The animation is centered on the players life display
			Transform2D transf_player(0.85f, 0.7f, 0.125f, 1.0f, true);
			animations_running.AddAnimation(move(FlipbookAnimation(segment.string_values[0],transf_player))); 
			return;
		}

		return;
	}


	//The other case is more differentiated
	vector<int> target_monster_ids = segment.SelectTargetsFrom(monsters);
	//Passes our monsters by reference to const for the segment to select the poper entities!
	//The -1 on entity turn is because the turn will have advanced by 1 when this function is called

	cout << "nr monster-targets:" << target_monster_ids.size() << endl;

	if (segment.type == "Damage") {
		int am = segment.values[0];
		for_each(target_monster_ids.begin(), target_monster_ids.end(), [this, am](int& mon) { monsters[mon].DealDamage(am); });
	}

	if (segment.type == "Heal") {
		//for (auto it = target_monster_ids.begin(); it != target_monster_ids.end(); ++it) {
		//	monsters[*it].Heal(segment.values[0]);
		//}
		int am = segment.values[0];
		for_each(target_monster_ids.begin(), target_monster_ids.end(), [this,am](int& mon){ monsters[mon].Heal(am);} );
	}


	if (segment.type == "ChangeStats") {
		Log("Debug", "Change stats");
		if (segment.values.size() < 4) {
			Log("Error", "(segment) Change Stats requires 4 value arguments: mHp,Attack,Armor and CastChance");
			return;
		}
		//Hp, Attack, Armor, Cast
		Stats add(segment.values[0], segment.values[1], segment.values[2], segment.values[3]);

		for (auto it = target_monster_ids.begin(); it != target_monster_ids.end(); ++it) {
			monsters[*it].ApplyBonusStats(add);
		}
	}


	if (segment.type == "ChangeLabyrinth") {
		Log("Debug", "Change Labyrinth");
		if (segment.string_values.size() < 1) {
			Log("Error", "(segment) Change Labyrinth requires at least 1 string value argument");
			return;
		}
		int r = rand() % segment.string_values.size();

		for (auto it = target_monster_ids.begin(); it != target_monster_ids.end(); ++it) {
			monsters[*it].SetLabyrinth(segment.string_values[r]);
		}
	}


	if (segment.type == "FlipbookAnimation") {
		//Plays a blocking animation for a few frames
		//the animation is centered on the target creature(s)
		for (auto it = target_monster_ids.begin(); it != target_monster_ids.end(); ++it) {
			//monsters[*it]
			FlipbookAnimation temp = FlipbookAnimation(segment.string_values[0], stage.MonsterPosition(*it));
			animations_running.AddAnimation(move(temp)); //run an animation

		}

		return;
	}

	
	for (auto it = target_monster_ids.begin(); it != target_monster_ids.end(); ++it) {
		if (hovered_enemy == *it) {
			RemakeUi();
		}
	}
	
};


void Fight::ChooseAndStartMonsterActionResult(vector<MonsterCast> new_cast_candidates) {
	if (new_cast_candidates.size() <= 0) {
		Log("Error", "A monster tried to cast but has no casts attached!");
		monster_action_currently_being_resolved = MonsterCast();//Use a default cast
		return;
	}
	//TODO: check conditions blahblah like group selector does

	//Choose one depending on condition
	int r = rand() % new_cast_candidates.size();

	monster_action_currently_being_resolved = new_cast_candidates[r];
}


void Fight::ChangePlayerHp(int change) {
	player_state.ChangeHp(change);//Reduce player HP
	cout << "Player hp is now: " << player_state.HitPoints() << endl;
	RemakeUi();
}


void Fight::ApplyResult(const LabyrinthResult& result, int spell) {
	if (result.goals <= 0) { return; }
	if (result.monster_id < 0 || result.monster_id >= monsters.size()) {
		Log("Error", "Invalid monster_id passed from result");
		cout << result.monster_id << endl;
		return;
	}

	auto damage = 0;
	auto ignore_armor = false;

	if (spell == 1) { //Fireball
		damage = 3;
		//Add a visual effect for successful fireball cast
		FlipbookAnimation temp2("../Data/Animations/Flipbook_Firesparks.xml", stage.MonsterPosition(result.monster_id));

		animations_running.AddAnimation(move(temp2));
		IrrklangSingleton().Get()->play2D("../Data/Sounds/explosion.wav", false);

	}

	if (spell == 2) { //Lightning Bolt
		damage = 3;
		monsters[result.monster_id].ApplyBonusStats(Stats(0,0,0,-25)); //Apply a -25% cast chance modifier to the enemy
		//Add visual effect for successful lightning bolt cast
		FlipbookAnimation temp2("../Data/Animations/LightningBolt.xml", stage.MonsterPosition(result.monster_id));
		animations_running.AddAnimation(move(temp2));
		IrrklangSingleton().Get()->play2D("../Data/Sounds/thunderstrike.wav", false);
	}


	if (spell == 3) { //Freeze
		damage = 6;//3 + 2 * result.goals;
		//Add visual effect for successful frozen star cast
		FlipbookAnimation temp2("../Data/Animations/FrozenStar.xml", stage.MonsterPosition(result.monster_id));
		animations_running.AddAnimation(move(temp2));
		IrrklangSingleton().Get()->play2D("../Data/Sounds/wind_monster03.mp3", false);
	}

	if (spell == 4) { //Life drain
		ignore_armor = true;
		damage = 3;
		//Add visual effect for successful lifedrain cast
		FlipbookAnimation temp2("../Data/Animations/LifeDrain.xml", stage.MonsterPosition(result.monster_id));
		animations_running.AddAnimation(move(temp2));
		IrrklangSingleton().Get()->play2D("../Data/Sounds/richerland_magic.ogg", false);
	}

	if (result.fast) {
		monsters[result.monster_id].ApplyEffect_Freeze(); //If the monster would attack next turn it instead does nothing.
	}


	auto combine_damage = 0;

	//Apply bonus effects
	for (auto it = result.bonus_effects.cbegin(); it != result.bonus_effects.cend(); ++it)
	{

		if(*it == "ExtraDamage_1"){
			damage++;
		}
		if (*it == "CombineDamage") {
			combine_damage++;
			if (combine_damage==5)
			{
				damage += 5;
			}
		}
		if (*it == "IgnoreArmor") {
			ignore_armor = true;
		}
		if (*it == "Shatter") { //Symbol: Cracked shield
			monsters[result.monster_id].ApplyBonusStats(Stats(0,0,-2,0));//Reduce armor by 2 permanently
		}
		if (*it == "Shield") { //Symbol: Cracked shield
			player_state.AddShield(3);
		}

	}

	//Apply total damage
	auto counter = monsters[result.monster_id].DealDamage(damage,ignore_armor);
	if (counter.size() > 0) {
		Log("Debug", "A cast when damage taken happened");
		counter[0].SetCasterID(result.monster_id); //Remember caster id for later execution
		monster_actions_pending.push_back(counter[0]);
	}


	if (spell == 4) { //Life drain
		player_state.ChangeHp(damage); //Life steal for spell 4
	}


	//If we killed the currently selected enemy, move selection
	if (monsters[hovered_enemy].Alive() == false) { 
		ChangeHoveredEnemy(1);
	}

	RemakeUi();
}

void Fight::AnimationStep() {
	animations_running.Progress();
}






glm::mat4 Fight::GetViewMatrixForFight() const {
	float camera_x = 0.0f;
	float camera_y = -20.0f;
	float camera_z = 10.0f;
	glm::vec3 position(camera_x, camera_y, camera_z);

	glm::vec3 origin( //Our view target
		0.0f,
		0.0f,
		0.0f
	);

	glm::vec3 direction = -position;
	direction = glm::normalize(direction);

	glm::vec3 right = glm::vec3(
		1,
		0,
		0
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	return glm::lookAt(
		position,           // Camera is here
		origin, // and to the origin
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
};



glm::mat4 Fight::GetProjectionMatrixForFight() const {
	return glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
};

bool Fight::AreEnemiesAlive() const {
	return any_of(monsters.cbegin(), monsters.cend(), mem_fun_ref(&Monster::Alive));
}

void Fight::RemakeUi() {
	{ //Sidebar
		std::vector<int> a;
		a.push_back(hovered_spell);  //int parameter[0]: Which spell is hovered
		std::vector<float> b;
		std::vector<string> c;
		c.push_back(intToString(player_state.HitPoints()));  //To construct a text field that displays current hit points
		c.push_back(intToString(player_state.Shield()));  //To construct a text field that displays current hit points

		UiElementParameter new_para(a, b, c);
		sidebar.RemakeIfDifferent("../Data/Ui/Fight_Sidebar.xml", new_para);
	}

	{ //Enemy display
		if (hovered_enemy > -1) {
			std::vector<int> a;
			std::vector<float> b;
			std::vector<string> c;
			c.push_back(monsters[hovered_enemy].DisplayName());

			Stats m_stats = monsters[hovered_enemy].ReadStats();
			c.push_back(intToString(m_stats.Health()));  //To construct a text field that displays current hit points
			c.push_back(intToString(m_stats.Attack()));  //To construct a text field that displays current hit points
			c.push_back(intToString(m_stats.Armor()));  //To construct a text field that displays current hit points
			c.push_back(intToString(m_stats.Cast_Chance()));  //To construct a text field that displays current hit points

			c.push_back("Hp");  //To construct a text field that displays current hit points
			c.push_back("Attack");  //To construct a text field that displays current hit points
			c.push_back("Resist");  //To construct a text field that displays current hit points
			c.push_back("Cast");  //To construct a text field that displays current hit points

			UiElementParameter new_para(a, b, c);
			monster_info.RemakeIfDifferent("../Data/Ui/Fight_MonsterInfoBar.xml", new_para);
		}
	}

};