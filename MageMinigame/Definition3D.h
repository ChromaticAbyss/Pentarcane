#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#define GLEW_STATIC
#include <GL/glew.h>
//#include <GL/glut.h>


#include "tinyxml2.h"
#include "Transform.h"
//#include "Instance3D.h"

#include "Color.h"

class OpenGLContainerWith3D;

class Definition3D{
	friend class Instance3D_New;
public:
	Definition3D();
	Definition3D(std::string,std::string,Transform);
	Definition3D(tinyxml2::XMLElement*);

	Definition3D(const Definition3D& o) = delete;
	Definition3D& operator=(const Definition3D& o) = delete;

	//void Render(glm::mat4 matrix, std::shared_ptr<OpenGLContainer> open_gl) const;
	void Render(glm::mat4 matrix, OpenGLContainerWith3D* open_gl, Color tint = Color(1, 1, 1, 1)) const;

	bool attemptToLoadModel();
private:
	void InitializeFromXml(tinyxml2::XMLElement * xml_root);


	int load_state; //0 not loaded, 1 loaded, 2 tried to load but failed
	int vertex_count;
	Transform transform;

	GLuint VertexArrayID; //VAO, this is a neccessary part, if left out nothing gets rendered.

	std::string model_file_name;
	std::string texture_file_name;
	
	GLuint texture;

	std::vector<std::unique_ptr<Definition3D>> children;  

	Color color;
};



