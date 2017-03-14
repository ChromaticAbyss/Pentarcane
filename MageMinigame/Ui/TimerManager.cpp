#include "TimerManager.h"

#include <algorithm>
#include <functional>

using namespace std;

TimerManager::TimerManager()
	:bars()
{
}

void TimerManager::AddBar(int id, int frames, std::string name)
{
//	TimerBar temp(id, frames, name);
	//Remove a potentially existing bar with the same name
	bars.remove_if([id](const TimerBar& t) {return t.Id() == id; });

	bars.emplace_back(id, frames, name);

}

void TimerManager::Progress()
{
	//Progress all
	for_each(bars.begin(), bars.end(), mem_fun_ref(&TimerBar::Progress));
	//Drop all that are finished. 
	//Using the member function remove_if gets rid of the finished bars in an effective way and in a single step
	bars.remove_if(mem_fun_ref(&TimerBar::Done));

}

void TimerManager::Render(OpenGLContainer * open_gl) const
{

	for_each(bars.cbegin(), bars.cend(), [open_gl](const TimerBar& t) {t.Render(open_gl); });
}
