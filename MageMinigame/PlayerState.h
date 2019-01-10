#pragma once

#include <vector>
#include <string>
#include "Spell.h"

class PlayerState {
public:
	enum TempEffect {Slow,Brittle,Confused,Panicked,Poison};
	const static int NrTempEffects;
	static TempEffect TempEffectNameToID(std::string);
	static std::string TempEffectToIconFile(TempEffect);



	explicit PlayerState();
	explicit PlayerState(int);

	int HitPoints() const { return hit_points; };
	int Shield() const { return shield; };
	void AddShield(int a) { shield += a; };
	void ChangeHp(int);

	void BeginningOfTurn();
	void a_fight_has_ended();

	void block_spell(Spell spell, int duration);
	bool is_spell_blocked(Spell spell) const;
	bool AllSpellsBlocked() const{ return (block_fireball > 0 && block_lightning > 0 && block_ice > 0 && block_lifesteal > 0); };

	std::vector<std::string> labyrinth_parameters() const;


	void addChaosFrost(int amt) { chaos_freeze_dmg += amt; };
	void addChaosLightning(int amt) { chaos_lightning_dmg += amt; };
	void addChaosLifedrain(int amt) { chaos_lifedrain_dmg += amt; };
	int get_spell_chaos_bonus(Spell spell);


	void add_temp_effect(TempEffect effect, int duration);
	bool has_effect(TempEffect eff) const {
		return temp_effect_durations[eff] > 0;
	};
	std::vector<int> temp_effects_with_durations() const {
		return temp_effect_durations;
	}

private:
	int hit_points;
	int max_hit_points;
	int shield;

	int block_fireball;
	int block_lightning;
	int block_ice;
	int block_lifesteal;
	int potion_count;
	int potion_boost;

	int chaos_lightning_dmg;
	int chaos_freeze_dmg;
	int chaos_lifedrain_dmg;

	std::vector<int> temp_effect_durations;
};