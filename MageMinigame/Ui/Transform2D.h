#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../tinyxml2.h"

#include "../Log.h"

#include "UiElementParameter.h"

class OpenGLContainer;

class Transform2D{
public:
	explicit Transform2D();
	explicit Transform2D(float,float,float,float,bool);
	explicit Transform2D(tinyxml2::XMLElement *, UiElementParameter param);

	Transform2D Merge(const Transform2D& other) const;

	float X() const{return pos_x;};
	float Y() const{return pos_y;};

	float ScaleX() const { return scale_x; };
	float ScaleY() const { return scale_y; };
	float ScaleYModded(OpenGLContainer *) const;
	void Rotate(float amt) {
		rot += amt; 
		cached_matrix_still_up_to_date = false; 	
		cached_translation_matrix_still_up_to_date = false;
	};

	float ScaleYAbs() const { return scale_y; }; //TODO: Outdated
	//bool IsScaleRelative() const { return scale_y_is_relative_to_scale_x; };

	void Move(float, float);

	glm::mat4 MakeModelMatrix(OpenGLContainer*) const;
	glm::mat4 MakeTranslationMatrix() const;

private:
	float pos_x;
	float pos_y;
	float rot;
	float scale_x;	

	float scale_y;
	bool scale_y_is_relative_to_scale_x;


	mutable bool cached_matrix_still_up_to_date;
	mutable glm::mat4 cached_matrix;
	mutable bool cached_translation_matrix_still_up_to_date;
	mutable glm::mat4 cached_translation_matrix;
};

