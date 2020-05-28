#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

/**
  * @class MainWindow
  * @brief This main application window
  *
  * The window is the main one. It contains the @ref PhotoLister, @ref CollageCreator,
  * and the other elements of the interfaces ( menus, statusbar, etc.)
  *
  */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    ///Gets the collage parameters
    bool getCollageParameters();
    ///Gets the photos to use for the collage
    QList<QPixmap> getPhotos() const;
protected:
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent * event);
public slots:
    ///Adds photos from the computer
    void addPhotosFromComputer();
    ///Adds photos from the the web
    void addPhotosFromWeb();
    ///Creates the collage (image)
    void createCollage();
    ///Save the collage as (image)
    void saveCollage();
    ///prints the collage
    void printCollage();

    ///Opens the text collage form dialog
    void openTextCollageShape();
    ///Opens the custom shape editor
    void openCustomShapeEditor();
    ///Opens a dialog for choosing the collage background image
    void openCollageBackgroundPhoto();

    void newCollageProject();
    void about();
    void help();

    void chooseCollageBorderColor();
    void chooseCollageBackgroundColor();
    void updateCollageShape(int shape);
    void updateCollageBackground(int background);

    void updateAutoCollageSize(bool autoOn);
    void updateAutoPhotoSize(bool autoOn);
    void updateAutoNbPhotos(bool autoOn);
    void updateAutoInterPhotoDistance(bool autoOn);

    void updatePhotoPreview();

    void updateStyle(QAction *a);
    void updateLanguage(QAction *a);


    ///Resets the collage parameters
    void resetCollageParameters();

private:
    Ui::MainWindow *ui;
    QString languageName(const QString &locale);

};

#endif // MAINWINDOW_H
