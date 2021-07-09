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

#include <Qt3DRender/QRenderCapture>
#include <Qt3DRender/QRenderCaptureReply>

#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DLogic/QFrameAction>

#include "ToolkitApp.h"
#include "SceneGroupsWidget.h"

#include "toolkit_config.h"

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


class TOOLKIT_DLLAPI SceneWidget : public QWidget {
	Q_OBJECT
	private:
		ToolkitApp* main_app;

		Qt3DExtras::Qt3DWindow *qt3d_view;
		Qt3DCore::QEntity *scene_root;
		Qt3DRender::QDirectionalLight* world_lighting;
		Qt3DLogic::QFrameAction *frame_action;

		Qt3DRender::QCamera *camera;
		QHBoxLayout *display_container;

		Qt3DRender::QRenderCapture *render_capture;

		SceneGroupsWidget* render_selector;
		std::vector<RenderGroupCheckBox*> render_select_check_boxes;

		int width, height;
		float fov;
		float near_v, far_v;

		QColor default_clear_color;

		GroupingMap scene_obj_grouping;

	public:
		SceneWidget(QWidget *parent=NULL);

		// adding an object will expect that all controll of memory is handed to this class meaning
		// that you don't have to care about freeing the memory again
		TOOLKIT_DLLAPI void addSceneObject(Qt3DCore::QEntity *scene_obj);
		TOOLKIT_DLLAPI void removeSceneObject(Qt3DCore::QEntity *scene_obj);
		TOOLKIT_DLLAPI void addSceneObjectToGroup(Qt3DCore::QEntity* scene_obj, QString group_name);
		TOOLKIT_DLLAPI void removeSceneObjectFromGroup(Qt3DCore::QEntity* scene_obj, QString group_name);

		TOOLKIT_DLLAPI Qt3DRender::QCamera* getCameraObj();
		TOOLKIT_DLLAPI void setCameraLens(Qt3DRender::QCameraLens::ProjectionType projection);

		TOOLKIT_DLLAPI Qt3DCore::QEntity* getSceneRoot() { return scene_root; }
		TOOLKIT_DLLAPI QColor getDefaultClearColor() { return default_clear_color; }

		TOOLKIT_DLLAPI int getWidth() { return width; }
		TOOLKIT_DLLAPI int getHeight() { return height; }

		TOOLKIT_DLLAPI Qt3DRender::QRenderCaptureReply* requestFrameCapture();

	protected:
		void resizeEvent(QResizeEvent *event);

	public slots:
		void toggle_render_obj_group(QString group_name, bool status);
		void frame_action_repeater(float dt);
		void update_orthographic_scale();
		void setOffscreenRender(QObject* surface);
		void setClearColor(QColor color);
		void setDefaultClearColor(QColor color);

	signals:
		void frame_sync_signal(float dt);
};


#endif 

