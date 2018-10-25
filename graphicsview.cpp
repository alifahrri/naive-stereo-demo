#include "graphicsview.h"
#include <QMouseEvent>
#include <QDebug>

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{

}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF scene_point = mapToScene(event->pos());
//    qDebug() << "mouse press :" << event->pos()
//             << "map to scene :" << mapToScene(event->pos());
    if(scene_point.x()>0 && scene_point.y()>0)
        emit mousePress(QPoint(scene_point.x(),scene_point.y()));
}
