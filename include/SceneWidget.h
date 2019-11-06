#ifndef SceneWidget_h_INCLUDED
#define SceneWidget_h_INCLUDED

#include <QResizeEvent>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DLogic/QFrameAction>

#include "ToolkitApp.h"
class ToolkitApp;
/*
 * A small wrapper class to render with the Qt3D Engine to be used as a QWidget
 */

typedef std::map<QString, std::vector<Qt3DCore::QEntity*>> GroupingMap;

class RenderGroupCheckBox : public QCheckBox {
	Q_OBJECT
	public:
		RenderGroupCheckBox(QString obj_group_name):QCheckBox(obj_group_name) {
			connect(this, SIGNAL(stateChanged(int)), this, SLOT(changeIntercept(int)));
		}
	public slots:
		void changeIntercept(int state) {
			emit renderGroupStateChanged(this->text(), (bool)state);
		}
	signals:
		void stateChanged(bool);
		void renderGroupStateChanged(QString name, bool state);

};

class SceneWidget : public QWidget {
	Q_OBJECT
	private:
		ToolkitApp* main_app;

		Qt3DExtras::Qt3DWindow *qt3d_view;
		Qt3DCore::QEntity *scene_root;
		Qt3DRender::QDirectionalLight* world_lighting;
		Qt3DLogic::QFrameAction *frame_action;

		Qt3DRender::QCamera *camera;
		QHBoxLayout *display_container;

		QWidget* render_selector;
		QVBoxLayout* render_selector_layout;
		std::vector<RenderGroupCheckBox*> render_select_check_boxes;

		int width, height;
		float fov;
		float near, far;

		GroupingMap scene_obj_grouping;

	public:
		SceneWidget(QWidget *parent=NULL);

		// adding an object will expect that all controll of memory is handed to this class meaning
		// that you don't have to care about freeing the memory again
		void addSceneObject(Qt3DCore::QEntity *scene_obj);
		void removeSceneObject(Qt3DCore::QEntity *scene_obj);

		Qt3DRender::QCamera* getCameraObj();

	protected:
		void resizeEvent(QResizeEvent *event);

	public slots:
		void toggle_render_obj_group(QString group_name, bool status);
		void frame_action_repeater(float dt);

	signals:
		void frame_sync_signal(float dt);
};


#endif 

