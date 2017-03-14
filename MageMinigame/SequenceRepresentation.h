#pragma once



#include <vector>
#include <string>

#include "Instance3D_New.h"

#include "Ui/UiElement.h"

#include "Key.h"

class OpenGLContainer;


class SequenceRepresentation {
public:
	SequenceRepresentation(const std::string&);

	std::string FileName() const { return internal_name; };
	std::string DisplayName() const { return display_name; };
	std::vector<std::string> Flavour() const; //returns (locked) or the flavour text

	void Render(OpenGLContainer* open_gl);

	void SetUnlockState(const std::vector<std::string>& names);

	bool Unlocked() const { return unlock_state > 0; };
	bool Completed() const { return unlock_state == 2; };

	std::vector<std::string> GetRewards();

	void CompleteScreen(OpenGLContainer* open_gl);

private:
	bool Fulfilled(int i, const std::vector<std::string>& names) const;

	std::string internal_name;
	std::string display_name;
	std::vector<std::string> description;

	std::vector<Instance3D_New> models;//Can have multiple models: Unlocked(Regular), Locked(Painting?), Finished(???)

	std::vector<std::string> unlock_requirements;
	std::string complete_requirement;
	std::vector<std::string> rewards;

	int unlock_state;
	float rotation;
};