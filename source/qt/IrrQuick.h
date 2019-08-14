#ifndef IRR_QUICK_INCLUDED
#define IRR_QUICK_INCLUDED

#include <QQuickItem>
#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <IrrlichtDevice.h>
#include "CQGLFunctionsDriver.h"

using namespace irr;
using namespace video;

class GLRenderer;
class IrrQuickItem : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
public:
	IrrQuickItem(QQuickItem *parent = nullptr);

	qreal t() const { return m_t; }
	void setT(qreal t);

	Q_INVOKABLE void loadExample(int index);
signals:
	void tChanged();

public slots:
	void sync();
	void cleanup();
private slots:
	void handleWindowChanged(QQuickWindow *window);
protected:
	virtual void touchEvent(QTouchEvent *e) override;
	virtual void mousePressEvent(QMouseEvent *e) override;
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void keyReleaseEvent(QKeyEvent *e) override;

	void sendKeyEventToIrrlicht( QKeyEvent* event, bool pressedDown );

private:
	qreal        m_t;
	QPointF      m_pressPos;
	GLRenderer  *m_renderer;
};

class GLRenderer : public QObject, protected QOpenGLFunctions
{
	Q_OBJECT

	friend class IrrQuickItem;
public:
	explicit GLRenderer(QObject *parent = 0);

	~GLRenderer();

	void setT(qreal t) {m_t = t;}
	void setViewportSize(const QSize &size);
	void setWindow(QQuickWindow *window) { m_window = window; }

	typedef void (GLRenderer::*initFunc)();
	initFunc init;


	void createCube();
	void setCamera();

	Q_INVOKABLE void loadExample(int index);

protected:
	void _first_init();
	void _empty_init() {}

	bool clear_scene();
	const io::path getExampleMediaPath();

	void _load_example_1();
	void _load_example_2();
	void _load_example_11();
signals:

public slots:
	void paint();

private:
	qreal                   m_angularSpeed;
	qreal                   m_t;
	QSize                   m_viewportSize;
	QQuickWindow           *m_window;
	irr::IrrlichtDevice    *m_device;
	CQGLFunctionsDriver    *m_driver;
};

#endif
