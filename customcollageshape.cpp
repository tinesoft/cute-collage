#include "customcollageshape.h"
#include "ui_customcollageshape.h"
#include "drawingarea.h"
#include "imageutils.h"
#include <QtGui>
#include <QDesktopServices>
#include <QFileDialog>
#include <QStandardPaths>

CustomCollageShape::CustomCollageShape(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CustomCollageShape)
{
    ui->setupUi(this);

    resize(500,315);

    //we fill the by default shape combobox
    ui->comboBoxShapeImage->clear();
    ui->comboBoxShapeImage->addItem(QIcon(),tr("Custom"));
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-heart.png"),"",":/images/mask-heart.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-heart-outline.png"),"",":/images/mask-heart-outline.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-circle.png"),"",":/images/mask-circle.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-pawprint.png"),"",":/images/mask-pawprint.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-cat.png"),"",":/images/mask-cat.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-butterfly.png"),"",":/images/mask-butterfly.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-puzzle.png"),"",":/images/mask-puzzle.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-star2.png"),"",":/images/mask-star2.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-moon.png"),"",":/images/mask-moon.png");
    ui->comboBoxShapeImage->addItem(QIcon(":/images/mask-world.png"),"",":/images/mask-world.png");
    ui->comboBoxShapeImage->addItem(QIcon(),tr("Extra"));

    //connection to slots
    connect(ui->btnBrush, SIGNAL(clicked()), this, SLOT(selectBrush()));
    connect(ui->btnEraser, SIGNAL(clicked()), this, SLOT(selectEraser()));
    connect(ui->btnLoad, SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveDrawingArea()));
    connect(ui->btnReset, SIGNAL(clicked()), this, SLOT(resetDrawingArea()));
    connect(ui->sliderBrushSize, SIGNAL(valueChanged(int)), this, SLOT(changeBrushSize(int)));
    connect(ui->comboBoxShapeImage,SIGNAL(currentIndexChanged(int)),SLOT(updateCollageShapeImage(int)));
}

CustomCollageShape::~CustomCollageShape()
{
    delete ui;
}

void CustomCollageShape::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // all designed forms have retranslateUi() method
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void CustomCollageShape::saveDrawingArea()
{
    //getting the location where to save the image to
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save image as ..."),
                        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
                        ImageUtils::supportedImageFormatsFilter());

    if (!fileName.isEmpty())
        ui->drawingArea->saveImage(fileName);
}

void CustomCollageShape::resetDrawingArea()
{
    ui->drawingArea->resetImage();
}

void CustomCollageShape::loadImage()
{
    //file dialog for selecting the image to load
    QString fileName = QFileDialog::getOpenFileName( this,tr("Open image"),
                        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
                            ImageUtils::supportedImageFormatsFilter());

    if (!fileName.isEmpty())
        ui->drawingArea->loadImageToFormat(fileName,QImage::Format_Mono); //we load the image in grayscale
}

void CustomCollageShape::selectBrush()
{
    if (ui->btnBrush->isChecked())
        ui->drawingArea->setPenColor(Qt::black);
}

void CustomCollageShape::selectEraser()
{
    if (ui->btnEraser->isChecked())
        ui->drawingArea->setPenColor(Qt::white);
}

void CustomCollageShape::changeBrushSize(int value)
{
    ui->drawingArea->setPenWidth(value);
}

void CustomCollageShape::updateCollageShapeImage(int index)
{
    if(index == 0)
    {

    }
    else if(index == ui->comboBoxShapeImage->count()-1)
    {//we open this website for downloading more shapes
        QDesktopServices::openUrl(QUrl("http://www.shapecollage.com/shapes.html"));
    }
    else
    {//we open the given default shape
        QString defaultShape = ui->comboBoxShapeImage->itemData(index).toString();
        ui->drawingArea->loadImage(defaultShape);
    }

}
void CustomCollageShape::setImage(const QImage &image)
{
   ui->drawingArea->setImage(image);
}

QImage CustomCollageShape::image() const
{
    return ui->drawingArea->image();
}
