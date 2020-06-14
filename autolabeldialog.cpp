#include "autolabeldialog.h"
#include "imagebinary/binary.h"
#include "models/tesseract_model.h"
#include "models/pageitem.h"
#include "imageutil.h"

#include <QGroupBox>
#include <QDebug>
#include <vector>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

AutoLabelDialog::AutoLabelDialog(QImage image, QWidget *parent) :
    QDialog (parent),
    sourceImage_(image)
{

   setWindowState(Qt::WindowMaximized);
   createUI();
}

//BinaryDialog::~BinaryDialog(){
//    if(tesseractWrap_)   {
//        tesseractWrap_->close();
//    }
//   QDialog::~QDialog();
//}

void AutoLabelDialog::setSourceImage(QImage sourceImage){
    sourceImage_= sourceImage;
    imageViewer_->resetImage(sourceImage);
}


void AutoLabelDialog::createUI(){
     mainLayout_= new QHBoxLayout();
     imageViewer_ = new ImageViewer(sourceImage_);
     imageViewer_->setMinimumSize(800,800);

     rightLayout_ = new QVBoxLayout();

     grayButton_ = new QPushButton("To Gray");

     createBinaryUI();
     createModelUI();

     saveResultButton_ = new QPushButton("Save Result");
     cancleResultButton_ = new QPushButton("Cancle");
     QHBoxLayout *butLayout = new QHBoxLayout();
     butLayout->addWidget(saveResultButton_);
     butLayout->addWidget(cancleResultButton_);
     rightLayout_->addLayout(butLayout);

     mainLayout_->addWidget(imageViewer_);
     mainLayout_->addLayout(rightLayout_);
     mainLayout_->setStretch(0, 4);
     mainLayout_->setStretch(1, 1);
     connect(saveResultButton_, SIGNAL(clicked()), this, SLOT(saveLabelResult()));
     connect(cancleResultButton_, SIGNAL(clicked()), this, SLOT(quitNotSave()));

     connect(grayButton_, SIGNAL(clicked()), this, SLOT(showGrayImage()));
}

void AutoLabelDialog::createModelUI(){

    QGroupBox *modelGroupBox = new QGroupBox(tr("OCR Models"));
    QVBoxLayout *modelLayout = new QVBoxLayout();

    models_ = new QComboBox();
    models_->addItem("tesseract");
    modelButton_ = new QPushButton("Run Model");
    modelLayout->addWidget(models_);
    modelLayout->addWidget(modelButton_);
    modelGroupBox->setLayout(modelLayout);
    rightLayout_->addWidget(modelGroupBox);

    connect(modelButton_, SIGNAL(clicked()), this, SLOT(startModel()));
}

void AutoLabelDialog::startModel(){
    QString modelName = models_->currentText();
    if(modelName=="tesseract"){
        if (tesseractWrap_==nullptr){
            tesseractWrap_ = new ocrmodel::TesseractHandler();
        }

        qDebug() << "start tesseract";
        if(binaryImage_.isNull()){
            qDebug() << "analysis source file";
            tesseractWrap_->tesseract_analysis(QImageToCvMat(sourceImage_));
            return;
        }
        else {
            cv::Mat bimg = QImageToCvMat(binaryImage_);
            qDebug() << "start tessearct binary model" << bimg.size().width << " height "<< bimg.size().height << " binary" <<binaryImage_.format();
            ocrResults_ = tesseractWrap_->tesseract_analysis(bimg.clone());
            showOcrResult();
        }
    }
}




void AutoLabelDialog::windowSizeChange(){
    QString windowSizeValue = QString::number(windowSizeSlider_->value());
    windowSizeLabel_->setText("Window Size: " + windowSizeValue);
//    update();
    windowSizeLabel_->update();
    // TODO use
//    toBinary();
}

void AutoLabelDialog::kChange(){
     // TODO use
//    toBinary();
}

void AutoLabelDialog::createBinaryUI(){
      QGroupBox *binaryGroupBox = new QGroupBox(tr("Binary Algorithm"));
      QVBoxLayout *binaryLayout = new QVBoxLayout();
      algorithm_ = new QComboBox();
      algorithm_->addItem("Sauvola");
      algorithm_->addItem("OTSU");
      algorithm_->addItem("NiBlack");
      algorithm_->addItem("Nick");
      algorithm_->addItem("Bernsen");
      algorithm_->addItem("Wolf");
      algorithm_->addItem("ISauvola");
      algorithm_->addItem("Wan");
      algorithm_->addItem("TRSingh");

      windowSizeSlider_ = new QSlider(Qt::Orientation::Horizontal);
      windowSizeSlider_->setRange(1, 500);
      windowSizeSlider_->setValue(75);
      QString windowSize = QString::number(windowSizeSlider_->value());
      qDebug()<< "windowSize" << windowSize;
      windowSizeLabel_ =new QLabel("Window Size: " + windowSize);
      kSpinBox_ = new QDoubleSpinBox();
      kSpinBox_->setRange(-2.0, 2.0);
      kSpinBox_->setSingleStep(0.01);
      kSpinBox_->setValue(0.25);

      kLabel_ = new QLabel("K: ") ;
      binaryButton_ = new QPushButton("To Binary ");

     rightLayout_ ->addWidget(grayButton_);
     rightLayout_ ->setMargin(50);

     thresholdSplider_ = new QSpinBox();
     thresholdSplider_->setRange(0, 255);
     thresholdLabel_ = new QLabel("threshold");

     contrastLimitLabel_ = new QLabel("Contrast Limit:");
     contrastLimit_ =new QSpinBox();
     contrastLimit_->setRange(0, 255);

     thresholdSplider_->setVisible(false);
     thresholdLabel_->setVisible(false);
     contrastLimitLabel_->setVisible(false);
     contrastLimit_->setVisible(false);


     binaryLayout->addWidget(algorithm_);
     binaryLayout->addWidget(windowSizeLabel_);
     binaryLayout->addWidget(windowSizeSlider_);
     binaryLayout->addWidget(kLabel_);
     binaryLayout->addWidget(kSpinBox_);
     binaryLayout->addWidget(thresholdLabel_);
     binaryLayout->addWidget(thresholdSplider_);
     binaryLayout->addWidget(contrastLimitLabel_);
     binaryLayout->addWidget(contrastLimit_);

     binaryLayout->addWidget(binaryButton_);
     binaryGroupBox->setLayout(binaryLayout);

     rightLayout_->addWidget(binaryGroupBox);
     rightLayout_->setAlignment(Qt::AlignTop);

     connect(algorithm_, SIGNAL(currentIndexChanged(int)) ,this, SLOT(algorithmChange()));
     connect(binaryButton_, SIGNAL(clicked()), this, SLOT(toBinary()));
     setLayout(mainLayout_);
     connect(kSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(kChange()));
     connect(windowSizeSlider_, SIGNAL(valueChanged(int)), this, SLOT(windowSizeChange()));
}


void AutoLabelDialog::algorithmChange(){
    QString algorithmName = algorithm_->currentText();
    qDebug() <<"change algorithm name: " << algorithmName;
    if(algorithmName=="OTSU"){
        windowSizeLabel_->setVisible(false);
        windowSizeSlider_->setVisible(false);
        kLabel_->setVisible(false);
        kSpinBox_->setVisible(false);
        thresholdSplider_->setVisible(false);
        thresholdLabel_->setVisible(false);
        contrastLimitLabel_->setVisible(false);
        contrastLimit_->setVisible(false);

    } else if(algorithmName=="NiBlack" || algorithmName=="Nick" || algorithmName=="Sauvola"
              || algorithmName=="Wolf" || algorithmName=="ISauvola" || algorithmName=="Wan" || algorithmName=="TRSingh"){
        windowSizeLabel_->setVisible(true);
        windowSizeSlider_->setVisible(true);
        kLabel_->setVisible(true);
        kSpinBox_->setVisible(true);

        thresholdSplider_->setVisible(false);
        thresholdLabel_->setVisible(false);
        contrastLimitLabel_->setVisible(false);
        contrastLimit_->setVisible(false);
    } else if(algorithmName == "Bernsen"){
        kLabel_->setVisible(false);
        kSpinBox_->setVisible(false);

        windowSizeLabel_->setVisible(true);
        windowSizeSlider_->setVisible(true);


        thresholdSplider_->setVisible(true);
        thresholdLabel_->setVisible(true);
        contrastLimitLabel_->setVisible(true);
        contrastLimit_->setVisible(true);
    }else{

    }

}


void AutoLabelDialog::toBinary(){
   cv::Mat cvImage = QImageToCvMat(sourceImage_, true);
   cv::Mat grayImage;
   cvtColor( cvImage, grayImage, cv::COLOR_BGR2GRAY );
   QString algorithmName = algorithm_->currentText();
   cv::Mat binarycv = cv::Mat(cvImage.size().height, cvImage.size().width, CV_8U, cv::Scalar(0));

    int ws = windowSizeSlider_->value();
    if (ws == 0 && algorithmName!="OTSU"){
        qDebug() << "windowSize == 0";
        return;
    }

//   cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/gray_image.jpg", grayImage);
   if(algorithmName=="OTSU"){
       imb::otsu(grayImage, binarycv);
   }
   else if(algorithmName=="NiBlack"){
       imb::niblack(grayImage, binarycv, cv::Size(ws,ws), kSpinBox_->value());
   }
   else if(algorithmName=="Sauvola"){
       imb::sauvola(grayImage, binarycv, cv::Size(ws,ws), kSpinBox_->value());
   }
   else if(algorithmName=="Nick"){
       imb::nick(grayImage, binarycv, cv::Size(ws, ws), kSpinBox_->value());
   } else if(algorithmName=="Bernsen"){
       imb::bernsen(grayImage, binarycv, cv::Size(ws, ws), thresholdSplider_->value(), contrastLimit_->value());
   } else if(algorithmName=="Wolf"){
       qDebug() << "use wolf";
       imb::wolf(grayImage, binarycv, cv::Size(ws, ws), kSpinBox_->value());
   } else if(algorithmName=="ISauvola"){
        binarycv = cv::Mat(cvImage.size().height, cvImage.size().width, CV_8U, cv::Scalar(255));
       imb::isauvola(grayImage, binarycv, cv::Size(ws, ws), kSpinBox_->value());
   } else if(algorithmName=="Wan") {
       imb::wan(grayImage, binarycv, cv::Size(ws, ws), kSpinBox_->value());
   } else if(algorithmName=="TRSingh"){
       imb::trsingh(grayImage, binarycv, cv::Size(ws,ws),kSpinBox_->value());
   }
   cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/binary_image.jpg", binarycv);
   binaryImage_ = cvMatToQImage(binarycv);
   showBinaryImage();
}


void AutoLabelDialog::showBinaryImage(){
    imageViewer_->resetImage(binaryImage_);
}

void AutoLabelDialog::showOcrResult(){
    qDebug() << "show ocr reslt " << binaryImage_.size();;
    cv::Mat cvImage = QImageToCvMat(sourceImage_);
    srand((unsigned)time(nullptr));

    for(ocrmodel::PageItem item:ocrResults_){
        std::vector<cv::Point> cvPoints;
        std::vector<ocrmodel::Point> points = item.getPoints();
        ocrmodel::Point p1= points[0];
        ocrmodel::Point p2 = points[1];
//        for(ocrmodel::Point p: points){
//           cvPoints.push_back(cv::Point(p.x, p.y));
//        }
        cv::Scalar color(std::rand() % 256, std::rand()% 256, std::rand()%256);
        cv::Rect rect(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
        std::vector<std::vector<cv::Point>> c;
        cv::rectangle(cvImage, rect, color , 2);
//        c.push_back(cvPoints);
//        cv::drawContours(cvImage, c, 0, color, 2);
    }
    imageViewer_->resetImage(cvMatToQImage(cvImage));
}

void AutoLabelDialog::showGrayImage(){
    cv::Mat cvImage = QImageToCvMat(sourceImage_, true);
    cv::Mat grayImage;
    cvtColor( cvImage, grayImage, cv::COLOR_BGR2GRAY );
    QImage bim = cvMatToQImage(grayImage)    ;
    imageViewer_->resetImage(bim);
}

void AutoLabelDialog::saveLabelResult(){
    close();
    emit finishAndSave();
}

void AutoLabelDialog::quitNotSave(){
   close();
}

std::vector<ocrmodel::PageItem> AutoLabelDialog::getResult(){
    return ocrResults_;
}
