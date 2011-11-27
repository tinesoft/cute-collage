#include "drawingarea.h"
#include <QtGui>

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);//for optimizing painting events
    _modified = false;
    _isDrawing = false;
    _penWidth = 10;
    _penColor = Qt::black;
}

bool DrawingArea::loadImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    //we resize it to fit the drawing area zone
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    _image = loadedImage;
    _modified = false;
    update();
    return true;
}

bool DrawingArea::loadImageToFormat(const QString &fileName,QImage::Format format, Qt::ImageConversionFlags flags)
 {
     QImage loadedImage;
     if (!loadedImage.load(fileName))
         return false;

     //we convert the image to the given format
     loadedImage = loadedImage.convertToFormat(format,flags);

     //we resize it to fit the drawing area zone
     QSize newSize = loadedImage.size().expandedTo(size());
     resizeImage(&loadedImage, newSize);
     _image = loadedImage;
     _modified = false;
     update();
     return true;
 }

bool DrawingArea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = _image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        _modified = false;
        return true;
    } else {
        return false;
    }
}

void DrawingArea::setPenColor(const QColor &newColor)
{
    _penColor = newColor;
}

void DrawingArea::setPenWidth(int newWidth)
{
    _penWidth = newWidth;
}

void DrawingArea::resetImage()
{
    _image.fill(qRgb(255, 255, 255));
    _modified = true;
    update();
}

void DrawingArea::setImage(const QImage &image)
{
    QImage resizedImage = image;
    QSize newSize = resizedImage.size().expandedTo(size());
    resizeImage(&resizedImage, newSize);
    _image = resizedImage;
    _modified = false;
    update();
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
        _lastPoint = event->pos();
        _isDrawing = true;
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
   drawCursor();
    if ((event->buttons() & Qt::LeftButton) && _isDrawing)
        drawLineTo(event->pos());
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _isDrawing) {
        drawLineTo(event->pos());
        _isDrawing = false;
    }
}

void DrawingArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, _image, dirtyRect);
}

void DrawingArea::resizeEvent(QResizeEvent *event)
{
    if (width() > _image.width() || height() > _image.height()) {
        int newWidth = qMax(width() + 128, _image.width());
        int newHeight = qMax(height() + 128, _image.height());
        resizeImage(&_image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void DrawingArea::drawCursor()
{
    QPixmap cursor = QPixmap(_penWidth+1,_penWidth+1);cursor.fill(Qt::transparent);
    QPainter painter(&cursor);
    painter.setPen(Qt::black);
    painter.setBrush(_penColor);
    painter.drawEllipse(0,0,_penWidth,_penWidth);
    setCursor(QCursor(cursor));
}

void DrawingArea::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(_lastPoint, endPoint);
    _modified = true;

    int rad = (_penWidth / 2) + 2;
    update(QRect(_lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    _lastPoint = endPoint;
}

void DrawingArea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
