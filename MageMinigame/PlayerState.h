#pragma once

class PlayerState {
public:
	explicit PlayerState();
	explicit PlayerState(int);

	int HitPoints() const { return hit_points; };
	int Shield() const { return shield; };
	void AddShield(int a) { shield += a; };
	void ChangeHp(int);

	void BeginningOfTurn();

private:
	int hit_points;
	int max_hit_points;
	int shield;
};