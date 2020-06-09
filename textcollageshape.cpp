#include "textcollageshape.h"
#include "ui_textcollageshape.h"
#include <QFontDatabase>

TextCollageShape::TextCollageShape(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::TextCollageShape)
{
    ui->setupUi(this);

    //setting the characters widget
    _characterWidget = new CharacterWidget;
    ui->scrollArea->setWidget(_characterWidget);

    //setting the preview text zone
    _graphicsScene = new QGraphicsScene;
    _previewTextItem = _graphicsScene->addText("");

    ui->graphicsViewPreview->setScene(_graphicsScene);

    //setting the display font
    _displayFont.setLetterSpacing(QFont::PercentageSpacing,75);

    //we fill the font list with all available fonts
    ui->listWidgetFonts->addItems(QFontDatabase().families());


    //connections to slots
    connect(ui->listWidgetFontStyle, SIGNAL(currentTextChanged(QString)), SLOT(setDisplayFontStyle(QString)));

    connect(ui->listWidgetFonts, SIGNAL(currentRowChanged(int)), ui->fontComboBox, SLOT(setCurrentIndex(int)));    

    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), SLOT(setDiplayFont(QFont)));

    connect(ui->btnAddCharacter, SIGNAL(clicked()),SLOT(insertSelectedCharacter()));

    connect(this,SIGNAL(displayFontChanged(QFont)),_characterWidget,SLOT(updateFont(QFont)));

    connect(this,SIGNAL(displayFontChanged(QFont)),SLOT(updateFont()));

    connect(ui->lineEditText,SIGNAL(textChanged(QString)),SLOT(updatePreviewText(QString)));

    connect(ui->btnAutoFontSize,SIGNAL(toggled(bool)),SLOT(updateFontSize(bool)));

    connect(ui->spinBoxTextSpacing,SIGNAL(valueChanged(int)),SLOT(updateFontLetterSpacing(int)));

    ui->listWidgetFonts->setCurrentRow(0);
    ui->labelEsitmatedSize->setVisible(false);
}

TextCollageShape::~TextCollageShape()
{
    delete ui;
}

void TextCollageShape::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void TextCollageShape::insertCharacter(const QString &character)
{
    ui->lineEditText->insert(character);
}

void TextCollageShape::insertSelectedCharacter()
{
    ui->lineEditText->insert(_characterWidget->selectedCharacter());
}

void TextCollageShape::setLetterSpacing(int spacing)
{
    if((int)_displayFont.letterSpacing() != spacing)
    {
        _displayFont.setLetterSpacing(QFont::PercentageSpacing,spacing);
        emit displayFontChanged(_displayFont);
    }
}

void TextCollageShape::setDisplayFontStyle(const QString &fontStyle)
{
    QFontDatabase fontdb;
    _displayFont = fontdb.font(_displayFont.family(), fontStyle, _displayFont.pointSize());
    emit displayFontChanged(_displayFont);
}

void TextCollageShape::setDiplayFont(const QFont &font)
{
    _displayFont.setFamily(font.family());
    emit displayFontChanged(_displayFont);
}

void TextCollageShape::updatePreviewText(const QString & text)
{
    _previewTextItem->setPlainText(text);
}

void TextCollageShape::updateFont()
{
    _previewTextItem->setFont(_displayFont);
    ui->lineEditText->setFont(_displayFont);
    ui->graphicsViewPreview->setFont(_displayFont);
}

void TextCollageShape::updateFontSize(bool autoSize)
{
    if(autoSize)
    {
        ui->btnAutoFontSize->setText(tr("Auto"));
        ui->graphicsViewPreview->fitInView(_graphicsScene->sceneRect());
        ui->labelEsitmatedSize->setVisible(false);
    }
    else
    {
        ui->btnAutoFontSize->setText(tr("Manual"));

        QFontDatabase fontdb;
        QFontInfo fontinfo(_displayFont);
        QString styleString = fontdb.styleString(fontinfo);
        _displayFont = fontdb.font(_displayFont.family(), styleString, ui->spinBoxFontSize->value());
        emit displayFontChanged(_displayFont);

    }

}

void TextCollageShape::updateFontLetterSpacing(int spacing)
{
    _displayFont.setLetterSpacing(QFont::PercentageSpacing,(qreal)spacing);
    emit displayFontChanged(_displayFont);
}

QFont TextCollageShape::displayFont() const
{
    return _displayFont;
}

QString TextCollageShape::text() const
{
    return ui->lineEditText->text();
}
