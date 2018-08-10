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
	
protected:
	void _first_init();
	void _empty_init() {}
	
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
