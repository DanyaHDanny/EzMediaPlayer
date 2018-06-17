#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QVideoWidget>

class Screen : public QVideoWidget
{
    Q_OBJECT

public:
    explicit Screen(QWidget *subclass = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // VIDEOWIDGET_H
