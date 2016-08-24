#include "coloreditor.h"

#include <KColorUtils>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QToolButton>

ColorEditor::ColorEditor(QWidget *parent) : QWidget(parent)
{
    mColorButton = new QPushButton();
    mLineEdit = new QLineEdit();
    connect(mLineEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
        if (QColor::isValidColor(text)) {
            setColor(QColor(text));
        }
    });

    QToolButton *darkerButton = createValueButton(-10);
    QToolButton *lighterButton = createValueButton(10);

    QToolButton *pickerButton = new QToolButton();
    pickerButton->setIcon(QIcon::fromTheme("color-picker"));
    pickerButton->setFocusPolicy(Qt::TabFocus);
    connect(pickerButton, &QToolButton::clicked, this, &ColorEditor::startPicking);
    mLuminanceLabel = new QLabel();

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(mColorButton, 0, 0);
    layout->addWidget(mLineEdit, 0, 1);
    layout->addWidget(darkerButton, 0, 2);
    layout->addWidget(lighterButton, 0, 3);
    layout->addWidget(pickerButton, 0, 4);

    layout->addWidget(mLuminanceLabel, 2, 0, 1, 6);
}

void ColorEditor::setColor(const QColor &color)
{
    if (mColor != color) {
        mColor = color;
        updateFromColor();
        colorChanged(mColor);
    }
}

void ColorEditor::updateFromColor()
{
    QPalette pal = mColorButton->palette();
    pal.setColor(QPalette::Button, mColor);
    mColorButton->setPalette(pal);

    if (!mLineEdit->hasFocus()) {
        mLineEdit->setText(mColor.name());
    }

    qreal luminance = KColorUtils::luma(mColor);
    QString lumaText = tr("Luminance: %1").arg(QLocale::system().toString(luminance, 'f', 3));
    mLuminanceLabel->setText(lumaText);
}

void ColorEditor::adjustValue(int delta)
{
    int h, s, v, a;
    mColor.getHsv(&h, &s, &v, &a);
    v = qBound(0, v + delta, 255);
    setColor(QColor::fromHsv(h, s, v, a));
}

void ColorEditor::startPicking()
{

}

QToolButton *ColorEditor::createValueButton(int delta)
{
    QToolButton *button = new QToolButton();
    button->setFocusPolicy(Qt::TabFocus);
    button->setArrowType(delta < 0 ? Qt::DownArrow : Qt::UpArrow);
    connect(button, &QToolButton::clicked, this, [this, delta]() {
        adjustValue(delta);
    });
    return button;
}
