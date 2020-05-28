
#include <QtGui>
#include <QToolTip>

#include "characterwidget.h"


CharacterWidget::CharacterWidget(QWidget *parent)
    : QWidget(parent)
{
    _squareSize = 24;
    _columns = 16;
    _lastKey = -1;
    setMouseTracking(true);
}

void CharacterWidget::updateFont(const QFont &font)
{
    _displayFont.setFamily(font.family());
    _squareSize = qMax(24, QFontMetrics(_displayFont).xHeight() * 3);
    adjustSize();
    update();
}

void CharacterWidget::updateSize(const QString &fontSize)
{
    _displayFont.setPointSize(fontSize.toInt());
    _squareSize = qMax(24, QFontMetrics(_displayFont).xHeight() * 3);
    adjustSize();
    update();
}


void CharacterWidget::updateStyle(const QString &fontStyle)
{
    QFontDatabase fontDatabase;
    const QFont::StyleStrategy oldStrategy = _displayFont.styleStrategy();
    _displayFont = fontDatabase.font(_displayFont.family(), fontStyle, _displayFont.pointSize());
    _displayFont.setStyleStrategy(oldStrategy);
    _squareSize = qMax(24, QFontMetrics(_displayFont).xHeight() * 3);
    adjustSize();
    update();
}

void CharacterWidget::updateFontMerging(bool enable)
{
    if (enable)
        _displayFont.setStyleStrategy(QFont::PreferDefault);
    else
        _displayFont.setStyleStrategy(QFont::NoFontMerging);
    adjustSize();
    update();
}


QSize CharacterWidget::sizeHint() const
{
    return QSize(_columns*_squareSize, (65536/_columns)*_squareSize);
}

void CharacterWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint widgetPosition = mapFromGlobal(event->globalPos());
    uint key = (widgetPosition.y()/_squareSize)*_columns + widgetPosition.x()/_squareSize;

    QString text = QString::fromLatin1("<p>Character: <span style=\"font-size: 24pt; font-family: %1\">").arg(_displayFont.family())
                   + QChar(key)
                   + QString::fromLatin1("</span></p>");

    QToolTip::showText(event->globalPos(), text, this);
}

void CharacterWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _lastKey = (event->y()/_squareSize)*_columns + event->x()/_squareSize;
        if (QChar(_lastKey).category() != 0 )
            emit characterSelected(QString(QChar(_lastKey)));
        update();
    }
    else
        QWidget::mousePressEvent(event);
}

void CharacterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(Qt::white));
    painter.setFont(_displayFont);

    QRect redrawRect = event->rect();
    int beginRow = redrawRect.top()/_squareSize;
    int endRow = redrawRect.bottom()/_squareSize;
    int beginColumn = redrawRect.left()/_squareSize;
    int endColumn = redrawRect.right()/_squareSize;

    painter.setPen(QPen(Qt::gray));
    for (int row = beginRow; row <= endRow; ++row) {
        for (int column = beginColumn; column <= endColumn; ++column) {
            painter.drawRect(column*_squareSize, row*_squareSize, _squareSize, _squareSize);
        }

        QFontMetrics fontMetrics(_displayFont);
        painter.setPen(QPen(Qt::black));
        for (int row = beginRow; row <= endRow; ++row) {

            for (int column = beginColumn; column <= endColumn; ++column) {

                int key = row*_columns + column;
                painter.setClipRect(column*_squareSize, row*_squareSize, _squareSize, _squareSize);

                if (key == _lastKey)
                    painter.fillRect(column*_squareSize + 1, row*_squareSize + 1, _squareSize, _squareSize, QBrush(Qt::red));

                painter.drawText(column*_squareSize + (_squareSize / 2) - fontMetrics.width(QChar(key))/2,
                                 row*_squareSize + 4 + fontMetrics.ascent(),
                                 QString(QChar(key)));
            }
        }
    }
}

