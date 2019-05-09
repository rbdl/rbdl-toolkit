#ifndef SceneWidget_h_INCLUDED
#define SceneWidget_h_INCLUDED

#include <QResizeEvent>
#include <QWidget>
#include <QHBoxLayout>

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/Qt3DWindow>
/*
 * A small wrapper class to render with the Qt3D Engine to be used as a QWidget
 */

typedef std::map<QString, std::vector<Qt3DCore::QEntity*>> GroupingMap;

class SceneWidget : public QWidget {
	Q_OBJECT
	private:
		Qt3DExtras::Qt3DWindow *qt3d_view;

		Qt3DCore::QEntity *scene_root;
		Qt3DRender::QDirectionalLight* world_lighting;

		Qt3DRender::QCamera *camera;
		QHBoxLayout *display_container;

		int width, height;
		float fov;
		float near, far;

		GroupingMap scene_obj_grouping;

	public:
		SceneWidget(QWidget *parent=NULL);

		// adding an object will expect that all controll of memory is handed to this class meaning
		// that you don't have to care about freeing the memory again
		void addSceneObject(Qt3DCore::QEntity *scene_obj);

	protected:
		void resizeEvent(QResizeEvent *event);
};

Qt3DCore::QEntity* createGridFloor(float lxy_border, float r_xyborder, int count, QColor line_color=QColor(QRgb(0xffffff)));
Qt3DCore::QEntity* loadMeshFromFile(const char* filename);

#endif 

