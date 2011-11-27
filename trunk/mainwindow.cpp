#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "textcollageshape.h"
#include "customcollageshape.h"
#include "application.h"
#include "imageutils.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

#include <QStyleFactory>
#include <QTranslator>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widgetCollageCreator->setMainWindow(this);

    //styles
    foreach(QString style, QStyleFactory::keys())
    {
        ui->menuInterface->addAction(style);
    }

    //languages
    foreach (QString availLang, Application::availableLanguages())
    {
        // figure out nice names for locales
        QLocale locale(availLang);
        QString language = languageName(locale.name());
        QString country = QLocale::countryToString(locale.country());
        QString name = language + " (" + country + ")";

        QAction * a = ui->menuLanguage->addAction(name);
        a->setData(availLang);
    }


    //default collage border color
    QPalette palette = ui->labelBorderColor->palette();
    palette.setColor(ui->labelBorderColor->backgroundRole(), Qt::transparent);
    ui->labelBorderColor->setPalette(palette);

    //connecting ui widgets to their slots
    connect(ui->actionAddPhotosFromComputer,SIGNAL(triggered()),SLOT(addPhotosFromComputer()));
    connect(ui->actionAddPhotosFromWeb,SIGNAL(triggered()),SLOT(addPhotosFromWeb()));
    connect(ui->actionCreateCollage,SIGNAL(triggered()),SLOT(createCollage()));
    connect(ui->actionPrint,SIGNAL(triggered()),SLOT(printCollage()));
    connect(ui->actionSaveCollageAs,SIGNAL(triggered()),SLOT(saveCollage()));
    connect(ui->actionQuit,SIGNAL(triggered()),SLOT(close()));
    connect(ui->actionNewProject,SIGNAL(triggered()),SLOT(newCollageProject()));

    connect(ui->btnTextCollageShape,SIGNAL(clicked()),SLOT(openTextCollageShape()));
    connect(ui->labelCustomShape,SIGNAL(clicked()),SLOT(openCustomShapeEditor()));
    connect(ui->labelDragPhotoHere,SIGNAL(clicked()),SLOT(openCollageBackgroundPhoto()));
    connect(ui->labelBgColor,SIGNAL(clicked()),SLOT(chooseCollageBackgroundColor()));
    connect(ui->labelBorderColor,SIGNAL(clicked()),SLOT(chooseCollageBorderColor()));

    connect(ui->radioText,SIGNAL(clicked()),SLOT(openTextCollageShape()));
    connect(ui->radioExtra,SIGNAL(clicked()),SLOT(openCustomShapeEditor()));
    connect(ui->radioPhoto,SIGNAL(clicked()),SLOT(openCollageBackgroundPhoto()));
    connect(ui->radioColor,SIGNAL(clicked()),SLOT(chooseCollageBackgroundColor()));
    connect(ui->radioTransparent,SIGNAL(clicked()),SLOT(updatePhotoPreview()));

     connect(ui->doubleSpinBoxBorderSize,SIGNAL(valueChanged(double)),SLOT(updatePhotoPreview()));

    connect(ui->btnAutoCollageSize,SIGNAL(toggled(bool)),SLOT(updateAutoCollageSize(bool)));
    connect(ui->btnAutoPhotoSize,SIGNAL(toggled(bool)),SLOT(updateAutoPhotoSize(bool)));
    connect(ui->btnAutoNbPhotos,SIGNAL(toggled(bool)),SLOT(updateAutoNbPhotos(bool)));
    connect(ui->btnAutoInterPhotoDistance,SIGNAL(toggled(bool)),SLOT(updateAutoInterPhotoDistance(bool)));

    connect(ui->btnRestoreDefaultParameters,SIGNAL(clicked()),SLOT(resetCollageParameters()));

    connect(ui->menuInterface,SIGNAL(triggered(QAction*)),SLOT(updateStyle(QAction *)));
    connect(ui->menuLanguage,SIGNAL(triggered(QAction*)),SLOT(updateLanguage(QAction *)));

    connect(ui->actionAbout,SIGNAL(triggered()),SLOT(about()));
    connect(ui->actionHelp,SIGNAL(triggered()),SLOT(help()));

    connect(ui->widgetCollageCreator,SIGNAL(collageAvailable(bool)),ui->actionSaveCollageAs,SLOT(setEnabled(bool)));
    connect(ui->widgetCollageCreator,SIGNAL(collageAvailable(bool)),ui->actionPrint,SLOT(setEnabled(bool)));

    connect(ui->widgetPhotoLister,SIGNAL(currentPhotoChanged(QPixmap)),SLOT(updatePhotoPreview()));

    updatePhotoPreview();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::languageName(const QString &locale)
{
    QTranslator translator;
    translator.load(QString("cutecollage_") + locale, ":/translations");

    return translator.translate("MainWindow", "French");
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,tr("Cute Collage"),
                             tr("Do you really want to quit the application?"),
                             QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }

}

//Adds photos from the computer
void MainWindow::addPhotosFromComputer()
{
    ui->widgetPhotoLister->addPhotosFromComputer();
}

//Adds photos from the the web
void MainWindow::addPhotosFromWeb()
{
    ui->widgetPhotoLister->addPhotosFromWeb();

}

//Creates the collage (image)
void MainWindow::createCollage()
{
    ui->widgetCollageCreator->createCollage();
}

//Save the collage as (image)
void MainWindow::saveCollage()
{
    ui->widgetCollageCreator->saveCollage();
}

//prints the collage
void MainWindow::printCollage()
{
    ui->widgetCollageCreator->printCollage();
}

//Start a new collage project
void MainWindow::newCollageProject()
{
    if(ui->widgetPhotoLister->getPhotos().count()>0)
    {
        if(QMessageBox::question(this,tr("Cute Collage"),tr("Are you sure you want to start a new project?"),
                                 QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            ui->widgetPhotoLister->clearPhotos();
            ui->widgetCollageCreator->clearCollage();
        }
    }

}
//About Cute Collage dialog
void MainWindow::about()
{
    QMessageBox::information(this,tr("Cute Collage"),
                             tr("<b>Cute Collage</b> is a software that allows you to easily create photos collage."),
                             QMessageBox::Ok);
}

//Help of Cute Collage
void MainWindow::help()
{

}

//Opens the text collage form dialog
void MainWindow::openTextCollageShape()
{
    TextCollageShape dialog(this);

    if(dialog.exec()==QDialog::Accepted)
    {
        ui->lineEditText->setText(dialog.text());
        ui->lineEditText->setFont(dialog.displayFont());
        ui->radioText->setChecked(true);
    }

}

//Opens the custom shape editor
void MainWindow::openCustomShapeEditor()
{
    CustomCollageShape dialog(this);

    //we get the image to preload in the custom shape editor
    QImage image;
    if(ui->labelCustomShape->pixmap())
        image = ui->labelCustomShape->pixmap()->toImage();
    else
    {
        image = QImage(512,512,QImage::Format_Mono);
        image.fill(qRgb(255,255,255));
    }
    dialog.setImage(image);

    if(dialog.exec()==QDialog::Accepted)
    {
        ui->labelCustomShape->setPixmap(QPixmap::fromImage(dialog.image()));
        ui->radioExtra->setChecked(true);
    }

}

void MainWindow::openCollageBackgroundPhoto()
{
    //file dialog for selecting the image to load
    QString fileName = QFileDialog::getOpenFileName( this,tr("Open image"),
                                                     QDesktopServices::storageLocation(QDesktopServices::PicturesLocation),
                                                     ImageUtils::supportedImageFormatsFilter());

    if (!fileName.isEmpty())
    {
        ui->labelDragPhotoHere->setPixmap(QPixmap(fileName));
        ui->radioPhoto->setChecked(true);
        updatePhotoPreview();
    }
}

void MainWindow::chooseCollageBorderColor()
{
    QPalette palette = ui->labelBorderColor->palette();
    QColor color = QColorDialog::getColor(palette.color(ui->labelBorderColor->backgroundRole()),this);

    if(color.isValid())
    {
        palette.setColor(ui->labelBorderColor->backgroundRole(), color);
        ui->labelBorderColor->setPalette(palette);
        updatePhotoPreview();

    }


}

void MainWindow::chooseCollageBackgroundColor()
{
    QPalette palette = ui->labelBgColor->palette();
    QColor color = QColorDialog::getColor(palette.color(ui->labelBorderColor->backgroundRole()),this);

    if(color.isValid())
    {
        palette.setColor(ui->labelBgColor->backgroundRole(), color);
        ui->labelBgColor->setPalette(palette);
        ui->radioColor->setChecked(true);
        updatePhotoPreview();
    }


}

void MainWindow::updateCollageShape(int shape)
{
    ui->widgetCollageCreator->setShape((CollageCreator::Shape)shape);

}

void MainWindow::updateCollageBackground(int background)
{
    ui->widgetCollageCreator->setBackground((CollageCreator::Background)background);
}

void MainWindow::updateAutoCollageSize(bool autoOn)
{
    if(autoOn)
    {
        ui->btnAutoCollageSize->setText(tr("Auto"));
        ui->btnAutoInterPhotoDistance->setChecked(false);
        ui->btnAutoInterPhotoDistance->setText(tr("Manual"));
        ui->sliderInterPhotoDistance->setEnabled(true);
        ui->spinBoxInterPhotoDistance->setEnabled(true);
    }
    else
    {
        ui->btnAutoCollageSize->setText(tr("Manual"));
    }

}

void MainWindow::updateAutoPhotoSize(bool autoOn)
{
    if(autoOn)
    {
        ui->btnAutoPhotoSize->setText(tr("Auto"));
    }
    else
    {
        ui->btnAutoPhotoSize->setText(tr("Manual"));
    }
}

void MainWindow::updateAutoNbPhotos(bool autoOn)
{
    if(autoOn)
    {
        ui->btnAutoNbPhotos->setText(tr("Auto"));
    }
    else
    {
        ui->btnAutoNbPhotos->setText(tr("Manual"));
    }
}

void MainWindow::updateAutoInterPhotoDistance(bool autoOn)
{
    if(autoOn)
    {
        ui->btnAutoInterPhotoDistance->setText(tr("Auto"));
    }
    else
    {
        ui->btnAutoInterPhotoDistance->setText(tr("Manual"));
    }
}


void MainWindow::updateStyle(QAction *a)
{
    QString styleName = a->text();
    QApplication::setStyle(QStyleFactory::create(styleName));

}

void MainWindow::updateLanguage(QAction *a)
{
    QString lang = a->data().toString();
    Application::setLanguage(lang);

}

void MainWindow::updatePhotoPreview()
{
    QPixmap photo = ui->widgetPhotoLister->getCurrentPhoto();

    if(photo.isNull())
    {
        photo = QPixmap(200,200);
        photo.fill();
    }

    qreal spacing = ui->doubleSpinBoxBorderSize->value();

    int w = photo.width() +  (int) ((photo.width() * spacing)/100);
    int h = photo.height() + (int) ((photo.height() * spacing)/100);


    QColor borderColor = ui->labelBorderColor->palette().color(ui->labelBorderColor->backgroundRole());
    QColor bgColor;

    //if the background is a color
    if(ui->radioColor->isChecked())
    {
        bgColor = ui->labelBgColor->palette().color(ui->labelBgColor->backgroundRole());
    }
    else
    {
        bgColor = QColor(Qt::transparent);
    }

    QPixmap pixmap1(w,h);
    pixmap1.fill(borderColor.toRgb());

    QPixmap pixmap2(w+40,h+40);
    pixmap2.fill(bgColor.toRgb());

    QPainter painter1(&pixmap1);
    QPainter painter2(&pixmap2);

    //drawing the photot with the border color
    painter1.drawPixmap((w-photo.width())/2,(h-photo.height())/2,photo);

     //if the background is a photo, we draw it
    if(ui->radioPhoto->isChecked() && !ui->labelDragPhotoHere->pixmap()->isNull())
    {
        QPixmap bgPixmap(*ui->labelDragPhotoHere->pixmap());
        painter2.drawPixmap(0,0,bgPixmap.scaled(w+40,h+40));
    }

    painter2.drawPixmap(20,20,pixmap1);

    //scaling and displaying the preview photo
    QSize size = ui->labelPhotoPreview->size();
    ui->labelPhotoPreview->setPixmap(pixmap2.scaled(size,Qt::KeepAspectRatio));
}

//Resets the collage parameters
void MainWindow::resetCollageParameters()
{
    if(QMessageBox::question(this,tr("Reset collage parameters"),
                             tr("Are you sure you want to reset collage parameters?"),
                             QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        ui->radioRectangle->setChecked(true);
        ui->btnAutoCollageSize->setChecked(true);

        ui->lineEditCollageWidth->setText("1366");
        ui->lineEditCollageHeight->setText("768");
        ui->lineEditPhotoSize->setText("200");
        ui->lineEditNbPhotos->setText("150");

        ui->sliderInterPhotoDistance->setValue(67);
        ui->radioColor->setChecked(true);

        QPalette palette = ui->labelBgColor->palette();
        palette.setColor(ui->labelBgColor->backgroundRole(), Qt::white);
        ui->labelBgColor->setPalette(palette);

        palette = ui->labelBorderColor->palette();
        palette.setColor(ui->labelBorderColor->backgroundRole(), Qt::transparent);
        ui->labelBorderColor->setPalette(palette);

        ui->spinBoxAvgAngle->setValue(0);
        ui->spinBoxMaxAngle->setValue(60);


        ui->checkBoxNoShadow->setChecked(false);
        ui->checkBoxJaggedBoundary->setChecked(false);
        ui->checkBoxPhotosSameSize->setChecked(true);


    }

}

///Gets the collage parameters
bool MainWindow::getCollageParameters()
{
    //getting the collage shape
    if(ui->radioRectangle->isChecked())
        ui->widgetCollageCreator->_shape = CollageCreator::Rectangle;
    else if(ui->radioCircle->isChecked())
        ui->widgetCollageCreator->_shape = CollageCreator::Circle;
    else if(ui->radioHeart->isChecked())
        ui->widgetCollageCreator->_shape = CollageCreator::Heart;
    else if(ui->radioText->isChecked())
    {
        if(ui->lineEditText->text().isEmpty())
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Please enter a text for the collage shape."),QMessageBox::Ok);
            return false;
        }

        ui->widgetCollageCreator->_shape = CollageCreator::Text;
        ui->widgetCollageCreator->_text = ui->lineEditText->text();
        ui->widgetCollageCreator->_font = ui->lineEditText->font();
    }
    else if(ui->radioExtra->isChecked())
    {
        if(ui->labelCustomShape->pixmap()->isNull())
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Please select a photo for the collage shape."),QMessageBox::Ok);
            return false;
        }

         ui->widgetCollageCreator->_shape = CollageCreator::Extra;
        ui->widgetCollageCreator->_extraPixmap =*ui->labelCustomShape->pixmap();
    }


    //getting the collage size
    ui->widgetCollageCreator->_collageSizeAuto = ui->btnAutoCollageSize->isChecked();

    if(!ui->widgetCollageCreator->_collageSizeAuto)
    {
        int pos = 0, width, height;

        QIntValidator sizeValidator(1,10000,this);
        QString str = ui->lineEditCollageHeight->text();
        //validating the given collage height
        if(sizeValidator.validate(str,pos)==QValidator::Acceptable)
        {
            height = str.toInt();
        }
        else
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Incorrect collage size. The collage height must be positive."),QMessageBox::Ok);
            return false;
        }
        str = ui->lineEditCollageWidth->text();
        //validating the given collage width
        if(sizeValidator.validate(str,pos)==QValidator::Acceptable)
        {
            width = str.toInt();
        }
        else
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Incorrect collage size. The collage width must be positive."),QMessageBox::Ok);
            return false;
        }

        ui->widgetCollageCreator->_collageSize = QSize(width,height);

    }

    //getting the photo size
    ui->widgetCollageCreator->_photoSizeAuto = ui->btnAutoPhotoSize->isChecked();

    if(!ui->widgetCollageCreator->_photoSizeAuto)
    {
        int pos = 0,size;

        QIntValidator sizeValidator(1,10000,this);
        QString str =ui->lineEditPhotoSize->text();
        //validating the given collage height
        if(sizeValidator.validate(str,pos)==QValidator::Acceptable)
        {
            size = str.toInt();
            ui->widgetCollageCreator->_photoSize = QSize(size,size);
        }
        else
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Incorrect photo size. The photo size must be positive."),QMessageBox::Ok);
            return false;
        }
    }

    //getting the number of photos
    ui->widgetCollageCreator->_nbPhotosAuto = ui->btnAutoNbPhotos->isChecked();

    if(!ui->widgetCollageCreator->_nbPhotosAuto)
    {
        if(ui->radioAllPhotos->isChecked())
            ui->widgetCollageCreator->_nbPhotos = ui->widgetPhotoLister->getPhotos().count();
        else
        {
            int pos = 0;
            QIntValidator validator(1,10000,this);
            QString str = ui->lineEditNbPhotos->text();

            //validating the given number of photos
            if(validator.validate(str,pos)==QValidator::Acceptable)
            {
                ui->widgetCollageCreator->_nbPhotos = str.toInt();
            }
            else
            {
                QMessageBox::critical(this,tr("Cute Collage"),tr("Incorrect number of photos."),QMessageBox::Ok);
                return false;
            }
        }
    }

    //getting the distance between photos
    ui->widgetCollageCreator->_distBetweenPhotosAuto = ui->btnAutoInterPhotoDistance->isChecked();

    if(!ui->widgetCollageCreator->_distBetweenPhotosAuto)
        ui->widgetCollageCreator->_distBetweenPhotos = ui->sliderInterPhotoDistance->value();


    //getting the collage background color/photo
    if(ui->radioColor->isChecked())
    {
        QPalette palette = ui->labelBgColor->palette();
        ui->widgetCollageCreator->_background = CollageCreator::Color;
        ui->widgetCollageCreator->_bgColor =  palette.color(ui->labelBgColor->backgroundRole());
    }
    else if(ui->radioTransparent->isChecked())
    {
        ui->widgetCollageCreator->_background = CollageCreator::Transparent;
        ui->widgetCollageCreator->_bgColor =  Qt::transparent;
    }
    else if(ui->radioPhoto->isChecked())
    {
        if(ui->labelDragPhotoHere->pixmap()->isNull())
        {
            QMessageBox::critical(this,tr("Cute Collage"),tr("Please select a photo for the collage shape."),QMessageBox::Ok);
            return false;
        }

        ui->widgetCollageCreator->_bgPixmap =*ui->labelDragPhotoHere->pixmap();
    }

    //getting the collage border color
    QPalette palette = ui->labelBorderColor->palette();
    ui->widgetCollageCreator->_borderColor =  palette.color(ui->labelBorderColor->backgroundRole());

    //getting the collage border size
    ui->widgetCollageCreator->_borderSize = ui->doubleSpinBoxBorderSize->value();


    //Advanced options
     ui->widgetCollageCreator->_irregularBorder = ui->checkBoxJaggedBoundary->isChecked();
     ui->widgetCollageCreator->_identicalPhotoSize = ui->checkBoxPhotosSameSize->isChecked();
     ui->widgetCollageCreator->_horizontalOffset = ui->spinBoxOffsetX->value();
    ui->widgetCollageCreator->_verticalOffset = ui->spinBoxOffsetY->value();
    return true;
}

QList<QPixmap> MainWindow::getPhotos() const
{
    return ui->widgetPhotoLister->getPhotos();
}
