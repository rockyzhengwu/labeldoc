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
#include <vector>

#include <opencv2/opencv.hpp>

#include "imageviewer.h"

#include "models/pageitem.h"
#include "models/tesseract_model.h"
#include "models/msertextdetector.h"

class AutoLabelDialog: public QDialog
{
    Q_OBJECT

public:
    explicit AutoLabelDialog(QImage sourceImage, QWidget *parent=nullptr);
    void setSourceImage(QImage sourceImage);
    std::vector<ocrmodel::PageItem> getResult();
    void setSelectArea(QVector<QRectF> areas);

//    ~BinaryDialog();

signals:
    void finishAndSave();

protected slots:
    void windowSizeChange();
    void kChange();
    void toBinary();
    void showBinaryImage();
    void showGrayImage();
    void algorithmChange();
    void startModel();
    void saveLabelResult();
    void quitNotSave();


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
    QVector<QRectF> selectAreas_;
    // bernsen
    QLabel *thresholdLabel_;
    QSpinBox *thresholdSplider_;
    QLabel *contrastLimitLabel_;
    QSpinBox *contrastLimit_;


    // cor model  and tool

    QComboBox *models_;
    QPushButton *modelButton_;

    std::vector<ocrmodel::PageItem> ocrResults_;
    QImage ocrResultImage_;
    ocrmodel::TesseractHandler *tesseractWrap_;
    void createBinaryUI();
    void createModelUI();
    void createUI();
    void showOcrResult();
    void runTesseract();

    ocrmodel::MserTextDetector *mserDetector_;
    void runMser();




    // save
    QPushButton *saveResultButton_;
    QPushButton *cancleResultButton_;


};

#endif // BINARYDIALOG_H
