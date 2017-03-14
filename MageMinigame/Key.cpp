#include "Key.h"

Key::Key(int k)
	:key_nr(k),key_was_down(false),block_until_released(true)
{
}

bool Key::Update(GLFWwindow * window)
{
	if (glfwGetKey(window, key_nr) == GLFW_PRESS) {
		key_was_down = true;
	} else {
		if (block_until_released) { //Ignore the next lifting of that key
			block_until_released = false;
			key_was_down = false;
			return false;
		}

		if (key_was_down) { //Message when 
			key_was_down = false;
			return true;
		} 
	}

	return false;
}

