#include "Transform.h"

#include "Log.h"

using namespace std;
using namespace glm;
;

Transform::Transform()
	:pos_x(0),pos_y(0),pos_z(0),rot_x(0),rot_y(0),rot_z(0),scale(1.0f), cached_matrix_still_up_to_date(false)
{
}

Transform::Transform(float x, float y, float z, float sc)
	:pos_x(x), pos_y(y), pos_z(z), rot_x(0), rot_y(0), rot_z(0), scale(sc), cached_matrix_still_up_to_date(false)
{
}

Transform::Transform(const tinyxml2::XMLElement * xml_root)
	: pos_x(0), pos_y(0), pos_z(0), rot_x(0), rot_y(0), rot_z(0), scale(1.0f), cached_matrix_still_up_to_date(false)
{

	if (xml_root == 0) {
		Log("Error", "LabyrinthPartDefinition::LabyrinthPartDefinition xml_root is null");
		return;
	}


	{ //Look up x
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("X");
		if (xml_child != 0) {
			pos_x = stof(xml_child->GetText());
		}
	}
	{ //Look up y
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Y");
		if (xml_child != 0) {
			pos_y = stof(xml_child->GetText());
		}
	}
	{ //Look up z
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Z");
		if (xml_child != 0) {
			pos_z = stof(xml_child->GetText());
		}
	}


	{ //Look up rot_x
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("RotX");
		if (xml_child != 0) {
			rot_x = stof(xml_child->GetText());
		}
	}
	{ //Look up rot_y
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("RotY");
		if (xml_child != 0) {
			rot_y = stof(xml_child->GetText());
		}
	}
	{ //Look up rot_z
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("RotZ");
		if (xml_child != 0) {
			rot_z = stof(xml_child->GetText());
		}
	}

	{ //Look up scale
		const tinyxml2::XMLElement * xml_child = xml_root->FirstChildElement("Scale");
		if (xml_child != 0) {
			scale = stof(xml_child->GetText());
		}
	}

}



void Transform::Move(float dx, float dy, float dz) {
	pos_x += dx;
	pos_y += dy;
	//cout << "Moved by:" << dy << endl;
	pos_z += dz;
	cached_matrix_still_up_to_date = false;
}

void Transform::RotateZ(float n_z)
{
	rot_z += n_z;
	if (rot_z > 360) { rot_z -= 360; }
	if (rot_z < 0) { rot_z += 360; }
	cached_matrix_still_up_to_date = false;
}
void Transform::RotateX(float n_z)
{
	rot_x += n_z;
	if (rot_x > 360) { rot_x -= 360; }
	if (rot_x < 0) { rot_x += 360; }
	cached_matrix_still_up_to_date = false;
}
void Transform::RotateY(float n_z)
{
	rot_y += n_z;
	if (rot_y > 360) { rot_y -= 360; }
	if (rot_y < 0) { rot_y += 360; }
	cached_matrix_still_up_to_date = false;
}



glm::mat4 Transform::MakeModelMatrix() const{
	if (cached_matrix_still_up_to_date) {
		return cached_matrix;
	}

	//Translation * Rotations * Scale <- they "apply" from right to left. This is important for the operations to result in what makes intuitive sense
	
	//Scale
	glm::mat4 Matrix = glm::scale(mat4(), vec3(scale, scale, scale)); 
	
	//Rotations (from the left)		<-These alone make up half of the work that happens here
	Matrix = glm::rotate(mat4(), (rot_x) + 0.0f, vec3(1.0f, 0.0f, 0.0f)) * Matrix;
	Matrix = glm::rotate(mat4(), (rot_y) + 0.0f, vec3(0.0f, 1.0f, 0.0f)) * Matrix;
	Matrix = glm::rotate(mat4(), (rot_z) + 0.0f, vec3(0.0f, 0.0f, 1.0f)) * Matrix;


	//Translation (from the left)
	Matrix = glm::translate(mat4(), vec3(pos_x, pos_y, pos_z)) * Matrix;
	
	cached_matrix = Matrix;
	cached_matrix_still_up_to_date = true;
	return Matrix;
}


tinyxml2::XMLElement* Transform::MakeXML(tinyxml2::XMLDocument & doc) const {
	tinyxml2::XMLElement * return_element = doc.NewElement("Transform");

	{
		tinyxml2::XMLElement * type_element = doc.NewElement("X");
		type_element->SetText(X());
		return_element->InsertEndChild(type_element);
	}

	{
		tinyxml2::XMLElement * type_element = doc.NewElement("Y");
		type_element->SetText(Y());
		return_element->InsertEndChild(type_element);
	}

	{
		tinyxml2::XMLElement * type_element = doc.NewElement("Z");
		type_element->SetText(Z());
		return_element->InsertEndChild(type_element);
	}


	
	if (rot_x != 0) {
		tinyxml2::XMLElement * type_element = doc.NewElement("RotX");
		type_element->SetText(rot_x);
		return_element->InsertEndChild(type_element);
	}

	if (rot_y != 0) {
		tinyxml2::XMLElement * type_element = doc.NewElement("RotY");
		type_element->SetText(rot_y);
		return_element->InsertEndChild(type_element);
	}

	if (rot_z != 0) {
		tinyxml2::XMLElement * type_element = doc.NewElement("RotZ");
		type_element->SetText(rot_z);
		return_element->InsertEndChild(type_element);
	}

	{
		tinyxml2::XMLElement * type_element = doc.NewElement("Scale");
		type_element->SetText(Scale());
		return_element->InsertEndChild(type_element);
	}

	return return_element;
};