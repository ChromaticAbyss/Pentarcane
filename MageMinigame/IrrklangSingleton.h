#pragma once

#include "Log.h"
#include "irrklang.h"
#include "conio.h"
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll


class IrrklangSingleton
{
public:
	IrrklangSingleton()
	{
		if (!my_instance)
		{
			Log("Sound", "Could not start IrrKlangEngine");
		}
	}

	irrklang::ISoundEngine* Get() {
		return my_instance;
	};

private:
	static irrklang::ISoundEngine*  my_instance;
};

