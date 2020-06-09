#include "photolister.h"
#include "ui_photolister.h"
#include "imageutils.h"

#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QImage>
#include <QMatrix>
#include <QFileDialog>
#include <QMimeData>
#include <QMessageBox>
#include <QtConcurrentMap>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QStandardPaths>


PhotoLister::PhotoLister(QWidget *parent) :
        QWidget(parent),ui(new Ui::PhotoLister)
{
    ui->setupUi(this);

    //watchers to manage threads for adding, rotating photos
    _photoAddingWatcher = new QFutureWatcher<PixmapWrapper>(this);
    //_photoRotatingWatcher = new QFutureWatcher<void>(this);

    //progress dialog to show the progress of heavy computations
    _progressDialog = new QProgressDialog(this);
    _progressDialog->setWindowModality(Qt::WindowModal);

    //context menu
    _contextMenu = new QMenu(this);
    _contextMenu->addAction(ui->actionAddPhotoFromComputer);
    _contextMenu->addAction(ui->actionAddPhotoFromWeb);
    _contextMenu->addSeparator();
    _contextMenu->addAction(ui->actionPreviewPhoto);
    _contextMenu->addSeparator();
    _contextMenu->addAction(ui->actionRotateLeft);
    _contextMenu->addAction(ui->actionRotateRight);
    _contextMenu->addSeparator();
    _contextMenu->addAction(ui->actionRemoveSelectedPhotos);
    _contextMenu->addAction(ui->actionRemoveAllPhotos);
    _contextMenu->addSeparator();
    _contextMenu->addAction(ui->actionSelectAllPhotos);

    //connecting the main buttons of the widget to their slots
    connect(ui->btnAddPhoto,SIGNAL(clicked()),SLOT(addPhotosFromComputer()));
    connect(ui->btnRemovePhoto,SIGNAL(clicked()),SLOT(removeSelectedPhotos()));
    connect(ui->btnRemovePhotos,SIGNAL(clicked()),SLOT(removeAllPhotos()));

    //connecting the watcher for managing images loading in threads
    connect(_photoAddingWatcher,SIGNAL(progressValueChanged(int)),_progressDialog,SLOT(setValue(int)));
    connect(_photoAddingWatcher,SIGNAL(resultReadyAt(int)),SLOT(photoLoadedAt(int)));
    connect(_photoAddingWatcher,SIGNAL(finished()),SLOT(addPhotosFinished()));
    connect(_progressDialog,SIGNAL(canceled()),_photoAddingWatcher,SLOT(cancel()));


    //connecting context menu actions to their slots
    connect(ui->actionAddPhotoFromComputer,SIGNAL(triggered()),SLOT(addPhotosFromComputer()));
    connect(ui->actionAddPhotoFromWeb,SIGNAL(triggered()),SLOT(addPhotosFromWeb()));
    connect(ui->actionRemoveAllPhotos,SIGNAL(triggered()),SLOT(removeAllPhotos()));
    connect(ui->actionRemoveSelectedPhotos,SIGNAL(triggered()),SLOT(removeSelectedPhotos()));
    connect(ui->actionRotateLeft,SIGNAL(triggered()),SLOT(rotatePhotosLeft()));
    connect(ui->actionRotateRight,SIGNAL(triggered()),SLOT(rotatePhotosRight()));
    connect(ui->actionSelectAllPhotos,SIGNAL(triggered()),ui->listWidgetPhotos,SLOT(selectAll()));
    connect(ui->actionPreviewPhoto,SIGNAL(triggered()),SLOT(previewPhoto()));

    connect(ui->listWidgetPhotos,SIGNAL(itemSelectionChanged()),SLOT(updateCurrentPhoto()));

    //context menu on right click
    connect(ui->listWidgetPhotos,SIGNAL(customContextMenuRequested(QPoint)),SLOT(showContextMenu(QPoint)));

    //photo preview on double click
    connect(ui->listWidgetPhotos,SIGNAL(itemDoubleClicked(QListWidgetItem *)),SLOT(previewPhoto(QListWidgetItem *)));
}

PhotoLister::~PhotoLister()
{
    delete ui;

    _photoAddingWatcher->cancel();
    _photoAddingWatcher->waitForFinished();
}

void PhotoLister::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}


QList<QPixmap> PhotoLister::getPhotos() const
{
    QList<QPixmap> photos;
    for(int i = 0; i < ui->listWidgetPhotos->count(); i++)
    {
        PixmapListWidgetItem * item = (PixmapListWidgetItem*) ui->listWidgetPhotos->item(i);
        photos << item->pixmap();

    }

    return photos;

}

QPixmap PhotoLister::getCurrentPhoto() const
{
    QList<QListWidgetItem *> selectedPhotos = ui->listWidgetPhotos->selectedItems();

    if(selectedPhotos.isEmpty())
    {
        if(ui->listWidgetPhotos->count()>0)
        {
            PixmapListWidgetItem * item = (PixmapListWidgetItem *) ui->listWidgetPhotos->item(0);
            return item->pixmap();
        }
        else
            return QPixmap();
    }
    else
    {
        PixmapListWidgetItem * item = (PixmapListWidgetItem *) selectedPhotos[0];
        return item->pixmap();
    }
}

void PhotoLister::clearPhotos()
{
    _addedImagesList.clear();
    ui->listWidgetPhotos->clear();
    ui->labelNbPhotos->setText(tr("%n Photo(s)","",0));
    setListPhotosEmpty(true);
}

///////////////////////////////////////////////
// slot-methods for handling signals
///////////////////////////////////////////////

void PhotoLister::showContextMenu(const QPoint &pos)
{
    int nbSelectedPhotos = ui->listWidgetPhotos->selectedItems().count();

    //we enable/disable some menu items, depending on the number of selected photos
    ui->actionPreviewPhoto->setEnabled(nbSelectedPhotos == 1);
    ui->actionRotateLeft->setDisabled(nbSelectedPhotos == 0);
    ui->actionRotateRight->setDisabled(nbSelectedPhotos == 0);
    ui->actionRemoveAllPhotos->setDisabled(nbSelectedPhotos == 0);
    ui->actionSelectAllPhotos->setDisabled(nbSelectedPhotos == 0);
    ui->actionRemoveSelectedPhotos->setDisabled(nbSelectedPhotos == 0);


    //we display the menu at the given position
    _contextMenu->popup(mapToGlobal(pos));

}

//Previews the current photo in the system default image viewer
void PhotoLister::previewPhoto()
{
    PixmapListWidgetItem * item = (PixmapListWidgetItem *)ui->listWidgetPhotos->currentItem();
    //we open the image in the default system image viewer
    QDesktopServices::openUrl(QUrl("file:///"+item->fileName(), QUrl::TolerantMode));
}

//Previews the given photo in the system default image viewer
void PhotoLister::previewPhoto(QListWidgetItem * imageItem)
{
    PixmapListWidgetItem * item = (PixmapListWidgetItem *)imageItem;
    //we open the image in the default system image viewer
    QDesktopServices::openUrl(QUrl("file:///"+item->fileName(), QUrl::TolerantMode));
}

//Adds a set of photos from the computer
void PhotoLister::addPhotosFromComputer()
{
    // this filter allows to only make appear supported images in the file dialog
    QString filter = ImageUtils::supportedImageFormatsFilter();

    //file dialog for selecting images to add
    QStringList imagesFileNames = QFileDialog::getOpenFileNames(
            this,tr("Select one or more photos"),
            QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),
            filter);

    if(imagesFileNames.isEmpty()) return ;

    addImages(imagesFileNames);

}

//Adds a set of phtoto from the web
void PhotoLister::addPhotosFromWeb()
{
    //opening Flickr.com or Google Picassa for exploring photos
    if(qrand()%2)
        QDesktopServices::openUrl(QUrl("http://www.flickr.com/explore/",QUrl::TolerantMode));
    else
        QDesktopServices::openUrl(QUrl("https://picasaweb.google.com/lh/explore",QUrl::TolerantMode));
}

void PhotoLister::removeSelectedPhotos()
{
    int nbPhotos = ui->listWidgetPhotos->selectedItems().count();
    QList<QListWidgetItem*> selectedPhotos = ui->listWidgetPhotos->selectedItems();

    setProgressDialog(tr("Removing selected photos..."), tr("Cancel"), 0, nbPhotos);

    for (int i = 0; i < nbPhotos; i++)
    {
        _progressDialog->setValue(i);

        if (_progressDialog->wasCanceled()) break; //removing photos cancelled

        //we first remove the photo from the added images list
        PixmapListWidgetItem * imageItem = (PixmapListWidgetItem*) selectedPhotos.at(i);
        _addedImagesList.removeOne(imageItem->fileName());

        //then we remove it from the list widget
        delete ui->listWidgetPhotos->takeItem(ui->listWidgetPhotos->row(imageItem));
    }

    _progressDialog->setValue(nbPhotos);

    ui->labelNbPhotos->setText(tr("%n Photo(s)","",ui->listWidgetPhotos->count()));
    setListPhotosEmpty(ui->listWidgetPhotos->count() == 0);
}

void PhotoLister::removeAllPhotos()
{
    if(QMessageBox::question(this,tr("Photos List"),
                             tr("Are you sure you want to remove all photos in the list?"),
                             QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        _addedImagesList.clear();
        ui->listWidgetPhotos->clear();
        ui->labelNbPhotos->setText(tr("%n Photo(s)","",0));
        setListPhotosEmpty(true);
    }
}

void PhotoLister::rotatePhotosLeft()
{

    QList<QListWidgetItem *> selectedItems = ui->listWidgetPhotos->selectedItems();
    int nbPhotos = selectedItems.count();

    setProgressDialog(tr("Rotating photos left..."), tr("Cancel"), 0, nbPhotos);

    for (int i = 0; i < nbPhotos; i++)
    {
        _progressDialog->setValue(i);//we update the progress dialog's progress bar

        if (_progressDialog->wasCanceled()) break; //adding photos cancelled

        rotateImageLeft(selectedItems[i]);

        qApp->processEvents();//to avoid the app to 'freeze'
    }

    _progressDialog->setValue(nbPhotos);

}

void PhotoLister::rotatePhotosRight()
{
    QList<QListWidgetItem *> selectedItems = ui->listWidgetPhotos->selectedItems();
    int nbPhotos = selectedItems.count();

    setProgressDialog(tr("Rotating photos right..."), tr("Cancel"), 0, nbPhotos);

    for (int i = 0; i < nbPhotos; i++)
    {
        _progressDialog->setValue(i);//we update the progress dialog's progress bar

        if (_progressDialog->wasCanceled()) break; //adding photos cancelled

        rotateImageRight(selectedItems[i]);

        qApp->processEvents();//to avoid the app to 'freeze'
    }

    _progressDialog->setValue(nbPhotos);

}

void PhotoLister::addPhotosFinished()
{
    //we re-enable the possibility to add new photos
    setAcceptDrops(true); //for adding by drag & drop
    ui->btnAddPhoto->setEnabled(true);//for adding by clicking


    //we display the list of last photos that could not be loaded/added, if any
    if(!_nonAddedImagesList.isEmpty())
        QMessageBox::warning(this,tr("Photos List"),
                             tr("The following photos could not be added :\n<b>%1</b>").arg(_nonAddedImagesList.join("\n")));

    _nonAddedImagesList.clear();

    setListPhotosEmpty(ui->listWidgetPhotos->count() == 0);
}

void PhotoLister::photoLoadedAt(int n)
{
    addImage(_photoAddingWatcher->resultAt(n));
}


///////////////////////////////////////////////
// methods for handling events
///////////////////////////////////////////////
void PhotoLister::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
    {
        removeSelectedPhotos();
    }
}

void PhotoLister::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->accept();
    }
}

void PhotoLister::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->accept();
    }
}

void PhotoLister::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void PhotoLister::dropEvent(QDropEvent*event)
{
    QStringList imagesFileNames;

    foreach(QUrl url,event->mimeData()->urls())
    {
        //if the files in the list are supported images, we add them to images list
        QString extension = QFileInfo(url.toString()).suffix().toLower();
        if(ImageUtils::supportedImageFormats().contains(extension))
        {
            if(url.toString().startsWith("file:///"))
                imagesFileNames<<url.toLocalFile().remove("file:///");
        }
    }

    addImages(imagesFileNames);

}

bool PhotoLister::addImage(const PixmapWrapper &pw)
{
    if(pw.pixmap().isNull())
    {
        _nonAddedImagesList <<"\t<b>"<< pw.fileName()<<"</b>";
        return false;
    }

    //if image is already in the list, we don't add it again
    if(_addedImagesList.contains(pw.fileName()))
        return true;
    else
        _addedImagesList<<pw.fileName();

    //first time that we add the image
    ui->listWidgetPhotos->addItem(new PixmapListWidgetItem(pw));

    ui->labelNbPhotos->setText(tr("%n Photo(s)","",ui->listWidgetPhotos->count()));

    return true;
}

void PhotoLister::addImages(const QStringList &fileNames)
{
    int nbPhotos = fileNames.count();

    setProgressDialog(tr("Adding new photos..."), tr("Cancel"), 0, nbPhotos);

#ifdef QT_NO_CONCURRENT
    //if the concurrent programming is not supported, we load/add photos in this thread
    for (int i = 0; i < nbPhotos; i++)
    {
        _progressDialog->setValue(i);//we update the progress dialog's progress bar

        if (_progressDialog->wasCanceled()) break; //adding photos cancelled

        addImage(loadImage(fileNames[i]));

        qApp->processEvents();//to avoid the app to 'freeze'
    }

    _progressDialog->setValue(nbPhotos);

#else
    //concurrent programming is supported, we load/add photos in separate threads for better performances

    _photoAddingWatcher->setFuture(QtConcurrent::mapped(fileNames,&PhotoLister::loadImage));

#endif

}

/////////////////////////////////////////////////////////////////
//utility method called in threads for concurrent programming
/////////////////////////////////////////////////////////////////

PixmapWrapper PhotoLister::loadImage(const QString &fileName)
{
    return PixmapWrapper(fileName);
}

/////////////////////////////////////////////////////////////////
//utility methods on images
/////////////////////////////////////////////////////////////////
void PhotoLister::rotateImageLeft(QListWidgetItem * imageItem)
{
    PixmapListWidgetItem * item = (PixmapListWidgetItem *)imageItem;
    QMatrix matrix; matrix.rotate(90);
    QImage rotatedImage = item->pixmap().toImage().transformed(matrix);

    //updating item icon and image
    item->setIcon(QIcon(QPixmap::fromImage(rotatedImage)));
    item->setPixmap(QPixmap::fromImage(rotatedImage));
}

void PhotoLister::rotateImageRight(QListWidgetItem * imageItem)
{
    PixmapListWidgetItem * item = (PixmapListWidgetItem *)imageItem;
    QMatrix matrix; matrix.rotate(-90);
    QImage rotatedImage = item->pixmap().toImage().transformed(matrix);

    //updating item icon and image
    item->setIcon(QIcon(QPixmap::fromImage(rotatedImage)));
    item->setPixmap(QPixmap::fromImage(rotatedImage));
}

void PhotoLister::setListPhotosEmpty(bool yes)
{
    ui->btnRemovePhoto->setDisabled(yes);
    ui->btnRemovePhotos->setDisabled(yes);
    ui->labelNbPhotos->setDisabled(yes);
}

void PhotoLister::setProgressDialog(const QString & labelText, const QString & cancelButtonText, int minimum, int maximum)
{
    _progressDialog->setLabelText(labelText);
    _progressDialog->setCancelButtonText(cancelButtonText);
    _progressDialog->setRange(minimum,maximum);
}

void PhotoLister::updateCurrentPhoto()
{
    QList<QListWidgetItem *> l = ui->listWidgetPhotos->selectedItems();

    if(l.isEmpty())
        emit currentPhotoChanged(QPixmap());
    else
    {
        PixmapListWidgetItem * item = (PixmapListWidgetItem *) l[0];
        emit currentPhotoChanged(item->pixmap());
    }
}
