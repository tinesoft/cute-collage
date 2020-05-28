#include "imageutils.h"

#include <QPixmap>
#include <QFileInfo>
#include <QDateTime>
#include <QStringList>

ImageUtils::ImageUtils()
{
}


QString ImageUtils::supportedImageFormatsFilter()
{
    return

        QObject::tr("Portable Network Graphics") + " ( *.png );;" +
        QObject::tr("JPEG Images") + " ( *.jpeg *.jpg );;" +
        QObject::tr("Windows Bitmaps") + " ( *.bmp );;" +
        QObject::tr("TIFF Images") + " ( *.tiff *.tif );;" +
        QObject::tr("X11 Bitmaps") + " ( *.xbm );;" +
        QObject::tr("X11 Pixmaps") + " ( *.xpm );;" +
        QObject::tr("Portable Pixmaps") + " ( *.ppm );;" +
        QObject::tr("Portable Bitmaps") + " ( *.pbm );;" +
        QObject::tr("Portable Graymaps") + " ( *.pgm );;"+
        QObject::tr("All Images") + " ( *.bmp *.jpeg *.jpg *.png *.tiff *.tif *.xbm *.xpm *.ppm *.pbm *.pgm );;";

}

QStringList ImageUtils::supportedImageFormats()
{
    return
        QStringList() <<"bmp"<<"jpeg"<<"jpg"<<"png"<<"tiff"<<"tif"<<"xbm"<<"xpm"<<"ppm"<<"pbm"<<"pgm" ;
}

QString ImageUtils::imageInfo(const QPixmap & image, const QString &fileName)
{
    QFileInfo fileInfo(fileName);

    qreal fileSize = fileInfo.size();
    QString sizeString;

    if (fileSize < 1024)
        sizeString = "<b>Size : </b>%1 Bytes";
    else if (fileSize < 1024*1024)
    {
        fileSize /= 1024;
        sizeString = "<b>Size : </b>%1 KBytes";
    }
    else
    {
        fileSize /= 1024*1024;
        sizeString = "<b>Size : </b>%1 MBytes";
    }

    return
           "<img src=\":/images/info.png\" width=\"32\" height=\"32\"/>"+
            QObject::tr("<b>Informations on the photo :<br/>") +
            "______________________________________________________________________</b><br/>"+
            QObject::tr("<b>File : </b>%1").arg(fileInfo.fileName()) + "<br/>" +
            QObject::tr(sizeString.toStdString().c_str()).arg(fileSize) + "<br/>" +
            QObject::tr("<b>Folder : </b>%1").arg(fileInfo.absolutePath()) + "<br/>" +
            QObject::tr("<b>Created on : </b>%1").arg(fileInfo.created().toString(Qt::DefaultLocaleLongDate)) + "<br/>" +
            QObject::tr("<b>Dimensions : </b>%1 x %2").arg(image.width()).arg(image.height())+ "<br/>" +
            QObject::tr("<b>Depth : </b>%1").arg(image.depth());


}
