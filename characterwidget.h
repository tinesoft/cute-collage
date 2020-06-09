
#ifndef CHARACTERWIDGET_H
#define CHARACTERWIDGET_H

#include <QFont>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPaintEvent;
QT_END_NAMESPACE


/**
  * @class CharacterWidget
  * @brief This widget represents a map of all characters.
  *
  */
class CharacterWidget : public QWidget
{
    Q_OBJECT

public:
    CharacterWidget(QWidget *parent = 0);
    QSize sizeHint() const;

    QString selectedCharacter() const { return QString(QChar(_lastKey));}
public slots:
    void updateFont(const QFont &font);
    void updateSize(const QString &fontSize);
    void updateStyle(const QString &fontStyle);
    void updateFontMerging(bool enable);

signals:
    void characterSelected(const QString &character);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QFont   _displayFont;
    int     _columns;
    int     _lastKey;
    int     _squareSize;
};


#endif
