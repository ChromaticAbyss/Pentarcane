#include "TimerBar.h"

#include "../Log.h"

#include "../OpenGLContainer.h"


using namespace std;

TimerBar::TimerBar()
	:id(0),frames_left(0),frames_total(1),ui()
{
}

TimerBar::TimerBar(int id,int frames, std::string ui_name_)
	:id(id),frames_left(frames),frames_total(frames),ui_name(ui_name_),ui(nullptr)
{

	std::vector<int> a;
	std::vector<float> b;
	b.push_back(1.0f);
	std::vector<string> c;
	UiElementParameter new_para(a, b, c);
	ui = make_unique<UiElement>(ui_name, new_para);

	if (frames_total <= 1) {
		frames_total = 1;
		Log("Error", "We are creating a progressbar with length 0 or less frames!");
		//Halt, because this is an odd bug
	}
}

void TimerBar::Render(OpenGLContainer * open_gl) const
{
	if (!Done()) {
		ui->Render(glm::mat4(),open_gl);
	}


}


void TimerBar::Progress() {
	frames_left--;

	if (frames_left % 5 == 0) { //reduce frequency of updates as they wil be the same visually
		std::vector<int> a;
		std::vector<float> b;
		b.push_back((float)frames_left / frames_total);
		std::vector<string> c;

		UiElementParameter new_para(a, b, c);
		ui->RemakeIfDifferent(ui_name, new_para);
	}
}
