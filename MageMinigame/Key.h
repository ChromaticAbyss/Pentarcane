#pragma once

#include <glfw3.h>

#include "glm\glm.hpp"

class Key
{
public:
	Key(int);
	
	void BlockUntilReleased() { block_until_released = true; };

	bool Update(GLFWwindow* window);

private:
	int key_nr;
	bool key_was_down;
	bool block_until_released;
};

