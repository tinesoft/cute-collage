#include "imagelabel.h"
#include "imageutils.h"

#include <QUrl>
#include <QFileInfo>

ImageLabel::ImageLabel(QWidget *parent) :
    QLabel(parent)
{
}


void ImageLabel::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->accept();
    }
}

void ImageLabel::dropEvent(QDropEvent * event)
{
    QString imageName;
    //we only get the first image in the list
    imageName = event->mimeData()->urls().at(0).toLocalFile().remove("file:///");

    //if the files in the list are supported images, we add them to images list
    QString extension = QFileInfo(imageName).suffix().toLower();
    if(ImageUtils::supportedImageFormats().contains(extension))
    {
        setPixmap(QPixmap(imageName));
    }

}

void ImageLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit clicked();
}
