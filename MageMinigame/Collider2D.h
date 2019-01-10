#pragma once

#include <vector>
#include "tinyxml2.h"

class Transform;

class Collider2D{
public:
	explicit Collider2D(float,float,float);
	explicit Collider2D(int, tinyxml2::XMLElement *);
	explicit Collider2D();

	bool RangeCheck(const Transform&, const Collider2D&, const Transform&) const;
	bool CheckCollision(const Transform& ,const Collider2D&, const Transform&) const;
	void UpdateRange(float r) { range = r; };


private:
	int type;//0 Sphere, 1 Rectangle
	float centre_of_mass_x;//Position
	float centre_of_mass_y;//Position
	float range; //Maximum range of influence, basically a boundary circle. Two colliders outside their ranges need not be checked.

	std::vector<float> vertices;
};

