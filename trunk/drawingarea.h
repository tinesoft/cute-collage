#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QPoint>

class DrawingArea : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingArea(QWidget *parent = 0);

    bool loadImage(const QString &fileName);
    bool loadImageToFormat(const QString &fileName,QImage::Format format, Qt::ImageConversionFlags flags = Qt::AutoColor);
    bool saveImage(const QString &fileName, const char *fileFormat=0);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    void setImage(const QImage &image);

    bool isModified() const { return _modified; }
    QColor penColor() const { return _penColor; }
    int penWidth() const { return _penWidth; }
    QImage image() const { return _image; }

signals:

public slots:
    void resetImage();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawCursor();
    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    bool _modified;
    bool _isDrawing;
    int _penWidth;
    QColor _penColor;
    QImage _image;
    QPoint _lastPoint;

};

#endif // DRAWINGAREA_H
