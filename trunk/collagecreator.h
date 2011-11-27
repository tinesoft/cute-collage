#ifndef COLLAGECREATOR_H
#define COLLAGECREATOR_H


#include <QList>
#include <QSize>
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QPixmap>
#include <QPainterPath>

#include "graphicsview.h"

#include <QFutureWatcher>

class MainWindow;

namespace Ui {
    class CollageCreator;
}

/**
  * @class CollageCreator
  * @brief This widget allows to create a phtoto collage.
  *
  * The widget allows to create a photo collage based on parameters
  * defined by the user such as the collage shape, the space between
  * each photo in the collage, etc.
  *
  * @ref CollageCreator is integrated in the QuteCollage's main window,
  *  but can also be used as a standalone program.
  *
  */
class CollageCreator : public QWidget
{
    Q_OBJECT

public:
    friend class MainWindow;
    /**
      * @enum Shape
      * @brief This enum defines the collage's shape
      */
    enum Shape
    {
        Rectangle,
        Heart,
        Circle,
        Text,
        Extra,
        Random
    };

    /**
      * @enum Background
      * @brief This enum defines the collage's background
      */
    enum Background
    {
        Color,
        Transparent,
        Photo
    };
public slots:
    ///Previews the collage
    void previewCollage();
    ///Creates the collage (image)
    void createCollage();
    ///Save the already created collage (image)
    void saveCollage();
    ///Prints the collage (image)
    void printCollage();
    ///Activates/Desactivates the animation of the collage's layout
    void updateCollageLayoutAnim(bool enable);


    void setShape(Shape shape)  { _shape = shape;}
    void setBackground(Background background) { _background = background; }

protected:
    void changeEvent(QEvent* event);
public:
    explicit CollageCreator(QWidget *parent = 0);
    explicit CollageCreator(MainWindow * mainWindow, QWidget *parent = 0);
    ~CollageCreator();

    void setMainWindow(MainWindow * mainWindow);
    void clearCollage();
signals:
    void  collageAvailable(bool yes);
private:
    ///Gets the current collage (image)
    QImage getCollage();
    QPixmap toCollagePhoto(const QPixmap &photo);
    void setup();
private:
    MainWindow * _mainWindow;
    Ui::CollageCreator *ui;


    QGraphicsScene * _graphicsScene;
    QFutureWatcher<void> * _collageWatcher;
    QList<QPixmap>  _photos;


    //Shape and size options
    Shape   _shape;
    QPixmap _extraPixmap; /*!<Only used when the collage'shape is Extra*/
    QString _text;  /*!<Only used when the collage'shape is Text*/
    QFont   _font;  /*!<Only used when the collage'shape is Text*/
    QSize   _collageSize;
    QSize   _photoSize;
    int     _nbPhotos;
    int     _distBetweenPhotos;

    bool    _collageSizeAuto;
    bool    _photoSizeAuto;
    bool    _distBetweenPhotosAuto;
    bool    _nbPhotosAuto;
    bool    _animateCollageLayout;

    //Apprearance options
    QColor  _bgColor;
    QPixmap _bgPixmap;
    QColor  _borderColor;
    qreal   _borderSize;
    Background  _background;

    //Advanced options
    qreal     _horizontalOffset;
    qreal     _verticalOffset;
    bool    _irregularBorder;
    bool    _identicalPhotoSize;



};

#endif // COLLAGECREATOR_H
