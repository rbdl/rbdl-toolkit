#ifndef SceneWidget_h_INCLUDED
#define SceneWidget_h_INCLUDED

#include <QWidget>
#include <QHBoxLayout>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

/*
 * A small wrapper class to render with the Qt3D Engine to be used as a QWidget
 */


class SceneWidget : public QWidget {
	Q_OBJECT
	private:
		Qt3DExtras::Qt3DWindow *qt3d_view;

		Qt3DCore::QEntity *scene_root;

		Qt3DRender::QCamera *camera;
		QHBoxLayout *display_container;
	public:
		SceneWidget(QWidget *parent=NULL);
};

#endif 

