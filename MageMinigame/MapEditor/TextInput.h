#pragma once

/*
#include <string>
#include <glfw3.h>

#include "../Log.h"


namespace TextInputForMapEditor {

	extern std::string input_so_far;
	extern bool input_allowed;
	extern bool input_done;


	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (!input_allowed) {
			return;
		}

		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			Log("Debug", "Enter was pressed!");
			input_done = true;
			input_allowed = false;
			return;
		}

		//std::cout << key << "|" << (char)key << std::endl;

		if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
			Log("Debug", "BS was pressed!");
			input_so_far.pop_back();
			std::cout << "|" << input_so_far << std::endl;
			return;
		}


		input_so_far = input_so_far + (char)key;
		std::cout << "|" << input_so_far << std::endl;


	}





}

*/