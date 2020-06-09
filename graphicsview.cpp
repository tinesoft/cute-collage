#include "graphicsview.h"


GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent)
{
}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget * parent) : QGraphicsView(scene,parent)
{
}


void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}
