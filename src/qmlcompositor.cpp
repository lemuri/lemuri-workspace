#include "qmlcompositor.h"

#include <QQmlContext>

QmlCompositor::QmlCompositor()
    : QWaylandCompositor(this, 0, DefaultExtensions | SubSurfaceExtension)
    , m_fullscreenSurface(0)
{
    rootContext()->setContextProperty("compositor", this);
    setSource(QUrl(QLatin1String("qrc:/qml/main.qml")));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::black);
    winId();

    connect(this, SIGNAL(beforeSynchronizing()), this, SLOT(startFrame()), Qt::DirectConnection);
    connect(this, SIGNAL(afterRendering()), this, SLOT(sendCallbacks()));
}

QWaylandSurface *QmlCompositor::fullscreenSurface() const
{
    return m_fullscreenSurface;
}

void QmlCompositor::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void QmlCompositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, SIGNAL(destroyed(QObject *)), this, SLOT(surfaceDestroyed(QObject *)));
    connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
    connect(surface,SIGNAL(unmapped()), this,SLOT(surfaceUnmapped()));
}
