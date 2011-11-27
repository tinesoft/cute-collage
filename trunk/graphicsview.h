#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

/**
  * @class GraphicsPixmapItem
  * @brief This class represents a photo in the collage.
  *
  * This class represents a photo in the collage. It inherits the class
  * QObject, so that we can animate it with the Qt Animation Framework.
  */
class GraphicsPixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
public:
    GraphicsPixmapItem(const QPixmap &pix)
        : QObject(), QGraphicsPixmapItem(pix)
    {
        setCacheMode(DeviceCoordinateCache);
        setFlag(ItemIsMovable);
    }
};

/**
  * @class GraphicsView
  * @brief This class represents the view in which we create the collage.
  */
class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent = 0);
    GraphicsView(QGraphicsScene *scene, QWidget * parent = 0) ;

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // GRAPHICSVIEW_H

