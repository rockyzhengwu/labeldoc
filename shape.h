#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <QString>
#include <QJsonObject>


struct VertexHightStyle{
    double size;
    QString type;
};



class Shape :public QObject
{

public:
    enum ShapeType {NONE, POINT, LINE, RECTANGLE, POLYGON, CIRCLE};
    enum ShapeState {NoState, Drawing, Hover, Selected,  Moving};
    static QStringList ShapeTypeNames;;
//    explicit Shape(ShapeType type);
    Shape(QString label, ShapeType type);

    void setLineColor(QColor color);
    QString getLineColor();
    void setShapeState(ShapeState state);

    void paint(QPainter *painter);
    void paintDrawing(QPainter *painter);
    void paintClosed(QPainter *painter);

    void addPoint(QPointF point );
    QPointF getLastPoint();
    bool isClosed();
    void close();

    void updateLastPoint(QPointF point);
    void updatePoint(int pointIndex, QPointF point );

    int pointSize();
    QPointF pointAt(int paintIndex);
    QVector<QPointF> getPoints();
    void setPoints(QVector<QPointF> points);

    void setDrawingPoint(QPointF point);
    bool containsPoint(QPointF point);

    bool closeEnough(QPointF pos);
    QPainterPath makePath();
    QRectF boundingRect();

    // about drop vertex
    void clearSelectVertex();
    int getNearestVertexIndex(QPointF point, double dis);
    bool hasSelectedVertex();
    void moveSelectedVertex(QPointF point);
    const QPointF* getSelectPoint() const;

    // some function about move whole shape
    bool isSelected();
    void startMove(QPointF point);
    void currentMove(QPointF point);
    void paintMove(QPainter *painter);
    bool isMoving();
    void finishMove(QPointF point);
    QPointF getStartMovePoint();

    QRectF getCircleRectFromLine(QPointF p1, QPointF p2);
    double calDis(QPointF p1, QPointF p2);
    ShapeState getShapeState();

    QJsonObject convertToJson();
    QString getLabel();

private:

    // label
    QString label_;
    // shape 类型
    ShapeType type_;

    // 是否关闭
    bool closed_ = false;
    ShapeState state_ = ShapeState::NoState;
    // 顶点
    QVector<QPointF> points_;
    // 被选择高粱的点
    QPointF *selectedPoint_;
    // 相关颜色
    QColor vertexFillcolor_ ;
    QColor selectVertexFillColor_;
    QColor lineColor_  ;
    QColor fillColor_  ;
    QColor selectLineColor_;
    QColor selectFillColor_;
    // drawing 相关
    // 正在绘制的顶点
    QPointF drawingPoint;

    bool nearVertex_ ;
    bool fill_ ;

    // 移动相关
    QPointF startMovePoint_;
    QPointF currentMovePoint_;

    void drawVertex(QPainterPath *path, QPointF point);

    QString getTypeAsString();

};

#endif // SHAPE_H
