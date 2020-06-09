#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>

#include <QDropEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>


/**
  * @class ImageLabel
  * @brief This widget represents a label that supports drag & drop images.
  *
  */
class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);
signals:
    void clicked();
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent * event);
    void mouseReleaseEvent(QMouseEvent *ev);


};

#endif // IMAGELABEL_H
