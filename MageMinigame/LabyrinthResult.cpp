#include "LabyrinthResult.h"

using namespace std;

LabyrinthResult::LabyrinthResult()
	:state(state_keys::running),monster_id(0),goals(0),bonus_effects(0),fast(false)
{}


LabyrinthResult::LabyrinthResult(LabyrinthResult::state_keys s, int m, int g, vector<string> b)
	: state(s), monster_id(m), goals(g), bonus_effects(b),fast(false)
{

}