#include "canvas.h"
#include "statestorage.h"

#include <QPoint>
#include <QDebug>
#include <QSize>

#include <algorithm>
#include <cmath>

Canvas::Canvas(QWidget *parent):QWidget (parent),
    shapes_()
{
  painter_ = new QPainter();
  setFocusPolicy(Qt::WheelFocus);
  setMouseTracking(true);
  hasEdit_ = false;
}

void Canvas::setPixImage(QPixmap pixImage){
    pixmap_ = pixImage;
    shapes_.clear();
    repaint();
    qDebug() << "after pix Image";
}

void Canvas::paintEvent(QPaintEvent *event){
    painter_->begin(this);
    painter_->setRenderHint(QPainter::Antialiasing);
    painter_->setRenderHint(QPainter::HighQualityAntialiasing);
    painter_->setRenderHint(QPainter::SmoothPixmapTransform);
    painter_->scale(StateStorage::scale, StateStorage::scale);
    painter_->translate(offsetToCenter());
    painter_->drawPixmap(0, 0, pixmap_);
    if (!shapes_.empty()){
        for(int i=0; i<shapes_.size(); i++){
            shapes_[i]->paint(painter_);
        }
    }
    if(currentShape_!=nullptr){
        currentShape_->paint(painter_);
    }
    painter_->end();
//    QWidget::paintEvent(event);
}

// 能把图像放到中间的顶点
QPointF Canvas::offsetToCenter(){
    double width = pixmap_.width() * StateStorage::scale;
    double height = pixmap_.height() * StateStorage::scale;
    int areaWidth = size().width();
    int areaHeight = size().height();
    qreal offsetWidth = 0;
    qreal offsetHeight = 0;
    if (width < areaWidth){
        offsetWidth = qreal((areaWidth - width) / (2*StateStorage::scale));
    }
    if (height < areaHeight){
        offsetHeight = qreal((areaHeight - height) / (2*StateStorage::scale));
    }

    return QPointF(offsetWidth, offsetHeight);
}


void Canvas::mousePressEvent(QMouseEvent* event){
    QPointF pos = transformPointToImage(event->localPos());

    if(event->button() & Qt::LeftButton){
        // 如果是create 模式
        if(StateStorage::canvasMode == Canvas::CREATE){
             hasEdit_ = true;
             if (nullptr == currentShape_ ){
                 if(!outOfPixmap(pos)){
                   currentShape_ = new Shape(StateStorage::labelName, StateStorage::drawingShapeType);
                   currentShape_->setShapeState(Shape::Drawing);
                   currentShape_->addPoint(pos);
                   if(currentShape_->isClosed()){
                       currentShape_->setShapeState(Shape::NoState);
                       shapes_.append(currentShape_);
                       currentShape_ = nullptr;
                   }
                 }
             }
             else{
                 if (outOfPixmap(pos)){
                     pos = intersectionPoint(currentShape_->getLastPoint(), pos);
                 }
                 currentShape_->addPoint(pos);
                 // todo 所有 新建 shape 的地方
                 if(currentShape_->isClosed()){
                     currentShape_->setShapeState(Shape::NoState);
                     shapes_.setSharable(Shape::NoState);

                     shapes_.append(currentShape_);
                     currentShape_=nullptr;
                 }
             }
             update();
        }
        // 编辑模式
        else {
            for(Shape *shape: shapes_){
                if(shape->getShapeState()==Shape::Selected && !shape->containsPoint(pos)){
                    shape->setShapeState(Shape::NoState);
                }
                if(shape->getShapeState()==Shape::Hover ){
                    if(shape->containsPoint(pos)|| shape->hasSelectedVertex()){
                      hasEdit_ = true;
                      shape->setShapeState(Shape::Selected);
                      shape->startMove(pos);
                    }else{
                        shape->setShapeState(Shape::NoState);
                    }
                }
            }
       }
    }
}


void Canvas::mouseMoveEvent(QMouseEvent *event){
   QPointF pos = transformPointToImage(event->localPos());
   // CREATE 模式下绘制 标注的过程
   if (StateStorage::canvasMode == Canvas::CREATE){
       if (nullptr==currentShape_){
           return;
       }
       else{
           if(outOfPixmap(pos)){
               QPointF lastPont = currentShape_->getLastPoint();
               if (!lastPont.isNull()){
                 pos = intersectionPoint(currentShape_->getLastPoint() ,pos);
               }
           }
           currentShape_->setDrawingPoint(pos);
       }
     update();
   }
   // 编辑模式下
   else if (StateStorage::canvasMode == Canvas::EDIT){
       // 按住左键拖动
       if(event->button() & Qt::LeftButton){
           for (Shape *shape: shapes_){
             if(shape->isSelected() || shape->hasSelectedVertex()){
                  shape->setShapeState(Shape::Moving);
              }
             if(shape->isMoving()){
                 if(shape->hasSelectedVertex()){
                     qDebug() <<"move by vertex";
                     if(outOfPixmap(pos)){
                       const QPointF * sp = shape->getSelectPoint();
                       pos = intersectionPoint(*sp, pos);
                     }
                     shape->moveSelectedVertex(pos);
                 }
                 else{
                     qDebug() << "move whole ";
                    if(!outOfPixmap(pos)){
                      QPointF boundP = canBoundShapePoint(shape, pos);
                      shape->currentMove(boundP);
                    }
                 }
             }
//             shape->setShapeState(Shape::Selected);
           }
           update();
       }
       // hover 设置 shape 和顶点是否被选中
       else {
           for(Shape *shape: shapes_){
//               shape->clearSelectVertex();
               int nearIndex = shape->getNearestVertexIndex(pos, StateStorage::closeEpsilon / StateStorage::scale);
               if (nearIndex>= 0){
                   shape->setShapeState(Shape::Hover);
               }
               if(shape->isSelected()){
                   continue;
               }
               if (shape->containsPoint(pos)){
                  shape->setShapeState(Shape::Hover);
               }else{
                   shape->setShapeState(Shape::NoState);
               }
           }
           update();
       }
   }
}

QPointF Canvas::canBoundShapePoint(Shape *shape, QPointF currentPoint){
    QRectF rect = shape->boundingRect();
    QPointF startPoint = shape->getStartMovePoint();
    double x1 = rect.x() - startPoint.x();
    double y1 = rect.y() - startPoint.y();
    double x2 = (rect.x() + rect.width() - 1) - startPoint.x();
    double y2 = (rect.y() + rect.height() - 1) - startPoint.y();
    QPointF offset_1 = QPointF(x1, y1);
    QPointF offset_2 = QPointF(x2, y2);
    QPointF o1 = currentPoint + offset_1;
    QPointF o2 = currentPoint + offset_2;
    if(outOfPixmap(o1)){
        currentPoint -= QPointF(std::min(0.0, o1.x()), std::min(0.0, o1.y()));
    }
    if(outOfPixmap(o2)){
        currentPoint += QPointF(std::min(0.0, pixmap_.width()-o2.x()), std::min(0.0, pixmap_.height()-o2.y()));
    }
    return currentPoint;

}

void Canvas::mouseReleaseEvent(QMouseEvent *event){
    QPointF pos = transformPointToImage(event->localPos());
    // 完成拖动编辑
    if(event->button() & Qt::LeftButton){
      for(Shape *shape: shapes_){
          if(shape->isMoving() && !shape->hasSelectedVertex()){
              if (!outOfPixmap(pos)){
                  pos = canBoundShapePoint(shape, pos);
                  shape->finishMove(pos);
              }
              else{
                 shape->finishMove(QPointF{0,0});
              }
          }
      }
      update();
    }
}


QPointF Canvas::transformPointToImage(const QPointF point){
    QPointF offset = offsetToCenter();
    qreal x = qreal(point.x() / StateStorage::scale - offset.x());
    qreal y = qreal(point.y() / StateStorage::scale - offset.y());
    return QPointF(x, y);
}

bool Canvas::outOfPixmap(QPointF point){
    double w = pixmap_.width();
    double h = pixmap_.height();
    if(0 <= point.x() && point.x() < w-1 && 0 <= point.y() and point.y() <=h-1) {
        return false;
    }
    return  true;
}

QPointF Canvas::intersectionPoint(QPointF p1, QPointF p2){
    double pwidth = pixmap_.width();
    double pheight = pixmap_.height();
    double x1 = std::min(std::max(p1.x(), 0.0),pwidth-1);
    double y1 = std::min(std::max(p1.y(), 0.0), pheight-1);
    double x2 = p2.x();
    double y2 = p2.y();
    QVector<QPointF> ppoints;
    ppoints.push_back(QPointF(0,0));
    ppoints.push_back(QPointF(pwidth-1, 0));
    ppoints.push_back(QPointF(pwidth-1, pheight-1));
    ppoints.push_back(QPointF(0, pheight-1));
    QVector<QVector<double>> edges = intersectingEdges(QPointF(x1,y1),QPointF(x2,y2), ppoints);

    std::sort(edges.begin(), edges.end());
    QVector<double> np = edges[0];
    int index = np[1];
    double x3 = ppoints[index].x();
    double y3 = ppoints[index].y();
    int nextIndex = (index + 1) % 4;
    double x4 = ppoints[nextIndex].x();
    double y4 = ppoints[nextIndex].y();
    double x = np[2];
    double y = np[3];
    if(x== x1 && y == y1){
        if(x3 == x4){
            return QPointF(x3, std::min(std::max(0.0, y2), std::max(y3, y4)));
        }
        else {
           return QPointF(std::min(std::max(0.0, x2), std::max(x3, x4)), y3) ;
        }
    }
    return QPointF(x, y);
}



QVector<QVector<double>>  Canvas::intersectingEdges(QPointF p1, QPointF p2, QVector<QPointF> points){
    QVector<QVector<double>> res;
    double x1 = p1.x();
    double y1 = p1.y();
    double x2 = p2.x();
    double y2 = p2.y();
    for(int i=0; i<points.size(); i++){
        double x3 = points[i].x();
        double y3 = points[i].y();
        int nextIndex = (i+1)%4;

        double x4 = points[nextIndex].x();
        double y4 = points[nextIndex].y();

        double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
        double nua = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
        double nub = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
        if (denom == 0.0){
           continue;
        }

        double ua = nua / denom ;
        double ub = nub / denom;
        if(ua>=0 && ua <=1 && ub >=0 && ub <=1){
            double x = x1 + ua * (x2 - x1);
            double y = y1 + ua * (y2 - y1);
            QPointF m = QPointF((x3 + x4) / 2, (y3 + y4) / 2);

            QPointF p = (m - QPointF(x2,y2));
            double d  =  std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));

            QVector<double> tm ;
            tm.push_back(d);
            tm.push_back(i);
            tm.push_back(x);
            tm.push_back(y);
            res.push_back(tm);
        }
    }
    return res;
}

void Canvas::keyPressEvent(QKeyEvent *event){
    int key = event->key();

    if(key == Qt::Key_Escape && currentShape_!= nullptr){
        currentShape_ = nullptr;
        update();
    } else if (key == Qt::Key_Delete || key==Qt::Key_Backspace){
        deleteSelectedShape();
    }

}


QSize Canvas::sizeHint() const{
    return minimumSizeHint();
}

QSize Canvas::minimumSizeHint() const{
    if(!pixmap_.isNull()){
        return StateStorage::scale * pixmap_.size();
    }
    return QWidget::minimumSizeHint();
}
bool Canvas::hasSelected(){
    for(Shape *shape : shapes_){
        if (shape->isSelected()){
            return true;
        }
    }
    return false;
}

void Canvas::deleteSelectedShape(){
    for(Shape *shape: shapes_){
        if(shape->isSelected()){
            shapes_.removeOne(shape);
        }
    }
    update();
}

QJsonArray Canvas::getShapeAsJson(){
    QJsonArray allShapes ;
    for(Shape *shape :shapes_){
        if(!shape->isClosed()){
            continue;
        }
        allShapes.append(shape->convertToJson());
    }
    return allShapes;
}

void Canvas::restoreShapeFromJson(QJsonArray jsonArray){
    // TODO clean shapes
//    for(Shape *s : shapes_){
//        delete s;
//    }
    shapes_.clear();
    for(auto jsonVal: jsonArray){
        QJsonObject obj = jsonVal.toObject();
        QString typeName = obj["shape_type"].toString();
        int typeIndex = Shape::ShapeTypeNames.indexOf(typeName);
        QString label = obj["label"].toString();
        QJsonValue colorVal = obj["lineColor"];
        QString colorName="";
        if(colorVal.isNull()){
            QStringList colors = QColor::colorNames();
            int idx = rand() % colors.size() ;
            colorName = colors[idx];
        }else{
          colorName = obj["lineColor"].toString();
        }

        Shape *shape = new Shape(label, (Shape::ShapeType)typeIndex);
        QVector<QPointF> points;
        for(auto pointVal: obj["points"].toArray()){
            QJsonObject pointObj = pointVal.toObject();
            double x = pointObj["x"].toDouble();
            double y = pointObj["y"].toDouble();
            points.append(QPointF(x, y));
        }
        shape->setLineColor(QColor(colorName));
        shape->setPoints(points);
        shape->close();
        shape->setShapeState(Shape::NoState);
        shapes_.append(shape);
        emit addNewShape(shape);
    }
    qDebug() << "convert from json: " << shapes_.size();
    repaint();
}

bool Canvas::isNeedSave(){
    return hasEdit_;

}

void Canvas::resetState(){
    hasEdit_ = false;
}

int Canvas::shapeSize(){
    return shapes_.size();
}

void Canvas::addShape(Shape *shape){
    shapes_.append(shape);
}


QVector<Shape *> Canvas::getSelectedShape(){
    QVector<Shape *> selected ;
    for(Shape* s : shapes_){
        if(s->isSelected()){
            selected.push_back(s);
        }
    }
    return selected;
}

