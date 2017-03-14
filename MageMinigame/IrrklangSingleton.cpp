#include "IrrklangSingleton.h"

using namespace irrklang;


irrklang::ISoundEngine* IrrklangSingleton::my_instance = irrklang::createIrrKlangDevice();
