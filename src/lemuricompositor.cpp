#include "lemuricompositor.h"
#include "viewoutput.h"
#include "surfaceitem.h"

#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QGuiApplication>

LemuriCompositor::LemuriCompositor(const char *socketName, ExtensionFlags extensions)
    : QWaylandCompositor(new ViewOutput(this), socketName, extensions)
{
    m_viewOutput = qobject_cast<ViewOutput*>(window());
}

void LemuriCompositor::initOuputs()
{
    // TODO honor the plural of this method
    m_viewOutput->init();
    m_viewOutput->setTitle(QLatin1String("Lemuri Workspace"));
    m_viewOutput->setGeometry(0, 0, 1024, 768);
    m_viewOutput->show();
    m_viewOutput->printInfo();
}

QWaylandSurface *LemuriCompositor::fullscreenSurface() const
{
    return m_fullscreenSurface;
}

void LemuriCompositor::setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY)
{
    if ((m_cursorSurface != surface) && surface) {
        connect(surface, &QWaylandSurface::damaged, this, &LemuriCompositor::updateCursor);
    }

    m_cursorSurface = surface;
    m_cursorHotspotX = hotspotX;
    m_cursorHotspotY = hotspotY;
}

void LemuriCompositor::destroyWindow(QVariant window)
{
    qvariant_cast<QObject *>(window)->deleteLater();
}

void LemuriCompositor::destroyClientForWindow(QVariant window)
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurfaceItem *>(qvariant_cast<QObject *>(window))->surface();
    destroyClientForSurface(surface);
}

void LemuriCompositor::setFullscreenSurface(QWaylandSurface *surface)
{
    if (surface == m_fullscreenSurface)
        return;
    m_fullscreenSurface = surface;
    emit fullscreenSurfaceChanged();
}

void LemuriCompositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    //Ignore surface if it's not a window surface
    if (!surface->hasShellSurface())
        return;

    qDebug() << Q_FUNC_INFO << surface->title() << surface->windowType() << surface->windowFlags();
    qDebug() << Q_FUNC_INFO << surface->size() << surface->pos() << surface->windowProperties();
    if (surface->windowType() == QWaylandSurface::Popup) {
        qDebug() << surface->title() << "POPUP";
    }
    QWaylandSurfaceItem *item = new SurfaceItem(surface, m_viewOutput->rootObject());
    surface->setSurfaceItem(item);

    item->setTouchEventsEnabled(true);
//    item->takeFocus();
    emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void LemuriCompositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == m_fullscreenSurface) {
        m_fullscreenSurface = 0;
    }

    QQuickItem *item = surface->surfaceItem();
    qDebug() << Q_FUNC_INFO << item;
    emit windowDestroyed(QVariant::fromValue(item));
}

void LemuriCompositor::surfaceDestroyed(QObject *object)
{
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = 0;
    QQuickItem *item = surface->surfaceItem();
    if (item) {
        emit windowDestroyed(QVariant::fromValue(item));
    }
}

void LemuriCompositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, &QWaylandSurface::destroyed,
            this, &LemuriCompositor::surfaceDestroyed);
    connect(surface, &QWaylandSurface::mapped,
            this, &LemuriCompositor::surfaceMapped);
    connect(surface, &QWaylandSurface::unmapped,
            this, &LemuriCompositor::surfaceUnmapped);
}

void LemuriCompositor::sendCallbacks()
{
    if (m_fullscreenSurface)
        sendFrameCallbacks(QList<QWaylandSurface *>() << m_fullscreenSurface);
    else
        sendFrameCallbacks(surfaces());
}

void LemuriCompositor::updateCursor()
{
    if (!m_cursorSurface) {
        return;
    }

    QCursor cursor(QPixmap::fromImage(m_cursorSurface->image()), m_cursorHotspotX, m_cursorHotspotY);
    static bool cursorIsSet = false;
    if (cursorIsSet) {
        QGuiApplication::changeOverrideCursor(cursor);
    } else {
        QGuiApplication::setOverrideCursor(cursor);
        cursorIsSet = true;
    }
}
