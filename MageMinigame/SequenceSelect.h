#pragma once

#include <vector>
#include <string>

#include "Instance3D_New.h"

#include "Ui/UiElement.h"

#include "Key.h"

#include "SequenceRepresentation.h"

class OpenGLContainer;


class SequenceSelect
{
public:
	SequenceSelect(OpenGLContainer* open_gl_, const std::vector<std::string>& list_of_sequences_, const std::vector<std::string>& unlocks);


	std::string Run();


private:
	//Member functions
	void Render();
	void RenderUi();
	void RenderBackground();
	glm::mat4 GetViewMatrix() const;
	void RemakeUi();

	//Fields
	int pos;
	std::vector<SequenceRepresentation> sequences;

	Instance2D spinning_symbol; //TODO: A 2d element would also do nicely with a new 2D "background render" call.
	Instance2D controls_display;

	OpenGLContainer* open_gl;

	UiElement name_display;
	Key key_esc;
	Key key_l;
	Key key_r;
	Key key_enter;

};


