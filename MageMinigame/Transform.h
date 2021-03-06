#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tinyxml2.h"


class Transform{
public:
	explicit Transform();
	explicit Transform(float,float,float,float);
	explicit Transform(const tinyxml2::XMLElement *);

	float X() const{return pos_x;};
	float Y() const{return pos_y;};
	float Z() const{return pos_z;};
	float Scale() const { return scale; };

	void Move(float, float, float);
	void RotateX(float n_z);
	void RotateY(float n_z);
	void RotateZ(float n_z);
	void SetRotX(float n) { rot_x = n; cached_matrix_still_up_to_date = false; };
	void SetRotY(float n) { rot_y = n; cached_matrix_still_up_to_date = false; };
	void SetRotZ(float n) { rot_z = n; cached_matrix_still_up_to_date = false; };
	void setScale(float n) { scale = n; cached_matrix_still_up_to_date = false; };

	tinyxml2::XMLElement* MakeXML(tinyxml2::XMLDocument&) const;

	glm::mat4 MakeModelMatrix() const;

private:
	float pos_x;
	float pos_y;
	float pos_z;
	float rot_x;
	float rot_y;
	float rot_z;
	float scale;	

	mutable bool cached_matrix_still_up_to_date;
	mutable glm::mat4 cached_matrix;
};

