#include "binarydialog.h"
#include "imagebinary/binary.h"
#include "imageutil.h"
#include <QGroupBox>
#include <QDebug>

BinaryDialog::BinaryDialog(QImage image, QWidget *parent) :
    QDialog (parent),
    sourceImage_(image)
{

    qDebug() << "imge format: " << sourceImage_.format();

     setWindowState(Qt::WindowMaximized);
     mainLayout_= new QHBoxLayout();
     imageViewer_ = new ImageViewer(sourceImage_);
     imageViewer_->setMinimumSize(800,800);

     rightLayout_ = new QVBoxLayout();
     grayButton_ = new QPushButton("To Gray");
     createBinaryUI();

     mainLayout_->addWidget(imageViewer_);
     mainLayout_->addLayout(rightLayout_);
     mainLayout_->setStretch(0, 4);
     mainLayout_->setStretch(1, 1);
     connect(grayButton_, SIGNAL(clicked()), this, SLOT(showGrayImage()));


}
void BinaryDialog::windowSizeChange(){
    QString windowSizeValue = QString::number(windowSizeSlider_->value());
    windowSizeLabel_->setText("Window Size: " + windowSizeValue);
//    update();
    windowSizeLabel_->update();
    // TODO use
//    toBinary();
}

void BinaryDialog::kChange(){
     // TODO use
//    toBinary();
}

void BinaryDialog::createBinaryUI(){
      QGroupBox *binaryGroupBox = new QGroupBox(tr("Binary Algorithm"));
      QVBoxLayout *binaryLayout = new QVBoxLayout();
      algorithm_ = new QComboBox();
      algorithm_->addItem("Sauvola");
      algorithm_->addItem("OTSU");
      algorithm_->addItem("NiBlack");
      algorithm_->addItem("Nick");
      algorithm_->addItem("Bernsen");
      algorithm_->addItem("Wolf");
      windowSizeSlider_ = new QSlider(Qt::Orientation::Horizontal);
      windowSizeSlider_->setRange(1, 500);
      QString windowSize = QString::number(windowSizeSlider_->value());
      qDebug()<< "windowSize" << windowSize;
      windowSizeLabel_ =new QLabel("Window Size: " + windowSize);
      kSpinBox_ = new QDoubleSpinBox();
      kSpinBox_->setRange(-2.0, 2.0);
      kSpinBox_->setSingleStep(0.01);
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


void BinaryDialog::algorithmChange(){
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

    } else if(algorithmName=="NiBlack" || algorithmName=="Nick" || algorithmName=="Sauvola" || algorithmName=="Wolf"){
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


void BinaryDialog::toBinary(){
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
   cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/gray_image.jpg", grayImage);
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
   }
   cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/binary_image.jpg", binarycv);
   binaryImage_ = cvMatToQImage(binarycv);
   showBinaryImage();
}


void BinaryDialog::showBinaryImage(){
    imageViewer_->resetImage(binaryImage_);
}


void BinaryDialog::showGrayImage(){
    cv::Mat cvImage = QImageToCvMat(sourceImage_);
    cv::Mat grayImage;
    cvtColor( cvImage, grayImage, cv::COLOR_BGR2GRAY );
    QImage bim = cvMatToQImage(grayImage)    ;
    imageViewer_->resetImage(bim);
}
