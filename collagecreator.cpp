#include "collagecreator.h"
#include "ui_collagecreator.h"
#include "mainwindow.h"

#include <QtDebug>
#include "imageutils.h"

#include <math.h>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include <QPrinter>
#include <QFileDialog>
#include <QPrintDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStandardPaths>

CollageCreator::CollageCreator(QWidget *parent) :
        QWidget(parent),_mainWindow(0),ui(new Ui::CollageCreator)
{
    setup();
}
CollageCreator::CollageCreator(MainWindow * mainWindow, QWidget *parent) :
        QWidget(parent),_mainWindow(mainWindow),ui(new Ui::CollageCreator)
{
    setup();
}

void CollageCreator::setup()
{
    ui->setupUi(this);

    _graphicsScene  = new QGraphicsScene(this) ;
    _collageWatcher = new QFutureWatcher<void>(this);

    ui->graphicsView->setScene(_graphicsScene);

    ui->labelCollageNbPhotos->setVisible(false);
    ui->labelCollageProgression->setVisible(false);
    ui->labelCollageSize->setVisible(false);

    _graphicsScene->addText(tr("Add Photos and\nClick on \"Create\""));

    //connecting the main button of the widget to their slots
    connect(ui->btnPreviewCollage,SIGNAL(clicked()),SLOT(previewCollage()));
    connect(ui->btnCreateCollage,SIGNAL(clicked()),SLOT(createCollage()));
    connect(ui->checkBoxAnimateCollage,SIGNAL(toggled(bool)),SLOT(updateCollageLayoutAnim(bool)));
}

CollageCreator::~CollageCreator()
{
    _collageWatcher->cancel();
    _collageWatcher->waitForFinished();
}

void CollageCreator::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);

        ui->labelCollageProgression->setText(tr("Analyzing photos..."));
    }
    QWidget::changeEvent(event);
}

void CollageCreator::setMainWindow(MainWindow * mainWindow)
{
    _mainWindow = mainWindow;
}

//Previews the collage
void CollageCreator::previewCollage()
{
    //we check if we have added photos for the collage. If not, we return
    if(!_mainWindow || _mainWindow->getPhotos().isEmpty())
    {
        QMessageBox::critical(this,tr("Cute Collage"),tr("No photo added! Please add some photos and try again."),
                              QMessageBox::Ok);
        return ;
    }

    //we get the parameters of the collage. If incorrect, we return
    if(!_mainWindow->getCollageParameters())
    {
        return;
    }

    //we get the photos
    _photos = _mainWindow->getPhotos();

    _graphicsScene->clear();


    if(_collageSizeAuto)
    {
        //automatic collage size, based on the biggest photo size
        foreach(QPixmap photo, _photos)
        {
            if(photo.width()>_collageSize.width())
                _collageSize.setWidth(photo.width());

            if(photo.height()>_collageSize.height())
                _collageSize.setHeight(photo.height());
        }
        _collageSize += QSize(30,30);
    }

    //updating the informations about the number of photos, and the collage size
    ui->labelCollageSize->setVisible(true);
    ui->labelCollageNbPhotos->setVisible(true);

    ui->labelCollageNbPhotos->setText( tr("Number of Photos : %1").arg(_photos.count()));
    ui->labelCollageSize->setText(tr("Collage Size: %1 x %2").arg(_collageSize.width()).arg(_collageSize.height()));

    _graphicsScene->setSceneRect(0,0,_collageSize.width(),_collageSize.height());

    //collage background
    switch(_background)
    {
    case Color:
    case Transparent:   _graphicsScene->setBackgroundBrush(QBrush(_bgColor));break;
    default :           _graphicsScene->addPixmap(_bgPixmap);
}

    ui->labelCollageProgression->setText(tr("Analyzing photos..."));
    ui->progressBarCollageCreation->setRange(1, _nbPhotos);

    // adding the photos to the collage
    QList<GraphicsPixmapItem *> items;
    for (int i = 0; i < _nbPhotos; ++i)
    {
        QPixmap photo =  toCollagePhoto(_photos[i % _photos.count()]); //we use '%' as _nbPhotos >= _photos.count()
        photo = photo.scaled(_photoSize);
        GraphicsPixmapItem *item = new GraphicsPixmapItem(photo);
        item->setOffset(-photo.width(), -photo.height());
        item->setZValue(i);
        items << item;
        _graphicsScene->addItem(item);

        ui->progressBarCollageCreation->setValue(i);
    }


    QParallelAnimationGroup *animGroup = new QParallelAnimationGroup;
    QPainterPath shapePath;


    int xOffset = _photoSize.width();
    int yOffset = _photoSize.height();

    //collage shape
    switch(_shape)
    {

    case Random:
    case Extra:
    case Rectangle: shapePath.addRect(xOffset,yOffset,_collageSize.width()-xOffset,_collageSize.height()-yOffset);break;
    case Circle:    shapePath.addEllipse(xOffset,yOffset,_collageSize.width()-xOffset,_collageSize.height()-yOffset);break;
    case Text:      shapePath.addText(xOffset,_collageSize.height()/2,_font,_text);break;

    case Heart:     shapePath.moveTo(QPoint(_collageSize.width()/2,_collageSize.height()/3));
                    shapePath.lineTo(QPoint(xOffset,yOffset));
                    shapePath.lineTo(QPoint(_collageSize.width()/2,_collageSize.height()-yOffset));
                    shapePath.lineTo(QPoint(_collageSize.width()-xOffset,yOffset));
                    shapePath.lineTo(QPoint(_collageSize.width()/2,_collageSize.height()/3));
        break;

    }

    //placing and animating the collage's photos
    qreal t = 0;
    for (int i = 0; i < items.count(); ++i)
    {
        QPropertyAnimation *anim = new QPropertyAnimation(items[i], "pos");
        QPropertyAnimation *anim2 = new QPropertyAnimation(items[i], "rotation");

        //setting the animation of the position of photos
        anim->setEasingCurve(QEasingCurve::InOutBack);
        anim->setDuration(750 + i * 25);
        anim->setStartValue(QPointF());
        anim->setEndValue( shapePath.pointAtPercent(t));        

        //setting the animation of the rotation of the photos
        anim2->setEasingCurve(QEasingCurve::InOutBack);
        anim2->setDuration(750 + i * 25);
        anim2->setStartValue(0);
        anim2->setEndValue(-20 + qrand()%20);

        t += 1.0/_nbPhotos;

        animGroup->addAnimation(anim);
        animGroup->addAnimation(anim2);

        ui->graphicsView->fitInView(_graphicsScene->sceneRect(), Qt::KeepAspectRatio);
    }
    //we start the animation of collage layout
    animGroup->start();

}

//Creates the collage (image)
void CollageCreator::createCollage()
{
    if(!_mainWindow || _mainWindow->getPhotos().isEmpty())
    {
        QMessageBox::critical(this,tr("Cute Collage"),tr("No photo added! Please add some photos and try again"),
                              QMessageBox::Ok);
        return ;
    }

    //getting the location where to save the image to
    QString collageName = QFileDialog::getSaveFileName(this,tr("Save collage as ..."),
                                                       QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
                                                       ImageUtils::supportedImageFormatsFilter());

    if(!collageName.isEmpty())
    {

        //we get the parameters of the collage. If incorrect, we return
        if(!_mainWindow->getCollageParameters())
        {
            return;
        }

        //we get the photos
        _photos = _mainWindow->getPhotos();

        _graphicsScene->clear();


        if(_collageSizeAuto)
        {
            //automatic collage size, based on the biggest photo size
            foreach(QPixmap photo, _photos)
            {
                if(photo.width()>_collageSize.width())
                    _collageSize.setWidth(photo.width());

                if(photo.height()>_collageSize.height())
                    _collageSize.setHeight(photo.height());
            }
            _collageSize += QSize(30,30);
        }

        //updating the informations about the number of photos, and the collage size
        ui->labelCollageSize->setVisible(true);
        ui->labelCollageNbPhotos->setVisible(true);

        ui->labelCollageNbPhotos->setText( tr("Number of Photos : %1").arg(_photos.count()));
        ui->labelCollageSize->setText(tr("Collage Size: %1 x %2").arg(_collageSize.width()).arg(_collageSize.height()));

        _graphicsScene->setSceneRect(0,0,_collageSize.width(),_collageSize.height());

        //collage background
        switch(_background)
        {
        case Color:
        case Transparent:   _graphicsScene->setBackgroundBrush(QBrush(_bgColor));break;
        default :           _graphicsScene->addPixmap(_bgPixmap);
    }

        ui->labelCollageProgression->setText(tr("Analyzing photos..."));
        ui->progressBarCollageCreation->setRange(1, _nbPhotos);


        int xOffset = _photoSize.width();
        int yOffset = _photoSize.height();

        //collage shape
        QPainterPath shapePath;
        switch(_shape)
        {

        case Random:
        case Extra:
        case Rectangle: shapePath.addRect(xOffset,yOffset,_collageSize.width()-xOffset,_collageSize.height()-yOffset);break;
        case Circle:    shapePath.addEllipse(xOffset,yOffset,_collageSize.width()-xOffset,_collageSize.height()-yOffset);break;
        case Text:      shapePath.addText(xOffset,_collageSize.height()/2,_font,_text);break;

        case Heart:     shapePath.moveTo(QPoint(_collageSize.width()/2,_collageSize.height()/3));
                        shapePath.lineTo(QPoint(xOffset,yOffset));
                        shapePath.lineTo(QPoint(_collageSize.width()/2,_collageSize.height()-yOffset));
                        shapePath.lineTo(QPoint(_collageSize.width()-xOffset,yOffset));
                        shapePath.lineTo(QPoint(_collageSize.width()/2,_collageSize.height()/3));
            break;

        }

        //placing the collage's photos
        qreal t = 0;
        // adding the photos to the collage
        QList<GraphicsPixmapItem *> items;
        for (int i = 0; i < _nbPhotos; ++i)
        {
            QPixmap photo =  toCollagePhoto(_photos[i % _photos.count()]); //we use '%' as _nbPhotos >= _photos.count()
            photo = photo.scaled(_photoSize);
            GraphicsPixmapItem *item = new GraphicsPixmapItem(photo);
            item->setOffset(-photo.width(), -photo.height());
            item->setZValue(i);
            item->setPos(shapePath.pointAtPercent(t));
            item->setRotation(-20 + qrand()%20);
            items << item;

            t += 1.0/_nbPhotos;

            _graphicsScene->addItem(item);

             ui->graphicsView->fitInView(_graphicsScene->sceneRect(), Qt::KeepAspectRatio);
            ui->progressBarCollageCreation->setValue(i);
        }

        //we save the collage(image) to computer
        getCollage().save(collageName);
        emit collageAvailable(true);
    }
}

//Save the already created collage (image)
void CollageCreator::saveCollage()
{
    //getting the location where to save the image to
    QString collageName = QFileDialog::getSaveFileName(this,tr("Save collage as ..."),
                                                       QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
                                                       ImageUtils::supportedImageFormatsFilter());

    if(!collageName.isEmpty())
    {
        //we save the collage(image) to computer
        getCollage().save(collageName);
    }
}

void CollageCreator::clearCollage()
{
    _graphicsScene->clear();
    _graphicsScene->addText(tr("Add Photos and\nClick on \"Create\""));
    ui->labelCollageNbPhotos->setVisible(false);
    ui->labelCollageSize->setVisible(false);
    emit collageAvailable(false);
}

//Prints the collage (image)
void CollageCreator::printCollage()
{
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {

        QPainter painter(&printer);
        //version 1
        /*

        QRect rect = painter.viewport();
        QSize size = getCollage.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(getCollage.rect());
        painter.drawImage(0, 0, getCollage);
        */

        //version 2
        //TODO (which is better?)
        _graphicsScene->render(&painter);

    }

}

//Gets the current collage (image)
QImage CollageCreator::getCollage()
{
    QImage collageImage(_collageSize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&collageImage);
    collageImage.fill(_bgColor.rgb());
    _graphicsScene->render(&painter);
    return collageImage;
}

QPixmap CollageCreator::toCollagePhoto(const QPixmap &photo)
{

    int w = photo.width() +  (int) ((photo.width() * _borderSize)/100);
    int h = photo.height() + (int) ((photo.height() * _borderSize)/100);

    QPixmap pixmap(w,h);
    pixmap.fill(_borderColor.toRgb());

    QPainter painter(&pixmap);
    //drawing the photo with the border color
    painter.drawPixmap((w-photo.width())/2,(h-photo.height())/2,photo);
    return pixmap;
}

void CollageCreator::updateCollageLayoutAnim(bool enable)
{
    _animateCollageLayout = enable;
}

