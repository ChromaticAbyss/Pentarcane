
#ifndef THREEDINSTANCE_NEW_H
#define THREEDINSTANCE_NEW_H

#include <vector>
#include <string>
#include <functional> //For reference_wrapper
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <GL/glew.h>

#include "Transform.h"
#include "Definition3D.h"
class OpenGLContainer;

class Instance3D_New{
public:
	Instance3D_New();
	Instance3D_New(std::shared_ptr<Definition3D> temp, Transform = Transform());
	static Instance3D_New MakeRogueModel(Transform, std::string, std::string);//Loads the model once for EACH instance!!


	//bool Render(glm::mat4 matrix,std::shared_ptr<OpenGLContainer> open_gl) const;
	bool Render(glm::mat4 matrix, OpenGLContainer* open_gl) const;


	bool Move(float,float,float);
	bool MoveTo(float,float,float);


	bool marked_for_cleanup;
	bool hidden;	

private:
	std::vector<Instance3D_New> children;

    std::shared_ptr<Definition3D> definition; 

	Transform transform;
};



#endif
