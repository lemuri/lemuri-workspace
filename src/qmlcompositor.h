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

    virtual void setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY);

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
    void surfaceMapped();
    void surfaceUnmapped();
    void surfaceDestroyed(QObject *object);

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

private slots:
    void updateCursor();

private:
    QWaylandSurface *m_fullscreenSurface;

    // Cursor
    QWaylandSurface *m_cursorSurface = 0;
    int m_cursorHotspotX = 0;
    int m_cursorHotspotY = 0;
};
#endif // QMLCOMPOSITOR_H
