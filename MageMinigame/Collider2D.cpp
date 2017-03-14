#include "Collider2D.h"

#include "Log.h"

#include <cmath>
#include "Transform.h"

using namespace std;
using namespace tinyxml2;

Collider2D::Collider2D(float x, float y, float r)
	:type(0),centre_of_mass_x(x),centre_of_mass_y(y),range(r)
{}



Collider2D::Collider2D() 
	:type(0), centre_of_mass_x(0), centre_of_mass_y(0), range(1.0f), vertices()
{
}

Collider2D::Collider2D(int type_, tinyxml2::XMLElement * xml_root)
	:type(type_),centre_of_mass_x(0),centre_of_mass_y(0),range(1.0f),vertices()
{
	if (xml_root == 0) {
		Log("Error", "Attempting to create circular collider with nil pointer");
		return;
	}


	{ //Look up radius
		XMLElement * xml_child = xml_root->FirstChildElement("Radius");
		if (xml_child != 0) {
			range = stof(xml_child->GetText());
			if (type != 0) {
				Log("Error(Data)", "We just read a non circular collider with a radius. The value will not be used.");
			}
		}
	}

	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("X");
		if (xml_child != 0) {
			centre_of_mass_x = stof(xml_child->GetText());
		}
	}
	{
		XMLElement * xml_child = xml_root->FirstChildElement("Y");
		if (xml_child != 0) {
			centre_of_mass_y = stof(xml_child->GetText());
		}
	}

	if(type==1){ //Look up rect width/height
		float w = 0.01f;
		float h = 0.01f;
		XMLElement * xml_child = xml_root->FirstChildElement("Width");
		if (xml_child != 0) {
			w = stof(xml_child->GetText());
		}

		XMLElement * xml_child2 = xml_root->FirstChildElement("Height");
		if (xml_child2 != 0) {
			h = stof(xml_child2->GetText());
		}
		//Lower left
		vertices.push_back(centre_of_mass_x - w / 2);
		vertices.push_back(centre_of_mass_y - h / 2);
		//Lower right
		vertices.push_back(centre_of_mass_x + w / 2);
		vertices.push_back(centre_of_mass_y - h / 2);
		//Upper right
		vertices.push_back(centre_of_mass_x + w / 2);
		vertices.push_back(centre_of_mass_y + h / 2);
		//Upper left
		vertices.push_back(centre_of_mass_x - w / 2);
		vertices.push_back(centre_of_mass_y + h / 2);

		//This is the furthest a single point of the collider is outward. If two max dists don't touch, their colliders never will
		float cand_max_dist = 0;
		for (int i = 0; i < 7; i = i + 2) {
			float a = vertices[i] * vertices[i] + vertices[i+1] * vertices[i+1];
			if (a > cand_max_dist) {
				cand_max_dist = a;
			}
		}
		cand_max_dist = sqrt(cand_max_dist) * 2.0f; //Sqrt and add a tolerance factor to account for rotations
		UpdateRange(cand_max_dist);
	}
}


bool Collider2D::RangeCheck(Transform& my_transform, const Collider2D& other, Transform& other_transform) const {
	float r1 = range * my_transform.Scale();
	float r2 = other.range * other_transform.Scale();

	float d_x = (centre_of_mass_x + my_transform.X()) - (other.centre_of_mass_x + other_transform.X());
	float d_y = (centre_of_mass_y + my_transform.Y()) - (other.centre_of_mass_y + other_transform.Y());
	//if (d_x < 0) { d_x = -d_x; }
	//if (d_y < 0) { d_y = -d_y; }

	if(d_x*d_x + d_y*d_y < (r1 + r2) * (r1 + r2) ){ //Dist^2 < Range^2
		return true;
	}
	return false;
}


bool CheckEdgeVsCircle(float c_x, float c_y, float radius, float a_x, float a_y, float b_x, float b_y){
	//cout << "Edge from " << a_x << "|" << a_y << " to " << b_x << "|" << b_y << endl;
	float u_c = (c_x - a_x) * (b_x - a_x) + (c_y - a_y) * (b_y - a_y);
	float u_d = (b_x - a_x) * (b_x - a_x) + (b_y - a_y) * (b_y - a_y);

	if (u_d == 0) {
		//cout << "check edge vs circle: The two edge points are identical" << endl;
		return false;
	}

	float u = u_c / u_d;
	//cout << u << endl;
	//Check distance!
	if (u < 0) { u = 0;}
	if (u > 1) { u = 1;}

	float x = a_x + u * (b_x - a_x);
	float y = a_y + u * (b_y - a_y);

	float distance_squared = (x - c_x)*(x - c_x) + (y - c_y)*(y - c_y);
	//cout << "dist sqared " << distance_squared << endl;
	//cout << "radius squared " << radius*radius << endl;

	if (distance_squared < radius * radius) {
		return true;
	}



	return false;
}



bool Collider2D::CheckCollision(Transform& my_transform, const Collider2D& other, Transform& other_transform) const {

	if (RangeCheck(my_transform,other,other_transform)==false) {
		return false;
	}

	if (type == 0 && other.type == 0) {
		return true;//2 circles: Range chack is enough in this case
	}


	if (type == 0 && other.type == 1) {
		//TODO: Skip this step, make the vertices saved as glm::vec4's
		vector<float> verts = other.vertices;

		auto transf = other_transform.MakeModelMatrix();
		//grab the points of the collider and 
		glm::vec4 p_ll(verts[0],verts[1],0,1);
		glm::vec4 p_lr(verts[2],verts[3], 0, 1);
		glm::vec4 p_ur(verts[4],verts[5], 0, 1);
		glm::vec4 p_ul(verts[6],verts[7], 0, 1);

		p_ur = transf * p_ur;
		p_lr = transf * p_lr;
		p_ll = transf * p_ll;
		p_ul = transf * p_ul;
	
		//Check circle-centre in rectangle. This may cause no collisions with the walls and thus wouldn't be caught by the other tests!

		glm::vec4 modP(my_transform.X() - p_ll.x, my_transform.Y() - p_ll.y, 0, 1);
		glm::vec4 v1 = p_lr - p_ll;
		glm::vec4 v2 = p_ul - p_ll;

		
		float lambda1 = 0.5f;		
		float lambda2 = 0.5f;
		if (v1.x * v1.x + v1.y*v1.y != 0) {
			lambda1 = (modP.x * v1.x + modP.y * v1.y) / (v1.x * v1.x + v1.y*v1.y);
		}
		if (v2.x * v2.x + v2.y*v2.y != 0) {
			lambda2 = (modP.x * v2.x + modP.y * v2.y) / (v2.x * v2.x + v2.y*v2.y);
		}
		if (lambda1 >= 0 && lambda1 <= 1.0f && lambda2 >= 0 && lambda2 <= 1.0f) {
			Log("Debug","Circle center would be in rectangle");
			return true;
		}
		

		//Check vs the edges
		bool r1 = CheckEdgeVsCircle(my_transform.X(), my_transform.Y(), range, p_ll.x, p_ll.y, p_lr.x, p_lr.y);
		if (r1) { return true; }
		bool r2 = CheckEdgeVsCircle(my_transform.X(), my_transform.Y(), range, p_lr.x, p_lr.y, p_ur.x, p_ur.y);
		if (r2) { return true; }
		bool r3 = CheckEdgeVsCircle(my_transform.X(), my_transform.Y(), range, p_ur.x, p_ur.y, p_ul.x, p_ul.y);
		if (r3) { return true; }
		bool r4 = CheckEdgeVsCircle(my_transform.X(), my_transform.Y(), range, p_ul.x, p_ul.y, p_ll.x, p_ll.y);
		if (r4) { return true; }


		return false;
	}
	if (type == 1 && other.type == 0) {
		//cout << "CheckCollision: Swapping Pair!" << endl;
		return other.CheckCollision(other_transform,*this,my_transform);//Reverse case: swap objects and call, to avoid code duplication
	}
	


	return false;
}

