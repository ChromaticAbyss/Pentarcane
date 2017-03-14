#include "Definition3D.h"
#include "StringUtility.h"

#include <string>
#include <vector>

#include "Log.h"

#include "common/objloader.hpp"
#include "common/texture.hpp"

#include "common/quaternion_utils.hpp" // See quaternion_utils.cpp for RotationBetweenVectors, LookAt and RotateTowards

#include "OpenGLContainer.h"

using namespace glm;
using namespace std;
using namespace tinyxml2;


Definition3D::Definition3D()
	:load_state(0),vertex_count(0),transform(),model_file_name("../Data/3D/MissingModelMarker.obj"),texture_file_name("../Data/2D/MissingTexture.png"),children()
{
}

Definition3D::Definition3D(string file_3d, string file_texture, Transform t)
	:load_state(0), vertex_count(0), transform(t), model_file_name(file_3d), texture_file_name(file_texture), children()
{

	//attemptToLoadModel();
}


Definition3D::Definition3D(XMLElement* xml_root)
	:load_state(0),vertex_count(0),transform(),model_file_name(""),texture_file_name("../Data/2D/MissingTexture.png"),children()
{
	InitializeFromXml(xml_root);

	//attemptToLoadModel();
}

void Definition3D::InitializeFromXml(tinyxml2::XMLElement * xml_root){
	if (xml_root == 0) {
		Log("Error", "Definition3D::Definition3D xml_root is null");
		return;
	}



	{
		XMLElement * xml_child = xml_root->FirstChildElement("Transform");
		if (xml_child != 0) {
			transform = Transform(xml_child);
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("ModelFile");
		if (xml_child != 0) {
			model_file_name = xml_child->GetText();
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("TextureFile");
		if (xml_child != 0) {
			texture_file_name = xml_child->GetText();
		}
	}


	{
		XMLElement * xml_child = xml_root->FirstChildElement("Children");
		while (xml_child != 0) {

			children.push_back(make_shared<Definition3D>(Definition3D(xml_child)));	//Does this work?

			xml_child = xml_child->NextSiblingElement("Children");
		}
	}

};

bool Definition3D::attemptToLoadModel(){
	//TODO: Check if string empty or other shenanigans
	if (model_file_name == "") { return false; }

	//Load from file
	std::vector<glm::vec3> T_vertices;
	std::vector<glm::vec2> T_uvs;
	std::vector<glm::vec3> T_normals;
	cout << "Attempting to load model with name " << model_file_name << endl;
	bool res = loadOBJ(model_file_name, T_vertices, T_uvs, T_normals);
	if (res == false) { 
		load_state = 2; //tried to load but failed
		return false; 
	}
	vertex_count = T_vertices.size() + 1;

	//Make a Vao
	glGenVertexArrays(1, &VertexArrayID);//Create 1 Vao and store its id in the GLuint
	glBindVertexArray(VertexArrayID);

	//Store in Vao
	GLuint PerInstance_vertexbuffer;
	glGenBuffers(1, &PerInstance_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PerInstance_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, T_vertices.size() * sizeof(glm::vec3), &T_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);



	GLuint PerInstance_uvbuffer;
	glGenBuffers(1, &PerInstance_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PerInstance_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, T_uvs.size() * sizeof(glm::vec2), &T_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	GLuint PerInstance_normalbuffer;
	glGenBuffers(1, &PerInstance_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PerInstance_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, T_normals.size() * sizeof(glm::vec3), &T_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);



	glBindVertexArray(0);
	//Important to avoid memory leak
	glDeleteBuffers(1, &PerInstance_normalbuffer);
	glDeleteBuffers(1, &PerInstance_vertexbuffer);
	glDeleteBuffers(1, &PerInstance_uvbuffer);

	load_state = 1; //successfully loaded


	cout << "attempting to load texture:" <<texture_file_name<< endl;




	//Log("Data","Loading 3d element texture ["+renderDefinition.textureFileName+"] on the fly");
	texture = loadPNG_stb(texture_file_name.c_str());
	if (texture == 0) {
		Log("Error(Data)", "Failure loading texture: " + texture_file_name);
		return false;
	}


	//cout<<"Attempting to load texture with name"<<textureFileName<<endl;
	//texture = loadDDS("../3D/uvmap.DDS");

	cout<<"Sucessfully loaded!"<<endl;
	return true;
}


void Definition3D::Render(glm::mat4 matrix, OpenGLContainer* open_gl) const {
//void Definition3D::Render(glm::mat4 matrix, std::shared_ptr<OpenGLContainer> open_gl) const {


	//cout << "Definition3D: render called" << endl;
	glBindVertexArray(VertexArrayID);

	// 1st buffer: Vertices
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);


	//----------------------This step is expensive! Mostly the grabbing from the function----------------
	glm::mat4 ModelMatrix = matrix * transform.MakeModelMatrix(); //The matrix on the right(definition model matrix) applies first, then the model spcific one 



	//PV from gl_container since it doesn't change between objects
	//MAYBE:Possibly roll this into the matrix we get as input?
	glm::mat4 MVP = open_gl->PV * ModelMatrix;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform


	glUniformMatrix4fv(open_gl->MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform


	glUniformMatrix4fv(open_gl->MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(open_gl->ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(open_gl->ViewMatrixID, 1, GL_FALSE, &open_gl->ViewMatrix[0][0]); //View matrix into opengl_container?


	//-------------Set up texture stuff------------------
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(open_gl->TextureID, 0);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);

}

