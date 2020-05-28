#ifndef TEXTCOLLAGESHAPE_H
#define TEXTCOLLAGESHAPE_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include "characterwidget.h"

namespace Ui {
    class TextCollageShape;
}

/**
  * @class TextCollageShape
  * @brief This dialog allows to enter the text of the collage.
  *
  * The dialogs allows to enter the text of the collage, and
  * the font that must be used.
  *
  */
class TextCollageShape : public QDialog
{
    Q_OBJECT

public:
    explicit TextCollageShape(QWidget *parent = 0);
    ~TextCollageShape();
    QFont displayFont() const;
    QString text()      const;
protected:
    void changeEvent(QEvent* event);
signals:
    void displayFontChanged(const QFont &font);
public slots:
    void insertCharacter(const QString &character);
    void insertSelectedCharacter();
    void setDisplayFontStyle(const QString &fontStyle);
    void setLetterSpacing(int spacing);
    void setDiplayFont(const QFont &font);
    void updatePreviewText(const QString &text);
    void updateFontSize(bool autoSize);
    void updateFontLetterSpacing(int spacing);
    void updateFont();

private:
    Ui::TextCollageShape *ui;
    CharacterWidget *_characterWidget;
    QGraphicsScene  *_graphicsScene;
    QGraphicsTextItem * _previewTextItem;
    QFont _displayFont;
};

#endif // TEXTCOLLAGESHAPE_H
