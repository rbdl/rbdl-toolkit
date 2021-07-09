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

		TOOLKIT_DLLAPI void setMaxTime(float max_time);
		TOOLKIT_DLLAPI float getMaxTime() { return max_time; }
		TOOLKIT_DLLAPI float getCurrentTime() { return current_time; }
		TOOLKIT_DLLAPI void setCurrentTime(float current_time, bool stop_playing=false);
		TOOLKIT_DLLAPI void reset();

	public slots:
		TOOLKIT_DLLAPI void speedChanged(double speed);
		TOOLKIT_DLLAPI void togglePlaying();

		TOOLKIT_DLLAPI void sliderMoved(int pos);
		TOOLKIT_DLLAPI void sliderPressed();
		TOOLKIT_DLLAPI void sliderReleased();

		TOOLKIT_DLLAPI void tick(float dt);

		TOOLKIT_DLLAPI void reset_timeline();

	signals:
		//signal always sends absolute time so that all listeners can update accordingly
		void timeChanged(float current_time);
		void maxTimeChanged(float max_time);
};

#endif 

