#include "Transform2D.h"

#include "../Log.h"

using namespace std;
using namespace glm;
using namespace tinyxml2;

#include "../OpenGLContainer.h"

Transform2D::Transform2D()
	:pos_x(0),pos_y(0),rot(0),scale_x(1.0f),scale_y(1.0f),scale_y_is_relative_to_scale_x(false)
	,cached_matrix_still_up_to_date(false), cached_translation_matrix_still_up_to_date(false)
{
}

Transform2D::Transform2D(float x, float y, float sc_x, float sc_y, bool rel_sc)
	: pos_x(x), pos_y(y), rot(0), scale_x(sc_x), scale_y(sc_y), scale_y_is_relative_to_scale_x(rel_sc)
	, cached_matrix_still_up_to_date(false), cached_translation_matrix_still_up_to_date(false)
{


}

Transform2D::Transform2D(tinyxml2::XMLElement * xml_root, UiElementParameter param)
	: pos_x(0), pos_y(0), rot(0), scale_x(1.0f), scale_y(1.0f), scale_y_is_relative_to_scale_x(false)
	, cached_matrix_still_up_to_date(false), cached_translation_matrix_still_up_to_date(false)
{

	if (xml_root == 0) {
		Log("Error", "LabyrinthPartDefinition::LabyrinthPartDefinition xml_root is null");
		return;
	}


	{ //Look up x
		XMLElement * xml_child = xml_root->FirstChildElement("X");
		if (xml_child != 0) {
			pos_x = stof(xml_child->GetText());
		}
	}
	{ //Look up x: alternative way: from parameters
		XMLElement * xml_child = xml_root->FirstChildElement("UseFloatParamForX");
		if (xml_child != 0) {
			auto pos = stoi(xml_child->GetText());
			pos_x += param.Float_Value(pos);
		}
	}


	{ //Look up y
		XMLElement * xml_child = xml_root->FirstChildElement("Y");
		if (xml_child != 0) {
			pos_y = stof(xml_child->GetText());
		}
	}
	{ //Look up y: alternative way: from parameters
		XMLElement * xml_child = xml_root->FirstChildElement("UseFloatParamForY");
		if (xml_child != 0) {
			auto pos = stoi(xml_child->GetText());
			pos_y += param.Float_Value(pos);
		}
	}



	{ //Look up rot
		XMLElement * xml_child = xml_root->FirstChildElement("Rot");
		if (xml_child != 0) {
			rot = stof(xml_child->GetText());
		}
	}
	
	{ //Look up scale x
		XMLElement * xml_child = xml_root->FirstChildElement("SizeX");
		if (xml_child != 0) {
			scale_x = stof(xml_child->GetText());
		}
	}
	{ //Look up scale_x: alternative way: from parameters
		XMLElement * xml_child = xml_root->FirstChildElement("UseFloatParamForSizeX");
		if (xml_child != 0) {
			//cout << "Found size x scaler" << endl;
			auto pos = stoi(xml_child->GetText());
			//cout << "Scale applied: "<<param.Float_Value(pos) << endl;
			scale_x = scale_x * param.Float_Value(pos);
		}
	}

	{ //Look up scale y
		XMLElement * xml_child = xml_root->FirstChildElement("SizeY");
		if (xml_child != 0) {
			scale_y = stof(xml_child->GetText());
		}
	}
	{ //Look up scale_y: alternative way: from parameters
		XMLElement * xml_child = xml_root->FirstChildElement("UseFloatParamForSizeY");
		if (xml_child != 0) {
			auto pos = stoi(xml_child->GetText());
			scale_y = scale_y * param.Float_Value(pos);
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("SizeYRelativeToX");
		if (xml_child != 0) {
			string a = xml_child->GetText();
			if (a == "1" || a == "True" || a == "true") {
				scale_y_is_relative_to_scale_x = true;
			}
		}
	}
	
	{ //Look up scale xy
		XMLElement * xml_child = xml_root->FirstChildElement("Scale");
		if (xml_child != 0) {
			scale_y = scale_y * stof(xml_child->GetText());
			scale_x = scale_x * stof(xml_child->GetText());
		}
	}
	

}

Transform2D Transform2D::Merge(const Transform2D & other) const
{
	//Not using the ScaleY()  function here because we need the raw value instead of the one fitted to ScaleX()!
	float new_scale_y = scale_y*other.scale_y;

	//Case 1: The 2nd value is not to scale with Y, in this case divide by the others scale
	if (scale_y_is_relative_to_scale_x && !other.scale_y_is_relative_to_scale_x) {

		new_scale_y = new_scale_y / other.scale_x;
	}
	if (other.scale_y_is_relative_to_scale_x && !scale_y_is_relative_to_scale_x) {
		new_scale_y = new_scale_y / scale_x;
	}

	//TODO: Both to scale?
	if (other.scale_y_is_relative_to_scale_x && scale_y_is_relative_to_scale_x) {
		new_scale_y = new_scale_y; // / scale_x / scale_y;

	}


	Transform2D temp(X() + other.X(), Y() + other.Y(), ScaleX()*other.ScaleX(), new_scale_y, (scale_y_is_relative_to_scale_x || other.scale_y_is_relative_to_scale_x));


	return temp;
}





float Transform2D::ScaleYModded(OpenGLContainer* open_gl) const
{
	float m_scale_y = scale_y;

	//Allows squares atc that are square independant of viewport dimension
	//Also allows things to scale with width
	if (scale_y_is_relative_to_scale_x) {
		float window_ratio = open_gl->window_x / static_cast<float>(open_gl->window_y);
		m_scale_y = (scale_x * scale_y * window_ratio);
	}
	return m_scale_y;
}

void Transform2D::Move(float dx, float dy) {
	pos_x += dx;
	pos_y += dy;
	cached_matrix_still_up_to_date = false;
	cached_translation_matrix_still_up_to_date = false;
};


glm::mat4 Transform2D::MakeModelMatrix(OpenGLContainer * open_gl) const{
	if (cached_matrix_still_up_to_date) {
		return cached_matrix;
	}
	//Translation * Rotations * Scale <- they "apply" from right to left. This is important for the operations to result in what makes intuitive sense


	//Scale
	glm::mat4 Matrix = glm::scale(mat4(), vec3(scale_x, ScaleYModded(open_gl), 1.0f));
	

	//Rotations (from the left)		<-These alone make up half of the work that happens here
	if (rot != 0) {
		Matrix = Matrix * glm::rotate(mat4(), (rot)+0.0f, vec3(0.0f, 0.0f, 1.0f)) ;
	}

	//Translation (from the left)
	Matrix = glm::translate(mat4(), vec3(pos_x, pos_y, 0.0f)) * Matrix;



	cached_matrix = Matrix;
	cached_matrix_still_up_to_date = true;
	return Matrix;
}

glm::mat4 Transform2D::MakeTranslationMatrix() const {
	if (cached_translation_matrix_still_up_to_date) {
		return cached_translation_matrix;
	}
	//Translation (from the left)
	glm::mat4 Matrix = glm::translate(mat4(), vec3(pos_x, pos_y, 0.0f));

	cached_translation_matrix = Matrix;
	cached_translation_matrix_still_up_to_date = true;
	return Matrix;
}

