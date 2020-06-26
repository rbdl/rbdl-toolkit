#include "Timeline.h"

#include <math.h>

ToolkitTimeline::ToolkitTimeline(QWidget *parent) : speed_factor(1.0), 
                                                    max_time(0.0), 
                                                    current_time(0.0), 
                                                    playing(false), 
                                                    was_playing(false), 
                                                    slider_granularity(10000),
                                                    QWidget(parent) 
{
	//setup ui created by designer
	setupUi(this);

	//display correct values
	TimelineScaleSpinBox->setValue(speed_factor);
	TimelineVertSlider->setMaximum(slider_granularity);

	//set play/pause shortcut
	TimelinePlayPauseButton->setShortcut(QKeySequence::fromString(" "));

	//setup interactions
	connect(TimelineScaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(speedChanged(double)));
	connect(TimelinePlayPauseButton, SIGNAL(clicked()), this, SLOT(togglePlaying()));
	connect(TimelineVertSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
	connect(TimelineVertSlider, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
	connect(TimelineVertSlider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
}

void ToolkitTimeline::reset() {
	max_time = 0.;
	current_time = 0.;
	playing = false;
	speed_factor = 1.0;
}

void ToolkitTimeline::setMaxTime(float max) {
	float old_max = max_time;
	max_time = std::max(max, max_time);

	if (old_max != max_time)
		emit maxTimeChanged(max_time);
}

void ToolkitTimeline::togglePlaying() {
	if (max_time > 0.) {
		playing = !playing;
		was_playing = playing;
	}
}

void ToolkitTimeline::sliderPressed() {
	playing = false;
}

void ToolkitTimeline::sliderReleased() {
	playing = was_playing;
}

void ToolkitTimeline::sliderMoved(int pos) {
	float p = (float)pos / (float)slider_granularity;
	setCurrentTime( max_time * p);
}

void ToolkitTimeline::setCurrentTime(float current, bool stop_playing) {
	if (stop_playing) {
		playing = false;
	}
	current_time = current;

	//reset to start when reached max time
	if (current_time > max_time) {
		current_time = 0.0;
	}

	TimelineTimeLabel->setText(QString("%1").arg(current_time, 0, 'f', 3));
	float p = current_time / max_time;
	TimelineVertSlider->setValue(slider_granularity * p);

	emit timeChanged(current_time);
}

void ToolkitTimeline::tick(float dt) {
	if (max_time <= 0. || !playing ) {
		//do not do anything if max time is smaller or equal to zero
		//or if animation is halted
		return;
	}

	current_time += dt * speed_factor;
	setCurrentTime(current_time);
}

void ToolkitTimeline::speedChanged(double speed) {
	speed_factor = speed;
}

void ToolkitTimeline::reset_timeline() {
	max_time = 0.;
	setCurrentTime(0.);
	playing = false;

	emit timeChanged(current_time);
}
