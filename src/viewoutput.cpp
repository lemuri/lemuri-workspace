#include "viewoutput.h"

#include "lemuricompositor.h"

#include <QQmlContext>

// ---
#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QWindow>
#include <QtCore/QDebug>

#include <qpa/qplatformnativeinterface.h>

//#include "gldebug.h"
//#include "logging.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(LEMURI_COMPOSITOR, "lemuri.compositor")

#include <EGL/egl.h>
#include <EGL/eglext.h>
// ---

ViewOutput::ViewOutput(LemuriCompositor *compositor)
    : m_compositor(compositor)
{
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::black);

    connect(this, &ViewOutput::beforeSynchronizing, this, &ViewOutput::startFrame, Qt::DirectConnection);
    connect(this, &ViewOutput::afterRendering, m_compositor, &LemuriCompositor::sendCallbacks);
}

void ViewOutput::init()
{
    rootContext()->setContextProperty("compositor", m_compositor);
    setSource(QUrl(QLatin1String("qrc:/qml/main.qml")));
    printInfo();
}

void ViewOutput::printInfo()
{
    const char *str;

    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    if (nativeInterface) {
        EGLDisplay display = nativeInterface->nativeResourceForWindow("EglDisplay", this);
        if (display) {
            str = eglQueryString(display, EGL_VERSION);
            qCDebug(LEMURI_COMPOSITOR) << "EGL version:" << str;

            str = eglQueryString(display, EGL_VENDOR);
            qCDebug(LEMURI_COMPOSITOR) << "EGL vendor:" << str;

            str = eglQueryString(display, EGL_CLIENT_APIS);
            qCDebug(LEMURI_COMPOSITOR) << "EGL client APIs:" << str;

            str = eglQueryString(display, EGL_EXTENSIONS);
            QStringList extensions = QString(str).split(QLatin1Char(' '));
            qCDebug(LEMURI_COMPOSITOR) << "EGL extensions:"
                                            << qPrintable(extensions.join(QStringLiteral("\n\t")));
        }
    }

    str = (char *)glGetString(GL_VERSION);
    qCDebug(LEMURI_COMPOSITOR) << "GL version:" << str;

    str = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    qCDebug(LEMURI_COMPOSITOR) << "GLSL version:" << str;

    str = (char *)glGetString(GL_VENDOR);
    qCDebug(LEMURI_COMPOSITOR) << "GL vendor:" << str;

    str = (char *)glGetString(GL_RENDERER);
    qCDebug(LEMURI_COMPOSITOR) << "GL renderer:" << str;

    str = (char *)glGetString(GL_EXTENSIONS);
    QStringList extensions = QString(str).split(QLatin1Char(' '));
    qCDebug(LEMURI_COMPOSITOR) << "GL extensions:"
                                    << qPrintable(extensions.join(QStringLiteral("\n\t")));
}

void ViewOutput::keyPressEvent(QKeyEvent *event)
{
    m_idleInhibit++;

    qCDebug(LEMURI_COMPOSITOR) << event->modifiers() << (Qt::MetaModifier | Qt::ShiftModifier) << Qt::MetaModifier << (event->modifiers() == (Qt::MetaModifier | Qt::ShiftModifier));
    qCDebug(LEMURI_COMPOSITOR) << (event->modifiers() & Qt::MetaModifier) << (event->modifiers() & Qt::ShiftModifier) << event->key();
    if (event->modifiers() == Qt::MetaModifier && event->key() == Qt::Key_Tab) {
        qCDebug(LEMURI_COMPOSITOR) << "selectNextWindow";
        emit m_compositor->selectNextWindow();
    } else if (event->modifiers() == (Qt::MetaModifier | Qt::ShiftModifier) && event->key() == Qt::Key_Backtab) {
        qCDebug(LEMURI_COMPOSITOR) << "selectPreviousWindow";
        emit m_compositor->selectPreviousWindow();
    }
    QQuickView::keyPressEvent(event);
}

void ViewOutput::keyReleaseEvent(QKeyEvent *event)
{
    m_idleInhibit--;
    QQuickView::keyReleaseEvent(event);
}

void ViewOutput::mousePressEvent(QMouseEvent *event)
{
    m_idleInhibit++;
    QQuickView::mousePressEvent(event);
}

void ViewOutput::mouseReleaseEvent(QMouseEvent *event)
{
    m_idleInhibit--;
    QQuickView::mouseReleaseEvent(event);
}

void ViewOutput::mouseMoveEvent(QMouseEvent *event)
{
    QQuickView::mouseMoveEvent(event);
}

void ViewOutput::wheelEvent(QWheelEvent *event)
{
    QQuickView::wheelEvent(event);
}

void ViewOutput::touchEvent(QTouchEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        m_idleInhibit++;
    } else if (event->type() == QEvent::TouchEnd) {
        m_idleInhibit--;
    }
    QQuickView::touchEvent(event);
}

void ViewOutput::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    m_compositor->setOutputGeometry(QRect(0, 0, width(), height()));
}

void ViewOutput::startFrame()
{
    m_compositor->frameStarted();
}
