#ifndef VIEWOUTPUT_H
#define VIEWOUTPUT_H

#include <QQuickView>

class LemuriCompositor;
class ViewOutput : public QQuickView
{
    Q_OBJECT
public:
    explicit ViewOutput(LemuriCompositor *compositor);

    void init();

    void printInfo();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);
    void touchEvent(QTouchEvent *event);

    void resizeEvent(QResizeEvent *event);

private:
    void startFrame();

    LemuriCompositor *m_compositor;
    int m_idleInhibit = 0;
};

#endif // VIEWOUTPUT_H
