#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget* parent);
private:
    void mousePressEvent(QMouseEvent* event);
signals:
    void mousePress(QPoint);
};

#endif // GRAPHICSVIEW_H
