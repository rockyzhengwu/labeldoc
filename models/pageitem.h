#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <vector>

namespace  ocrmodel {

struct Point{
  int x;
  int y;
};

class PageItem
{
public:
    enum PageItemType{TEXTLINE, TEXTBLOCK, IMAGE, TABLE};
    PageItem();
    void setType(PageItemType item_type);
    PageItemType getType();
    void addPoint(Point p);
    std::vector<Point> getPoints();
    void moveTo(int x, int y);

private:
    PageItemType type_;
    std::vector<Point> points_;

};
}

#endif // PAGEITEM_H
