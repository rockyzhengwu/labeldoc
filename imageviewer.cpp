#include "imageviewer.h"

#include <QPixmap>

#include <QDebug>

ImageViewer::ImageViewer(QImage image, QWidget *parent):QWidget (parent),
    image_(QPixmap::fromImage(image))
{
   painter_ = new QPainter();
   scale_ = adjustWindowScale();
   qDebug() << "viewer" << scale_;
}

void ImageViewer::paintEvent(QPaintEvent *event){
    scale_ = adjustWindowScale();
    painter_->begin(this);
    painter_->setRenderHint(QPainter::Antialiasing);
    painter_->setRenderHint(QPainter::HighQualityAntialiasing);
    painter_->setRenderHint(QPainter::SmoothPixmapTransform);

    painter_->scale(scale_, scale_);
    painter_->translate(offsetToCenter());
    painter_->drawPixmap(0, 0, image_);
    painter_->end();
//    QWidget::paintEvent(event);
}


// 能把图像放到中间的顶点
QPointF ImageViewer::offsetToCenter(){
    double width = image_.width() * scale_;
    double height = image_.height() * scale_;
    int areaWidth = size().width();
    int areaHeight = size().height();
    qreal offsetWidth = 0;
    qreal offsetHeight = 0;
    if (width < areaWidth){
        offsetWidth = qreal((areaWidth - width) / (2*scale_));
    }
    if (height < areaHeight){
        offsetHeight = qreal((areaHeight - height) / (2*scale_));
    }
    return QPointF(offsetWidth, offsetHeight);
}



double ImageViewer::adjustWindowScale(){
    int windowWidth = size().width();
    int windowHeight = size().height();
    int imageWidth = image_.width();
    int imageHeight = image_.height();
    double rw = 1.0 * windowWidth / windowHeight;
    double ri = 1.0 * imageWidth / imageHeight;
    if (rw >  ri){
        return 1.0* windowHeight/imageHeight;
    }
    return 1.0 * windowWidth / imageWidth;
}

void ImageViewer::resetImage(QImage image){
    image_ = QPixmap::fromImage(image);
    repaint();
}

