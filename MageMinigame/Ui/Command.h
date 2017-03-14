#pragma once

//The idea is to later implement a version of the Ui System with the command pattern in place.
//TODO: call with reference to the current UiElement

class Command{
public:
	virtual ~Command() {};
	virtual void execute() = 0;
};


