#include "statestorage.h"

StateStorage::StateStorage()
{

}

Canvas::Mode StateStorage::canvasMode = Canvas::NOMODE;

Shape::ShapeType StateStorage::drawingShapeType = Shape::POLYGON;

double StateStorage::scale = 1.0;
double StateStorage::closeEpsilon = 10.0;
QString StateStorage::labelName = "";
QString StateStorage::labelColor="";
QStringList StateStorage::colorNames{"red"
                                     ,"blue"
                                     ,"brown"
                                     ,"coral"
                                     ,"blueviolet"
                                     ,"burlywood"
                                     ,"cadetblue"
                                     ,"chartreuse"
                                     ,"chocolate"
                                     ,"cornflowerblue"
                                     ,"darkcyan"
                                     ,"deeppink"
                                     ,"firebrick"
                                     ,"fuchsia"
                                     ,"yellowgreen"};
