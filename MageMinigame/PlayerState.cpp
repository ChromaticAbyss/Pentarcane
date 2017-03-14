#include "PlayerState.h"

#include "Log.h"

using namespace std;

PlayerState::PlayerState() 
	:hit_points(15),max_hit_points(15),shield(0)
{
}

PlayerState::PlayerState(int hp)
	: hit_points(hp), max_hit_points(15),shield(0)
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
}
