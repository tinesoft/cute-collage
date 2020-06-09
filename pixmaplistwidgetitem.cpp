#include "pixmaplistwidgetitem.h"
#include "imageutils.h"

#include <QFileInfo>

PixmapWrapper::PixmapWrapper(const QString & fileName)
    :_fileName(fileName),_pixmap(QPixmap(fileName))
{}

PixmapWrapper::PixmapWrapper(const PixmapWrapper & pw)
    :_fileName(pw.fileName()),_pixmap(pw.pixmap())
{}


void PixmapWrapper::setFileName(const QString &fileName)
{
    _fileName=fileName;
}

void PixmapWrapper::setPixmap(const QPixmap &pixmap)
{
    _pixmap=pixmap;
}

PixmapListWidgetItem::PixmapListWidgetItem(const PixmapWrapper &pw, QListWidget *parent)
    : QListWidgetItem(parent),_pw(pw)
{

    this->_pw = pw;
    this->setIcon(QIcon(pw.pixmap()));
    this->setText(QFileInfo(pw.fileName()).baseName());
    this->setToolTip(ImageUtils::imageInfo(pw.pixmap(),pw.fileName()));
}

void PixmapListWidgetItem::setFileName(const QString &fileName)
{
    _pw.setFileName(fileName);
}

void PixmapListWidgetItem::setPixmap(const QPixmap &image)
{
    _pw.setPixmap(image);
}

