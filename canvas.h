#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QPoint>
#include <QMouseEvent>
#include <QVector>
#include <QKeyEvent>
#include <QSize>
#include <QJsonArray>
#include <QPair>

#include "shape.h"

class Canvas: public QWidget
{
    Q_OBJECT

public:
    enum Mode{NOMODE, EDIT, CREATE};
    Canvas(QWidget *parent=nullptr);
    void setPixImage(QPixmap pixImage);
    bool hasSelected();
    void deleteSelectedShape();
    QJsonArray getShapeAsJson();
    void restoreShapeFromJson(QJsonArray jsonArray);
    bool isNeedSave();
    void resetState();
    int shapeSize();
    void addShape(Shape *shape);
    QVector<Shape *> getSelectedShape();


protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool outOfPixmap(QPointF point);
    void keyPressEvent(QKeyEvent *event);
    QPointF intersectionPoint(QPointF p1, QPointF p2);
    QVector<QVector<double>> intersectingEdges(QPointF p1, QPointF p2, QVector<QPointF>);
    QPointF canBoundShapePoint(Shape *shape, QPointF currentPoint);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

signals:
    void addNewShape(Shape *shape);

protected slots:

private:
    QPixmap pixmap_;
    QPainter *painter_;
    Shape *currentShape_ =nullptr;
    Shape *showShape_ = nullptr;
    QPointF drawStartPoint_ = QPointF{};
    QPointF drawEndPoint_ = QPointF{};

    QVector<Shape*> shapes_;

    QPointF offsetToCenter();
    QPointF transformPointToImage(const QPointF point);


    QPointF preMovingPrePointF = QPointF{};
//    QVector<Shape*> selectedShape_ ;
    Shape *hShape_=nullptr;
    Shape *preShape = nullptr;
    QPointF hVertex ;
    int selectPointIndex_ ;
    bool hasEdit_ ;
};

#endif // CANVAS_H
