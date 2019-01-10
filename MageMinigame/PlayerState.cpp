#include "PlayerState.h"

#include "Log.h"

using namespace std;

const int PlayerState::NrTempEffects = 5;

PlayerState::TempEffect PlayerState::TempEffectNameToID(std::string name)
{
	if (name == "Brittle") {
		return Brittle;//a single hit kills
	}
	if (name == "Confused" || name == "Confuse") {
		return Confused;//invert arrow keys
	}
	if (name == "Slow" || name == "Slowed") {
		return Slow; //-25% player movement speed
	}
	if (name == "Panicked" || name == "Panic") {
		return Slow;//-35% time 
	}
	if (name == "Poison" || name == "Poisoned") {
		return Poison;//player loses 1 hp each tick
	}
	Log("Error", "Not a valid temp effect: " + name);
	return Brittle;
}

std::string PlayerState::TempEffectToIconFile(TempEffect eff)
{
	switch (eff) {
	case Brittle:
		return "../Data/2D/Ui/TempEffects/Brittle.png";
	case Confused:
		return "../Data/2D/Ui/TempEffects/Confused.png";
	case Panicked:
		return "../Data/2D/Ui/TempEffects/Panicked.png";
	case Poison:
		return "../Data/2D/Ui/TempEffects/Poison.png";
	case Slow:
		return "../Data/2D/Ui/TempEffects/Slow.png";
	}
	Log("Error", "Unknown type");
	return "";
}

PlayerState::PlayerState()
	:hit_points(15),max_hit_points(15),shield(0),block_fireball(0),block_lightning(0),block_ice(0),block_lifesteal(0), potion_count(0), potion_boost(0)
	,chaos_lightning_dmg(0), chaos_freeze_dmg(0), chaos_lifedrain_dmg(0)
	, temp_effect_durations(NrTempEffects, 0)
{
}



PlayerState::PlayerState(int hp)
	: hit_points(hp), max_hit_points(15),shield(0),block_lightning(0), block_ice(0), block_lifesteal(0), potion_count(0), potion_boost(0)
	, chaos_lightning_dmg(0), chaos_freeze_dmg(0), chaos_lifedrain_dmg(0)
	, temp_effect_durations(NrTempEffects,0)
{
}



void PlayerState::ChangeHp(int d) {
	if (d < 0) { //If damage, apply shield if available
		if (shield > -d) {
			shield += d;
			return;
		}
		d += shield;
		shield = 0;
	} 

	hit_points += d;
	if (hit_points > max_hit_points) {
		shield = shield + hit_points - max_hit_points;
		hit_points = max_hit_points;
	}
}

void PlayerState::BeginningOfTurn()
{
	Log("Debug", "New turn for player");
	shield = 0;
	block_fireball--;
	block_lightning--;
	block_ice--;
	block_lifesteal--;
	if (temp_effect_durations[Poison]>0) {
		--hit_points;
	}
	for (auto& temp : temp_effect_durations) {
		--temp;
	}
}

void PlayerState::a_fight_has_ended()
{
	//Perform cleanup of states that should not persist between Fights
	block_fireball=0;
	block_lightning=0;
	block_ice=0;
	block_lifesteal=0;
	for (auto& temp : temp_effect_durations) {
		temp = 0;
	}
}

void PlayerState::block_spell(Spell spell, int duration)
{
	if (spell == Spell::Fire()) {
		block_fireball += duration;
	}
	if (spell == Spell::Lightning()) {
		block_lightning += duration;
	}
	if (spell == Spell::Ice()) {
		block_ice += duration;
	}
	if (spell == Spell::Lifesteal()) {
		block_lifesteal += duration;
	}
}

bool PlayerState::is_spell_blocked(Spell spell) const
{
	if (spell == Spell::Fire()) {
		return block_fireball > 0;
	}
	if (spell == Spell::Lightning()) {
		return block_lightning > 0;
	}
	if (spell == Spell::Ice()) {
		return block_ice > 0;
	}
	if (spell == Spell::Lifesteal()) {
		return block_lifesteal > 0;
	}
}

std::vector<std::string> PlayerState::labyrinth_parameters() const
{
	auto result = std::vector<std::string>();
	if (temp_effect_durations[Slow] > 0) {
		result.push_back("Slow");
	}
	if (temp_effect_durations[Brittle] > 0) {
		result.push_back("Brittle");
	}
	if (temp_effect_durations[Confused] > 0) {
		result.push_back("Confused");
	}
	if (temp_effect_durations[Panicked] > 0) {
		result.push_back("Panicked");
	}
	return result;
}



int PlayerState::get_spell_chaos_bonus(Spell spell)
{
	if (spell == Spell::Lightning()) { 
		int te = chaos_lightning_dmg;
		chaos_lightning_dmg = 0;
		return te;
	}

	if (spell == Spell::Ice()) {
		int te = chaos_freeze_dmg;
		chaos_freeze_dmg = 0;
		return te;
	}

	if (spell == Spell::Lifesteal()) {
		int te = chaos_lifedrain_dmg;
		chaos_lifedrain_dmg = 0;
		return te;
	}

	return 0;
}

void PlayerState::add_temp_effect(TempEffect effect, int duration)
{
	int& val = temp_effect_durations[effect];
	if (val <= 0) {
		val = duration;
		return;
	}
	val += duration;
}
