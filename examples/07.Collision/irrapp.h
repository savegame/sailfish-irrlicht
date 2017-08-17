#ifndef IRRAPP_H
#define IRRAPP_H

#include <QObject>
#include <QTimer>
#include <irrlicht.h>

using namespace irr;


enum
{
	// I use this ISceneNode ID to indicate a scene node that is
	// not pickable by getSceneNodeAndCollisionPointFromRay()
	ID_IsNotPickable = 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be picked by ray selection.
	IDFlag_IsPickable = 1 << 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be highlighted.  In this example, the
	// homonids can be highlighted, but the level mesh can't.
	IDFlag_IsHighlightable = 1 << 1
};

class IrrApp : public QObject
{
	Q_OBJECT
public:
	explicit IrrApp(QObject *parent = 0);

	~IrrApp();
signals:

public slots:
public Q_SLOTS:
	void onTimer();

protected:
	IrrlichtDevice *device;
	video::IVideoDriver* driver;
	scene::ISceneManager* smgr;
	QTimer *timer;
	scene::ISceneNode* highlightedSceneNode;
	scene::ISceneCollisionManager* collMan;
	scene::ICameraSceneNode* camera;
	video::SMaterial material;
	scene::IBillboardSceneNode * bill;
};

#endif // IRRAPP_H
