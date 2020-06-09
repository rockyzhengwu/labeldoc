#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPointF>
#include <QPixmap>

class ImageViewer: public QWidget
{
public:
    ImageViewer(QImage image, QWidget *parent=nullptr);
    double adjustWindowScale();
    QPointF offsetToCenter();
    void resetImage(QImage image);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPixmap image_;
    QPainter *painter_ ;
    double scale_;
};

#endif // IMAGEVIEWER_H
