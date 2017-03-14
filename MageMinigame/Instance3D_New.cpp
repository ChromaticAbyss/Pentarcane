#include "Instance3D_New.h"
#include "Definition3D.h"

#include "StringUtility.h"

#include <string>
#include <vector>

#include "Log.h"

#include "common/objloader.hpp"
#include "common/texture.hpp"

#include "common/quaternion_utils.hpp" // See quaternion_utils.cpp for RotationBetweenVectors, LookAt and RotateTowards
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include "OpenGLContainer.h"

using namespace glm;
using namespace std;


//The default cosntructor creates an empty, hidden object which points to no Definition!
Instance3D_New::Instance3D_New() //We can safely assume the vector containing hte model defs never changes because we only spawn instances after all definitions are loaded(definitions never change at runtime)!
	:marked_for_cleanup(true), hidden(true), definition(shared_ptr<Definition3D>(0)), transform()
{
}


//TODO: re order base member initialisations!
Instance3D_New::Instance3D_New(std::shared_ptr<Definition3D> p_definition, Transform t_) //We can safely assume the vector containing hte model defs never changes because we only spawn instances after all definitions are loaded(definitions never change at runtime)!
:marked_for_cleanup(false),hidden(false),definition(p_definition),transform(t_)
{
	
	unsigned int size = p_definition->children.size();
	if(size>0){
		for(unsigned int i=0;i < size;i++){
			//cout<<"Instance3D_New: Making child "<<i<<endl;
			Instance3D_New child_temp(p_definition->children[i]);
			children.push_back(child_temp);
		}
	}

	//cout<<"COUNT CHILDREN(instance)"<<children.size()<<endl;
};

Instance3D_New Instance3D_New::MakeRogueModel(Transform transf, std::string model_name, std::string texture_name) //Loads the model once for EACH instance!!
{
	auto def = std::make_shared<Definition3D>(model_name,texture_name,Transform());
	def->attemptToLoadModel();
	return Instance3D_New(def,transf);
}


bool Instance3D_New::Move(float x,float y,float z){
	transform.Move(x,y,z);
	/*
	unsigned int size = children.size();
	for(unsigned int i=0;i < size;i++){
		children[i].Move(x,y,z);
	}
	*/
	return true;
};

bool Instance3D_New::MoveTo(float x,float y,float z){
	transform.Move(x - transform.X(), y - transform.Y(), z - transform.Z());
	return true;
}; 


//bool Instance3D_New::Render(glm::mat4 matrix, std::shared_ptr<OpenGLContainer> open_gl) const {
bool Instance3D_New::Render(glm::mat4 matrix,OpenGLContainer* open_gl) const{
	//Skip rendering of object and children if its hidden or marked for cleanup or the definition is a nullpointer
	if(marked_for_cleanup==true || hidden==true || definition==0){
		return false;
	}

	//cout << "Instance3D_New: Render called" << endl;
	glm::mat4 Temp_ModelMatrix = matrix * transform.MakeModelMatrix();



	definition->Render(Temp_ModelMatrix,open_gl);


	/*
	//Recursion
	//TODO: pass over transformation matrices, not sure how exactly at the moment
	for(unsigned int i=0;i<children.size();i++){
		//cout<<"Instance3D_New: Rendering child"<<i<<endl;
		children[i].RenderModel(PV, MatrixID, ViewMatrixID, ModelMatrixID, TextureID); //Make respective children TODO
	}
	*/

	return true;
};
