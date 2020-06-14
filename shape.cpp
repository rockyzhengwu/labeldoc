#include "shape.h"
#include "statestorage.h"

#include <QPen>
#include <QColor>
#include <QPainterPath>
#include <QRectF>
#include <QJsonArray>

#include <QDebug>
#include <cmath>


static QColor VERTEX_FILE_COLOR = QColor(0,255, 0, 255);
static QColor DEFAULT_LINE_COLOR = QColor(0, 255,0, 128);
static QColor DEFAULT_FILL_COLOR = QColor(0, 255, 0, 128);
static QColor VERTEX_SELECT_FILL_COLOR = QColor(255, 255, 255, 255);
static QColor DEFAULT_SELECT_LINE_COLOR = QColor(255, 255, 255);
static QColor DEFAULT_SELECT_FILL_COLOR = QColor(0, 255, 0, 155) ;
QStringList Shape::ShapeTypeNames{"none", "point", "line", "rectangle", "polygon", "circle"};

//static VertexHightStyle NEAR_VERTEX_STYLE = VertexHightStyle{3.0, "round"};
//static VertexHightStyle SELECTEC_VERTEX_STYPE = VertexHightStyle{1.5, "squre"};

Shape::Shape(QString label, ShapeType type):
    label_{label},
    type_{type},
    closed_{false},
    state_{Shape::NoState},
    selectedPoint_{nullptr},
    vertexFillcolor_{DEFAULT_FILL_COLOR},
    selectVertexFillColor_{VERTEX_SELECT_FILL_COLOR},
    lineColor_{DEFAULT_LINE_COLOR},
    fillColor_{DEFAULT_FILL_COLOR},
    selectLineColor_{DEFAULT_SELECT_LINE_COLOR},
    selectFillColor_{DEFAULT_SELECT_FILL_COLOR},
    drawingPoint{0,0},
    nearVertex_{false},
    fill_{false},
    startMovePoint_{0,0},
    currentMovePoint_{0,0}
{
    state_ = ShapeState::Drawing;
    lineColor_ = StateStorage::labelColor;
    vertexFillcolor_ = StateStorage::labelColor;
}

void Shape::setShapeState(ShapeState state){
    state_ = state;
}
double Shape::calDis(QPointF p1, QPointF p2){
    QPointF p = p1-p2;
    double dis = std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));
    return dis;
}

QRectF Shape::getCircleRectFromLine(QPointF p1, QPointF p2){
    double d = calDis(p1, p2);
    QRectF rect = QRectF(p1.x() - d, p1.y() - d, 2 * d, 2 * d);
    return rect;
}

void Shape::paintMove(QPainter *painter){
    QPainterPath linePath;
    QPainterPath vertexPath;
    // todo 移动
    QPointF dp = currentMovePoint_ - startMovePoint_;
    if(type_==ShapeType::RECTANGLE){
        if (points_.size() == 2){
            linePath.addRect(QRectF(points_[0]+dp, points_[1]+dp));
        }
        for (QPointF point: points_){
            drawVertex(&vertexPath, point+dp);
        }
    }
    else if(type_ == ShapeType::POLYGON || type_==ShapeType::LINE){
        linePath.moveTo(points_[0]+dp);
        for(QPointF p: points_){
            linePath.lineTo(p+dp);
            drawVertex(&vertexPath, p+dp);
        }
    }
    else if(type_ == ShapeType::CIRCLE){
        if(points_.size() == 2){
            linePath.addEllipse(getCircleRectFromLine(points_[0]+dp, points_[1]+dp));
        }
        for (QPointF point: points_){
            drawVertex(&vertexPath, point+dp);
        }
    }

    painter->drawPath(linePath);
    painter->drawPath(vertexPath);
    painter->fillPath(linePath, selectFillColor_);
    if (nearVertex_){
        painter->fillPath(vertexPath, selectVertexFillColor_);
    }else{
      painter->fillPath(vertexPath, vertexFillcolor_);
    }
}
void Shape::paintDrawing(QPainter *painter){
    QPainterPath linePath;
    QPainterPath vertexPath;
    if(type_==ShapeType::RECTANGLE){
        linePath.addRect(QRectF(points_[0], drawingPoint));
        drawVertex(&vertexPath, points_[0]);
        drawVertex(&vertexPath, drawingPoint);
    }
    else if(type_== ShapeType::CIRCLE){
        linePath.addEllipse(getCircleRectFromLine(points_[0], drawingPoint));
        drawVertex(&vertexPath, points_[0]);
        drawVertex(&vertexPath, drawingPoint);
    }

    else if(type_ == ShapeType::POLYGON && points_.size() >0){
        linePath.moveTo(points_[0]);
        for(QPointF p: points_){
            linePath.lineTo(p);
            drawVertex(&vertexPath, p);
        }
        linePath.lineTo(drawingPoint);
        drawVertex(&vertexPath, drawingPoint);
    } else if(type_ ==ShapeType::LINE && points_.size()>0){
        linePath.moveTo(points_[0]);
        for(QPointF p: points_){
            linePath.lineTo(p);
            drawVertex(&vertexPath, p);
        }
        linePath.lineTo(drawingPoint);
        drawVertex(&vertexPath, drawingPoint);
    } else if(type_==ShapeType::POINT){
        linePath.moveTo(points_[0]);
        linePath.lineTo(points_[0]);
        drawVertex(&vertexPath, points_[0]);
    }
    painter->drawPath(linePath);
    painter->drawPath(vertexPath);
    painter->fillPath(linePath, selectFillColor_);
    if (nearVertex_){
        painter->fillPath(vertexPath, selectVertexFillColor_);
    }else{
      painter->fillPath(vertexPath, vertexFillcolor_);
    }

}

void Shape::paintClosed(QPainter *painter){
    QPainterPath linePath;
    QPainterPath vertexPath;

    if(type_==ShapeType::RECTANGLE){
        if (points_.size() == 2){
            linePath.addRect(QRectF(points_[0], points_[1]));
        }
        for (QPointF point: points_){
            drawVertex(&vertexPath, point);
        }
    }
    else if(type_==Shape::LINE){
        linePath.moveTo(points_[0]);
        for(QPointF p: points_){
            linePath.lineTo(p);
            drawVertex(&vertexPath, p);
        }
    } else if(type_==Shape::POLYGON ){
        linePath.moveTo(points_[0]);
        for(QPointF p: points_){
            linePath.lineTo(p);
            drawVertex(&vertexPath, p);
        }
        linePath.lineTo(points_[0]);
    } else if(type_ == Shape::POINT){
        linePath.moveTo(points_[0]);
        linePath.lineTo(points_[0]);
        drawVertex(&vertexPath, points_[0]);
    } else if(type_==Shape::CIRCLE){
//        linePath.moveTo(points_[0]);
          linePath.addEllipse(getCircleRectFromLine(points_[0], points_[1]));
        for(QPointF p: points_){
            drawVertex(&vertexPath, p);
        }
    }

    painter->drawPath(linePath);
    painter->drawPath(vertexPath);
    painter->fillPath(vertexPath, vertexFillcolor_);
    if(state_ == Shape::ShapeState::Selected || hasSelectedVertex() || state_==Shape::Hover){
        painter->fillPath(linePath, selectFillColor_);
    }
}

void Shape::paint(QPainter *painter){
    if (points_.isEmpty()){
        return;
    }

    QPen pen;
    pen.setWidth(std::max(1, (int)std::round(2/StateStorage::scale)));
    if(isSelected()){
        pen.setColor(selectLineColor_);
    }else{
      pen.setColor(lineColor_);
    }

    painter->setPen(pen);
    if (state_==Shape::ShapeState::Drawing){
        paintDrawing(painter);
        return;
    } else if(state_ == Shape::ShapeState::Moving && !hasSelectedVertex()){
        paintMove(painter);
    }
    else{
       paintClosed(painter);
    }
}

void Shape::addPoint(QPointF point){
    if(points_.size() ==0){
        drawingPoint = point;
    }

    if(points_.size()>0 && closeEnough(point) && type_==Shape::POLYGON){
//        points_.append(points_[0]);
        close();
    } else if(type_ == Shape::RECTANGLE || type_==Shape::CIRCLE){
        if(points_.size() <2){
          points_.append(point);
        }
        if(points_.size()==2 ){
            close();
        }
    } else if(type_==Shape::LINE){
        if(points_.size() < 2){
            points_.append(point);
        }
        if(points_.size()==2){
            close();
        }
    }else if(type_==Shape::POINT){
        if(points_.size()==0){
            points_.append(point);
        }
        if(points_.size() == 1){
            close();
        }
    }
    else{
      points_.append(point);
    }
}

const QPointF* Shape::getSelectPoint() const{
    return selectedPoint_;
}

QPointF Shape::getLastPoint(){
    if(points_.size()>0){
        return points_[points_.size()-1];
    }
    return QPointF{0, 0};
}

void Shape::drawVertex(QPainterPath *path, QPointF point ){
    double d = 8.0 / StateStorage::scale;
    if (closeEnough(point) && nearVertex_){
        d *= 3.0;
        path->addEllipse(point, d/2.0, d/2.0);
    }
    else if(selectedPoint_!=nullptr and *selectedPoint_==point){
        d *= 1.5;
        path->addRect(point.x() - d/2, point.y() - d/2 , d, d );
    }
    else{
      path->addEllipse(point, d/2.0, d/2.0);
    }
}

void Shape::close() {
    closed_ = true;
    nearVertex_ = false;
    state_ = ShapeState::NoState;
}


bool Shape::isClosed(){
    return closed_;
}


int Shape::pointSize(){
    return points_.size();
}

void Shape::updateLastPoint(QPointF point){
    if (points_.size() > 1){
      points_[points_.size() -1 ] = point;
    }
}

void Shape::updatePoint(int pointIndex, QPointF p){
    if(pointIndex < 0 or pointIndex > points_.size() ){
        return;
    }
    points_[pointIndex] = p;
}

QPointF Shape::pointAt(int pointIndex){
    return points_[pointIndex];
}

bool Shape::closeEnough(QPointF pos){
    if(points_.size() > 1 and type_==Shape::POLYGON ) {
        double dis = calDis(pos, points_[0]);
        if(dis < StateStorage::closeEpsilon/ StateStorage::scale){
            return true;
        }
    }
    return false;
}

QVector<QPointF> Shape::getPoints(){
    return points_;
}

void Shape::setPoints(QVector<QPointF> points){
    points_ = points;
}

void Shape::setDrawingPoint(QPointF point){
    if (closeEnough(point)){
        drawingPoint = points_[0];
        nearVertex_ = true;
    }
    else{
        nearVertex_ = false;
        drawingPoint = point;
    }
}

bool Shape::containsPoint(QPointF point){
    return makePath().contains(point);
}


QPainterPath Shape::makePath() {
    QPainterPath path ;
    if(type_ == Shape::POLYGON || type_==Shape::LINE || type_ == Shape::POINT){
        path.moveTo(points_[0]);
        for(QPointF p: points_){
            path.lineTo(p);
        }
    }
    else if(type_ == Shape::RECTANGLE){
        if(points_.size() == 2){
            QRectF rect = QRectF(points_[0], points_[1]);
            path.addRect(rect);
        }
    }else if(type_==Shape::CIRCLE){
        path.addRect(getCircleRectFromLine(points_[0], points_[1]));
    }
    return path;
}

QRectF Shape::boundingRect() {
    QPainterPath path = makePath();
    return path.boundingRect();
}

// 判断是否有顶点被选中
int Shape::getNearestVertexIndex(QPointF point, double dis){
    int resIndex = -1;
    double minDis = dis;
    for(int i=0; i<points_.size() ; i++ ){
        QPointF p = points_[i];
        double dist = calDis(p, point);
//        double dist = (p-point).manhattanLength();
        if(dist < minDis){
           resIndex = i;
           minDis = dist;
        }
    }
    if (resIndex >= 0){
      selectedPoint_ = &points_[resIndex];
    }else{
      selectedPoint_ = nullptr;
    }
    return resIndex;
}

void Shape::clearSelectVertex(){
    selectedPoint_ = nullptr;
}


bool Shape::hasSelectedVertex(){
    return selectedPoint_ != nullptr;
}

void Shape::moveSelectedVertex(QPointF point){
    if (selectedPoint_ != nullptr){
       *selectedPoint_ += point - *selectedPoint_;
    }
}

bool Shape::isSelected(){
    return state_ == ShapeState::Selected;
}


void Shape::startMove(QPointF point){
    startMovePoint_ = point;
}

void Shape::currentMove(QPointF point){
    currentMovePoint_ = point;
}


bool Shape::isMoving(){
    return state_ == ShapeState::Moving;
}


// 结束移动, point 结束的终点
void Shape::finishMove(QPointF point){
    qDebug() << "finish moving ";
    if (point.isNull()){
        point = currentMovePoint_;
    }
    QPointF dp = point - startMovePoint_;
    for(int i=0; i< points_.size(); i++){
        points_[i] += dp;
    }
    startMovePoint_ = QPointF{};
    currentMovePoint_ = QPointF{};
    setShapeState(ShapeState::NoState);
}


QPointF Shape::getStartMovePoint(){
    return startMovePoint_;
}

Shape::ShapeState Shape::getShapeState(){
    return state_;
}
QString Shape::getTypeAsString(){
    return ShapeTypeNames[type_];
//    if(type_==Shape::LINE){
//        return  "line";
//    }
//    if(type_==Shape::RECTANGLE){
//        return "rectangle";
//    }
//    if(type_==Shape::POINT){
//        return  "point";
//    }
//    if(type_==Shape::POLYGON){
//        return  "polygon";
//    }
//    if(type_==Shape::CIRCLE){
//        return "circle";
//    }
//    else {
//        return "";
//    }
}

QJsonObject Shape::convertToJson(){
    QJsonObject jsonObj;
    jsonObj["label"] = label_;
    jsonObj["shape_type"] = getTypeAsString();
    jsonObj["lineColor"]  = lineColor_.name();
    QJsonArray pointArray;
    for(QPointF p: points_){
        QJsonObject pobj;
        pobj["x"] = p.x();
        pobj["y"] = p.y();
        pointArray.append(pobj);
    }
    jsonObj["points"] = pointArray;
    return jsonObj;
}

void Shape::setLineColor(QColor color){
    lineColor_ = color;
    vertexFillcolor_ = color;
}

QString Shape::getLabel(){
    return label_;
}
QString Shape::getLineColor(){
    return lineColor_.name();
}


