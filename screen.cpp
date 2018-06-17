#include "screen.h"

Screen::Screen(QWidget *subclass) : QVideoWidget(subclass)
{
    QPalette qpalette = palette();
    qpalette.setColor(QPalette::Window, Qt::white);
    this->setPalette(qpalette);
    this->setAttribute(Qt::WA_OpaquePaintEvent);
}

void Screen::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen())
    {
        setFullScreen(false);
        event->accept();
    }
}

void Screen::mouseDoubleClickEvent(QMouseEvent *event)
{
    setFullScreen(!isFullScreen());
    event->accept();
}
