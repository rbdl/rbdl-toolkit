#ifndef TIMELINE_H_INCLUDED
#define TIMELINE_H_INCLUDED

#include <QWidget>
#include "ui_Timeline.h"
#include "toolkit_config.h"

class TOOLKIT_DLLAPI ToolkitTimeline : public QWidget, public Ui::Timeline {
	Q_OBJECT
	private:
		float speed_factor;
		float max_time;
		float current_time;
		unsigned int slider_granularity;
		bool playing;
		bool was_playing;

	public:
		ToolkitTimeline(QWidget* parent);

		void setMaxTime(float max_time);
		float getMaxTime() { return max_time; }
		float getCurrentTime() { return current_time; }
		void setCurrentTime(float current_time, bool stop_playing=false);
		void reset();

	public slots:
		void speedChanged(double speed);
		void togglePlaying();

		void sliderMoved(int pos);
		void sliderPressed();
		void sliderReleased();

		void tick(float dt);

		void reset_timeline();

	signals:
		//signal always sends absolute time so that all listeners can update accordingly
		void timeChanged(float current_time);
		void maxTimeChanged(float max_time);
};

#endif 

