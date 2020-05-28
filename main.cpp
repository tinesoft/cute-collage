#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>


#include "textcollageshape.h"
#include "characterwidget.h"
#include "application.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(res);

    Application a(argc, argv);
    /*loading translations*/
    a.loadTranslations(":/translations");
    a.processEvents();

    //QApplication a(argc, argv);

    //TextCollageShape t;
    //t.show();

    //CharacterWidget c;
    //c.show();

    MainWindow w;
    w.showMaximized();

    return a.exec();



}
