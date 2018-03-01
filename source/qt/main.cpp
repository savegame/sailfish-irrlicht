#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QQuickView>
#include <QGuiApplication>
#include "IrrQuick.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc,argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

	qmlRegisterType<GLView>("harbour.irrquick.ga", 1, 0, "GLView");

	QUrl path = SailfishApp::pathTo("qml/irrlicht.qml");
	view->setSource( path );
    view->show();
    return app->exec();
}
