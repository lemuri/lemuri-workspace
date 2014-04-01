#ifndef QMLCOMPOSITOR_H
#define QMLCOMPOSITOR_H

#include <QtCompositor/QWaylandCompositor>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>

#include <QQuickItem>
#include <QQuickView>

class QmlCompositor : public QQuickView, public QWaylandCompositor
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)

public:
    QmlCompositor();

    QWaylandSurface *fullscreenSurface() const;

signals:
    void windowAdded(QVariant window);
    void windowDestroyed(QVariant window);
    void windowResized(QVariant window);
    void fullscreenSurfaceChanged();

public slots:
    void destroyWindow(QVariant window) {
        qvariant_cast<QObject *>(window)->deleteLater();
    }

    void destroyClientForWindow(QVariant window) {
        QWaylandSurface *surface = qobject_cast<QWaylandSurfaceItem *>(qvariant_cast<QObject *>(window))->surface();
        destroyClientForSurface(surface);
    }

    void setFullscreenSurface(QWaylandSurface *surface) {
        if (surface == m_fullscreenSurface)
            return;
        m_fullscreenSurface = surface;
        emit fullscreenSurfaceChanged();
    }

private slots:
    void surfaceMapped() {
        QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
        //Ignore surface if it's not a window surface
        if (!surface->hasShellSurface())
            return;

        QWaylandSurfaceItem *item = surface->surfaceItem();
        //Create a WaylandSurfaceItem if we have not yet
        if (!item)
            item = new QWaylandSurfaceItem(surface, rootObject());

        item->setTouchEventsEnabled(true);
        //item->takeFocus();
        emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
    }
    void surfaceUnmapped() {
        QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
        if (surface == m_fullscreenSurface)
            m_fullscreenSurface = 0;
        QQuickItem *item = surface->surfaceItem();
        emit windowDestroyed(QVariant::fromValue(item));
    }

    void surfaceDestroyed(QObject *object) {
        QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
        if (surface == m_fullscreenSurface)
            m_fullscreenSurface = 0;
        QQuickItem *item = surface->surfaceItem();
        emit windowDestroyed(QVariant::fromValue(item));
    }

    void sendCallbacks() {
        if (m_fullscreenSurface)
            sendFrameCallbacks(QList<QWaylandSurface *>() << m_fullscreenSurface);
        else
            sendFrameCallbacks(surfaces());
    }
    void startFrame() {
        frameStarted();
    }

protected:
    void resizeEvent(QResizeEvent *event);

    void surfaceCreated(QWaylandSurface *surface);

private:
    QWaylandSurface *m_fullscreenSurface;
};
#endif // QMLCOMPOSITOR_H
