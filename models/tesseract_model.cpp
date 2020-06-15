#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <allheaders.h>
#include "tesseract_model.h"
#include "commutil.h"

static const char* BLOCK_TYPE_NAMES[] = {"PT_UNKNOWN",
"PT_FLOWING_TEXT",
"PT_HEADING_TEXT",
"PT_PULLOUT_TEXT",
"PT_EQUATION",
"PT_INLINE_EQUATION",
"PT_TABLE",
"PT_VERTICAL_TEXT",
"PT_CAPTION_TEXT",
"PT_FLOWING_IMAGE",
"PT_HEADING_IMAGE",
"PT_PULLOUT_IMAGE",
"PT_HORZ_LINE",
"PT_VERT_LINE",
"PT_NOISE",
"PT_COUNT"};

namespace ocrmodel {


TesseractHandler::TesseractHandler(){

}


std::vector<PageItem> TesseractHandler::tesseract_analysis_rects(cv::Mat input_image, std::vector<cv::Rect> areas){
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(nullptr, "chi_sim")) {
       fprintf(stderr, "Could not initialize tesseract.\n");
     }
    std::vector<PageItem> all_items;
    for(cv::Rect rect: areas){
        int x = rect.x;
        int y = rect.y;
        qDebug() << "x: " << x << " y " << y;
        cv::Mat sub_image = input_image(rect);
        std::vector<PageItem> txt_lines = analysis_textline(sub_image, api);
        for(PageItem item: txt_lines){
            item.moveTo(x, y);
            all_items.push_back(item);
        }
    }
    return all_items;
}


std::vector<PageItem> TesseractHandler::tesseract_analysis(cv::Mat input_image){
    qDebug() << "strt tesseract analysis";
     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
     if (api->Init(nullptr, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
      }
     std::vector<PageItem> all_items = analysis_textline(input_image, api);
    //cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/test_layout.png",  input_image);
    qDebug() <<"finish tesseract";
    api->End();
    return all_items;
 }

std::vector<PageItem> TesseractHandler::analysis_textline(cv::Mat image, tesseract::TessBaseAPI * api){
    std::vector<PageItem> all_items;
     Pix *pix = mat8ToPix(image);
     api->SetImage(pix);
     api->SetPageSegMode(tesseract::PSM_AUTO);
     // set threshold
     tesseract::ImageThresholder *threshold = new tesseract::ImageThresholder();
     threshold->SetImage(pix);
     api->SetThresholder(threshold);
     Pixa *pixa = pixaCreate(1);
     pixaAddPix(pixa, pix, 0);
     int *blockIds;
     int *paramIds;
     Boxa* textLines = api->GetTextlines(false, 0 , &pixa, &blockIds, &paramIds);
     for(int i=0; i< textLines->n; i++){
         Box *b = boxaGetBox(textLines, i, 1);
         int x1 = b->x;
         int y1 = b->y;
         int x2 = x1 + b->w;
         int y2 = y1 + b->h;
         PageItem item;
         item.setType(PageItem::TEXTLINE);
         item.addPoint(Point{x1, y1});
//         item.addPoint(Point{x1, y2});
         item.addPoint(Point{x2, y2});
//         item.addPoint(Point{x2, y1});
         all_items.push_back(item);
     }
     pixDestroy(&pix);
     return all_items;
}


void TesseractHandler::close(){

}

TesseractHandler::~TesseractHandler(){
    qDebug()<< "析构函数";
}

}

