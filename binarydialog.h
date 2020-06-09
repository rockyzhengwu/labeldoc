#ifndef BINARYDIALOG_H
#define BINARYDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QImage>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QPushButton>
#include <QDoubleSpinBox>

#include <opencv2/opencv.hpp>

#include "imageviewer.h"

class BinaryDialog: public QDialog
{
    Q_OBJECT

public:
    explicit BinaryDialog(QImage sourceImage, QWidget *parent=nullptr);

signals:

protected slots:
    void windowSizeChange();
    void kChange();
    void toBinary();
    void showBinaryImage();
    void showGrayImage();
    void algorithmChange();

private:
    QHBoxLayout *mainLayout_ ;
    QVBoxLayout *rightLayout_;
    QDockWidget *rightDockWidget_;

    ImageViewer *imageViewer_;
    QComboBox *algorithm_;

    QLabel *windowSizeLabel_;
    QSlider *windowSizeSlider_;

    QLabel *kLabel_;
    QDoubleSpinBox *kSpinBox_;

    QPushButton *binaryButton_;
    QPushButton *grayButton_;

    QImage sourceImage_;
    QImage binaryImage_;
    // bernsen
    QLabel *thresholdLabel_;
    QSpinBox *thresholdSplider_;
    QLabel *contrastLimitLabel_;
    QSpinBox *contrastLimit_;

    void createBinaryUI();

};

#endif // BINARYDIALOG_H
