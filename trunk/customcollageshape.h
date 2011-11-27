#ifndef CUSTOMCOLLAGESHAPE_H
#define CUSTOMCOLLAGESHAPE_H

#include <QDialog>
#include <QImage>

namespace Ui {
    class CustomCollageShape;
}

class CustomCollageShape : public QDialog
{
    Q_OBJECT

public:
    explicit CustomCollageShape(QWidget *parent = 0);
    ~CustomCollageShape();
    QImage image() const;
    void setImage(const QImage &image);
protected:
    void changeEvent(QEvent* event);
private:
    Ui::CustomCollageShape *ui;

public slots:
    void changeBrushSize(int value);
    void selectEraser();
    void selectBrush();
    void loadImage();
    void saveDrawingArea();
    void resetDrawingArea();
   void  updateCollageShapeImage(int index);
};

#endif // CUSTOMCOLLAGESHAPE_H
