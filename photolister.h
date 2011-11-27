#ifndef PHOTOLISTER_H
#define PHOTOLISTER_H



#include <QMenu>
#include <QPoint>
#include <QWidget>

#include <QDropEvent>
#include <QKeyEvent>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QListWidgetItem>
#include <QProgressDialog>

#include <QFutureWatcher>

#include "pixmaplistwidgetitem.h"

namespace Ui {
    class PhotoLister;
}


/**
  * @class PhotoLister
  * @brief This widget contains and displays a list of photos.
  *
  * The widget allows to add photos from the computer or the web, and
  * provides some basic methods (image rotation, image preview, etc.)
  *
  * @ref PhotoLister is integrated in theQuteCollage's  main window,
  *  but can also be used as a standalone program.
  *
  */
class PhotoLister : public QWidget
{
    Q_OBJECT

public:
    explicit PhotoLister(QWidget *parent = 0);
    ~PhotoLister();

    QList<QPixmap> getPhotos() const;
    QPixmap getCurrentPhoto() const;
    void clearPhotos();


protected:
    //methods for adding photos by Drag & Drop
    void dragEnterEvent ( QDragEnterEvent * event );
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent ( QDropEvent * event );

    //methods for deleteing photos by hitting 'Delete' key
    void keyPressEvent(QKeyEvent * event);

public slots:
    void addPhotosFromComputer();
    void addPhotosFromWeb();
    void addPhotosFinished();

    void removeSelectedPhotos();
    void removeAllPhotos();

    void rotatePhotosLeft();
    void rotatePhotosRight();

    void showContextMenu(const QPoint &pos);

    //called when a thread has finished its job (loading the image)
    void photoLoadedAt(int n);

    void previewPhoto();
    void previewPhoto(QListWidgetItem * item);

    void updateCurrentPhoto();
protected:
    void changeEvent(QEvent* event);
signals:
    void currentPhotoChanged(const QPixmap &);
private:
    //methods for adding images in the list widget
    bool addImage(const PixmapWrapper & pw);
    void addImages(const QStringList & fileNames);

    void setListPhotosEmpty( bool yes);
    void setProgressDialog(const QString & labelText, const QString & cancelButtonText, int minimum, int maximum);

    //utility functions
    static PixmapWrapper loadImage(const QString &fileName);
    static void rotateImageLeft(QListWidgetItem * imageItem);
    static void rotateImageRight(QListWidgetItem * imageItem);

private:
    Ui::PhotoLister *ui;

    //map containing images and their filenames
    QStringList _addedImagesList;

    //list of last images that could not be added/loaded
    QStringList _nonAddedImagesList;

    //watcher that will help monitoring the adding of photos in separate threads
    QFutureWatcher<PixmapWrapper> * _photoAddingWatcher;

    //progress dialog to show progress of some heavy computations
    QProgressDialog * _progressDialog;

    //context menu
    QMenu * _contextMenu;

};

#endif // PHOTOLISTER_H
