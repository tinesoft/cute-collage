#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

/**
  * @class ClickableLabel
  * @brief This widget represents a clickable label.
  *
  */
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0);
signals:
    void clicked();
protected:
    void mouseReleaseEvent(QMouseEvent *ev);


};

#endif // CLICKABLELABEL_H
