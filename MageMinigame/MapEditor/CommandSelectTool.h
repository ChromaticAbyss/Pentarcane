#pragma once

#include "../Ui/Command.h";

class CommandSelectTool : public Command {
public:
	CommandSelectTool(int t, bool& a, int& b) :tool(t), trigger_rebuild_ui(a), selected_tool(b) {};

	virtual void execute() {
		trigger_rebuild_ui = true; 
		selected_tool = tool;
	}

private: 
	int tool;
	bool & trigger_rebuild_ui;
	int & selected_tool;

};