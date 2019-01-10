#include "TimerBar.h"

#include "../Log.h"

#include "OpenGLContainerWith3D.h"

#include "group.h"
#include "BasicUiElement.h"


TimerBar::TimerBar(OpenGLContainer * open_gl)
	:mode(Spell::Fire()),frames_left(0),frames_total(1),ui(), open_gl(open_gl)
{
}


TimerBar::TimerBar(int frames, Spell mode, OpenGLContainer * open_gl)
	:mode(mode),frames_left(frames),frames_total(frames),ui(nullptr), open_gl(open_gl)
{
	if (frames_total <= 1) {
		Log("Error", "We are creating a progressbar with length 0 or less frames!");
		//Halt, because this is an odd bug
		return;
	}

	auto new_ui = std::make_unique<Group>(
		//TODO: Proper transform	
	);
	if (mode.is_combat_spell()) {
		new_ui->addElement(
			stun_background()
		);
		if (mode == Spell::Ice()) {
			makeBar("../Data/2D/Ui/TimeBarFrozen.png");
		} else {
			makeBar("../Data/2D/Ui/TimeBar.png");
		}
	} else if (mode.is_analysis_spell()) {
		new_ui->addElement(
			distraction_background()
		);
		makeBar("../Data/2D/Ui/TimeBar.png");
	} else {
		//TODO:
	}

	ui = std::move(new_ui);
}




void TimerBar::render(OpenGLContainer * open_gl) const
{
	if (!Done() && ui) {
		ui->Render(glm::mat4(),open_gl);
		bar->Render(glm::mat4());
	}
}


void TimerBar::Progress() {
	frames_left--;
	if (frames_left % 5 == 0) { //reduce frequency of updates as they wil be the same visually
		float progress = float(frames_left) / frames_total;
		bar->set_percent(progress);
	}
}

namespace detail {
	Transform2D BackgroundTransform() {
		return Transform2D(
			TransformPosition(0, -0.8f)
			, TransformScale(0.75f, 0.3f)
		);
	}
}

std::unique_ptr<PolymorphicUiElement> TimerBar::stun_background() const
{
	std::unique_ptr<PolymorphicUiElement> result = BasicUiElement::Make_BasicUiElement_UP(
		detail::BackgroundTransform()
		,"../Data/2D/Ui/StunBar.png"
	);
	return result;
}

std::unique_ptr<PolymorphicUiElement> TimerBar::distraction_background() const
{
	std::unique_ptr<PolymorphicUiElement> result = BasicUiElement::Make_BasicUiElement_UP(
		detail::BackgroundTransform()
		, "../Data/2D/Ui/DistractionBar.png"
	);
	return result;
}



void TimerBar::makeBar(const std::string& file)
{
	bar = BarUiElement::Make_BarUiElement_UP(
		Transform2DReduced(
			TransformPositionReduced(0, -0.818f)
			, TransformScaleReduced(0.50f, 0.127f)
		)
		,file
		, open_gl
		,1.0f
	);
}


void TimerBar::nullify() {
	//MAYBE: Explode or something
	frames_left = 0;
}