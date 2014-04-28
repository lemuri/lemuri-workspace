#include "qmlcompositor.h"

#include "surfaceitem.h"

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

void QmlCompositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    //Ignore surface if it's not a window surface
    if (!surface->hasShellSurface())
        return;

    qDebug() << Q_FUNC_INFO << surface->title() << surface->windowType() << surface->windowFlags();
    if (surface->windowType() == QWaylandSurface::Popup) {
        qDebug() << surface->title() << "POPUP";
    }
    QWaylandSurfaceItem *item = new SurfaceItem(surface, rootObject());
    surface->setSurfaceItem(item);

    item->setTouchEventsEnabled(true);
//    item->takeFocus();
    emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void QmlCompositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = 0;
    QQuickItem *item = surface->surfaceItem();
    qDebug() << Q_FUNC_INFO << item;
    emit windowDestroyed(QVariant::fromValue(item));
}

void QmlCompositor::surfaceDestroyed(QObject *object)
{
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = 0;
    QQuickItem *item = surface->surfaceItem();
    if (item) {
        emit windowDestroyed(QVariant::fromValue(item));
    }
}

void QmlCompositor::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void QmlCompositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, &QWaylandSurface::destroyed,
            this, &QmlCompositor::surfaceDestroyed);
    connect(surface, &QWaylandSurface::mapped,
            this, &QmlCompositor::surfaceMapped);
    connect(surface, &QWaylandSurface::unmapped,
            this, &QmlCompositor::surfaceUnmapped);
}
