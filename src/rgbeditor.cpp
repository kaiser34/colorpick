#include "rgbeditor.h"

#include "colorspace.h"
#include "imagegradientselector.h"

#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QSpinBox>

RgbEditor::RgbEditor(ColorSpace *colorSpace, QWidget *parent)
    : QWidget(parent)
    , mColorSpace(colorSpace)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(QMargins());

    QString labelTexts[3] = {
        mColorSpace->name(0),
        mColorSpace->name(1),
        mColorSpace->name(2)
    };
    for (int row = 0; row < 3; ++row) {
        QLabel *label = new QLabel(labelTexts[row]);

        int max = mColorSpace->maximum(row);

        ImageGradientSelector *selector = new ImageGradientSelector();
        selector->setRange(0, max);
        selector->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        selector->setArrowDirection(Qt::UpArrow);

        QSpinBox *spinBox = new QSpinBox();
        spinBox->setRange(0, max);

        layout->addWidget(label, row, 0);
        layout->addWidget(selector, row, 1);
        layout->addWidget(spinBox, row, 2);

        connect(selector, &KGradientSelector::valueChanged, this, &RgbEditor::updateFromSelectors);
        connect(selector, &KGradientSelector::valueChanged, spinBox, &QSpinBox::setValue);
        connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), selector, &KGradientSelector::setValue);

        mComponentSelectors[row] = selector;
        mComponentSpinBoxes[row] = spinBox;
    }

    updateSelectorGradients();
}

QColor RgbEditor::color() const
{
    QVector<int> values(3);
    for (int row = 0; row < 3; ++row) {
        values[row] = mComponentSelectors[row]->value();
    }
    return mColorSpace->fromValues(values);
}

void RgbEditor::setColor(const QColor &newColor)
{
    if (color() != newColor) {
        for (int row = 0; row < 3; ++row) {
            int value = mColorSpace->value(newColor, row);
            mComponentSelectors[row]->blockSignals(true);
            mComponentSpinBoxes[row]->blockSignals(true);
            mComponentSelectors[row]->setValue(value);
            mComponentSpinBoxes[row]->setValue(value);
            mComponentSelectors[row]->blockSignals(false);
            mComponentSpinBoxes[row]->blockSignals(false);
        }
        updateSelectorGradients();
        colorChanged(newColor);
    }
}

QImage RgbEditor::createGradientImage(int idx) const
{
    QVector<int> values = mColorSpace->values(color());

    int max = mColorSpace->maximum(idx);
    QImage image(max, 1, QImage::Format_ARGB32);
    QRgb* ptr = reinterpret_cast<QRgb*>(image.scanLine(0));
    for (int x = 0; x <= max; ++x, ++ptr) {
        values[idx] = x;
        *ptr = mColorSpace->fromValues(values).rgb();
    }
    return image;
}

void RgbEditor::updateSelectorGradients()
{
    for (int row = 0; row < 3; ++row) {
        QImage image = createGradientImage(row);
        mComponentSelectors[row]->setImage(image);
    }
}

void RgbEditor::updateFromSelectors()
{
    updateSelectorGradients();
    colorChanged(color());
}
