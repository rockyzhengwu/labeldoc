#ifndef STATESTORAGE_H
#define STATESTORAGE_H

#include "canvas.h"
#include "shape.h"
#include <QStringList>

class StateStorage
{
public:
    StateStorage();
    static Canvas::Mode canvasMode;
    static Shape::ShapeType drawingShapeType;
    static double scale ;
    static double closeEpsilon;
    static QString labelName;
    static QString labelColor;
    static QStringList colorNames;

};

#endif // STATESTORAGE_H
