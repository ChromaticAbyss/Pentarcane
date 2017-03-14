#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Transform2D.h"


#include <glm/glm.hpp>

#include <GL/glew.h>

#include "../tinyxml2.h"

#include "OffsetElement.h"

class OpenGLContainer;

class Instance2D{
public:
	explicit Instance2D();
	explicit Instance2D(std::string image_name_, Transform2D transf);
	explicit Instance2D(tinyxml2::XMLElement * xml_root); //Instance2D could use a factory

	explicit Instance2D(int mode, std::string text, Transform2D transf, OffsetElement = OffsetElement(), float max_box_size_x = -1); //mode 0: Build from text

	void Render(glm::mat4,OpenGLContainer*) const;

	void Rotate(float amt) {
		transform.Rotate(amt);
	};

private:
	void MakeSimpleRectangleMesh();
	bool MakeModel(std::vector<glm::vec3>, std::vector<glm::vec2>);

	bool AttemptToLoadImage();
	std::vector<glm::vec3> letter_vertices(float widthOfThisChar, float rel_x);
	std::vector<glm::vec2> letter_uvs(float widthOfThisChar, char letter);


	std::string type;

	//Visibility properties
	bool marked_for_cleanup;
	bool hidden;
	int layer;

	//Where is it?
	Transform2D transform;
	float size_x;
	float size_y;

	//What is it?
	int vertex_count;
	GLuint VertexArrayID;

	std::string image_name;
	GLuint image;

	//TODO: Color object similar to Transform
	bool grey;
	float colour_r;
	float colour_g;
	float colour_b;
	float transparency;
	OffsetElement offset;



};






