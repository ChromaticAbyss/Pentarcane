
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
class OpenGLContainerWith3D;

class Instance3D_New{
public:
	Instance3D_New();
	Instance3D_New(const Definition3D* temp, Transform = Transform());
	static Instance3D_New MakeRogueModel(Transform, std::string, std::string);//Loads the model once for EACH instance!!

	Instance3D_New(Instance3D_New&& other) = default;
	Instance3D_New& operator=(Instance3D_New&& other) = default;


	Instance3D_New(const Instance3D_New& other);
	Instance3D_New& operator=(const Instance3D_New& other);


	//bool Render(glm::mat4 matrix,std::shared_ptr<OpenGLContainer> open_gl) const;
	bool Render(glm::mat4 matrix, OpenGLContainerWith3D* open_gl) const;

	inline void setColorMod(Color col) { tint = col; };

	bool Move(float,float,float);
	bool MoveTo(float,float,float);

	void RotateX(float amt) { transform.RotateX(amt); };
	void RotateY(float amt) { transform.RotateY(amt); };
	void RotateZ(float amt) { transform.RotateZ(amt); };
	void SetRotX(float amt) { transform.SetRotX(amt); };
	void SetRotY(float amt) { transform.SetRotY(amt); };
	void SetRotZ(float amt) { transform.SetRotZ(amt); };
	void setScale(float sc) { transform.setScale(sc); };

	bool marked_for_cleanup;
	bool hidden;	

private:
	std::vector<Instance3D_New> children;

   // std::shared_ptr<Definition3D> definition; 
	const Definition3D* definition;

	std::unique_ptr<Definition3D> inner_definition; //For rogue models

	Transform transform;
	Color tint;
};



#endif
