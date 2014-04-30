#ifndef LEMURICOMPOSITOR_H
#define LEMURICOMPOSITOR_H

#include <QVariant>
#include <QtCompositor/QWaylandCompositor>

class ViewOutput;
class LemuriCompositor : public QObject, public QWaylandCompositor
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)
public:
    LemuriCompositor(const char *socketName = 0, ExtensionFlags extensions = DefaultExtensions);

    void initOuputs();

    QWaylandSurface *fullscreenSurface() const;

    virtual void setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY);

signals:
    void windowAdded(QVariant window);
    void windowDestroyed(QVariant window);
    void windowResized(QVariant window);
    void fullscreenSurfaceChanged();
    void selectNextWindow();
    void selectPreviousWindow();

public slots:
    void destroyWindow(QVariant window);
    void destroyClientForWindow(QVariant window);
    void setFullscreenSurface(QWaylandSurface *surface);

private slots:
    void surfaceMapped();
    void surfaceUnmapped();
    void surfaceDestroyed(QObject *object);

protected:
    void surfaceCreated(QWaylandSurface *surface);
    void sendCallbacks();

private slots:
    void updateCursor();

private:
    friend class ViewOutput;
    ViewOutput *m_viewOutput;
    QWaylandSurface *m_fullscreenSurface = 0;

    // Cursor
    QWaylandSurface *m_cursorSurface = 0;
    int m_cursorHotspotX = 0;
    int m_cursorHotspotY = 0;
};

#endif // LEMURICOMPOSITOR_H
