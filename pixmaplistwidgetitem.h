#ifndef PIXMAPLISTWIDGETITEM_H
#define PIXMAPLISTWIDGETITEM_H

#include <QPixmap>
#include <QListWidgetItem>

/**
  *@class PixmapWrapper
  *@brief A wrapper class that contains both a pixmap and his filename.
  */

class PixmapWrapper
{

public:
    PixmapWrapper(const QString & fileName);
    PixmapWrapper(const PixmapWrapper & pw);
    PixmapWrapper(){}
    ~PixmapWrapper(){}

    QString fileName()  const   {return _fileName;}
    QPixmap pixmap()   const   {return _pixmap;}

    void setFileName(const QString &fileName);
    void setPixmap(const QPixmap &image);
private:
    QString _fileName;
    QPixmap _pixmap;

};

/**
  *@class PixmapListWidgetItem
  *@brief This class adds a @ref PixmapWrapper to the QListWidgetItem.
  */
class PixmapListWidgetItem : public QListWidgetItem
{

public:
    PixmapListWidgetItem(const PixmapWrapper & pw, QListWidget *parent = 0);
    ~PixmapListWidgetItem(){}

    QString fileName()  const   {return _pw.fileName();}
    QPixmap pixmap()   const   {return _pw.pixmap();}

    void setFileName(const QString &fileName);
    void setPixmap(const QPixmap &pixmap);

private:
    PixmapWrapper _pw;

};


#endif // PIXMAPLISTWIDGETITEM_H
