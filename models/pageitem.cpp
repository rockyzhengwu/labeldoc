#include "pageitem.h"

namespace ocrmodel {
 PageItem::PageItem(){}

void PageItem::setType(PageItemType item_type){
     type_ = item_type;
}

PageItem::PageItemType PageItem::getType(){
  return type_;
}
void PageItem::addPoint(Point p){
    points_.push_back(p);
}

std::vector<Point> PageItem::getPoints(){
    return points_;
}
}
