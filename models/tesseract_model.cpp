#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <allheaders.h>
#include "tesseract_model.h"

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

Pix *mat8ToPix(cv::Mat &mat8)
{
    Pix *pixd = pixCreate(mat8.size().width, mat8.size().height, 8);
    for(int y=0; y<mat8.rows; y++) {
        for(int x=0; x<mat8.cols; x++) {
            pixSetPixel(pixd, x, y, (l_uint8) mat8.at<uchar>(y,x));
        }
    }
    return pixd;
}

cv::Mat pix8ToMat(Pix *pix8)
{
    cv::Mat mat(cv::Size(pix8->w, pix8->h), CV_8UC1);
    uint32_t *line = pix8->data;
    for (uint32_t y = 0; y < pix8->h; ++y) {
        for (uint32_t x = 0; x < pix8->w; ++x) {
            mat.at<uchar>(y, x) = GET_DATA_BYTE(line, x);
        }
        line += pix8->wpl;
    }
    return mat;
}
TesseractHandler::TesseractHandler(){
//   api_ = new tesseract::TessBaseAPI();
//   if (api_->Init(nullptr, "chi_sim")) {
//      fprintf(stderr, "Could not initialize tesseract.\n");
//    }
}

void deswap(Pix *pixb){
    PTAA         *ptaa1, *ptaa2;
    PIX *pixt1, *pixt2;
    ptaa1 = dewarpGetTextlineCenters(pixb, 0);
    Pixa *pixadb;
    Pix *pixvws;
    l_int32 tlfound ;
    pixt2 = pixGenTextlineMask(pixb, &pixvws, &tlfound, pixadb);
    pixWriteImpliedFormat("/Users/zhengwu/workspace/qtprojects/github/pixt2.jpg",  pixt2, 0, 0);

}

std::vector<PageItem> TesseractHandler::tesseract_analysis(cv::Mat input_image){
    qDebug() << "strt tesseract analysis";
     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
     if (api->Init(nullptr, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
      }
//    if(api_){
//        qDebug() << "api is null";
//       api_ = new tesseract::TessBaseAPI();
//       qDebug() << "tesseract" ;
//      if (api_->Init(nullptr, "chi_sim")) {
//         fprintf(stderr, "Could not initialize tesseract.\n");
//       }
//    }
     qDebug() << "new res items";
     std::vector<PageItem> all_items;
     qDebug()<<"mat convert to pix";
     Pix *pix = mat8ToPix(input_image);
     qDebug()<<"start deskew";
     l_float32    angle, conf;
     Pix* pix1 = pixConvertTo1(pix, 128);
     deswap(pix1);
//     pixFindSkew(pix1, &angle, &conf);
//     pixWriteImpliedFormat("/Users/zhengwu/workspace/qtprojects/github/result1_deskew.jpg", pix1, 0, 0);
//    qDebug()<<" angle " << angle << " conf" << conf;
//    Pix* pixd = pixRotate(pix1, angle * (3.1415/180.0), L_ROTATE_AREA_MAP, L_BRING_IN_WHITE, 0, 0);
//    pixWriteImpliedFormat("/Users/zhengwu/workspace/qtprojects/github/result2_deskew.jpg", pixd, 0, 0);
     qDebug()<<"segImage " ;
     api->SetImage(pix);
     api->SetPageSegMode(tesseract::PSM_AUTO);
     // set threshold
     tesseract::ImageThresholder *threshold = new tesseract::ImageThresholder();
     threshold->SetImage(pix);
     api->SetThresholder(threshold);
     Pixa *pixa = pixaCreate(1);
     pixaAddPix(pixa, pix, 0);
//     Boxa* regionBoxa = api_->GetRegions(&pixa);
//     qDebug() << "region box count " << regionBoxa->n;
//     for(int i=0; i< regionBoxa->n; i++){
//         Box *b = boxaGetBox(regionBoxa, i, 1);
//         qDebug()<< " x:" << b->x <<" y:"<<b->y;
//         int x1 = b->x;
//         int y1 = b->y;
//         int x2 = x1 + b->w;
//         int y2 = y1 + b->h;
//         PageItem item;
//         item.setType(PageItem::TEXTLINE);
//         item.addPoint(Point{x1, y1});
//         item.addPoint(Point{x2, y2});
//         all_items.push_back(item);
//     }
     int *blockIds;
     int *paramIds;
     qDebug()<<"start get Textline";
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
         item.addPoint(Point{x1, y2});
         item.addPoint(Point{x2, y2});
         item.addPoint(Point{x2, y1});
         all_items.push_back(item);
     }
     api->GetIterator();

//     api_->Recognize(0);
//     tesseract::ResultIterator* it = api_->GetIterator();
//     tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;
//     if (it != 0) {
//         do {
//            int x1, y1, x2, y2;
//            it->BoundingBox(level, &x1, &y1, &x2, &y2);
//            cv::Rect rect(x1, y1, x2-x1, y2-y1);
////            cv::rectangle(input_image, rect, cv::Scalar(0, 255, 0), 2);
//            std::string label = BLOCK_TYPE_NAMES[it->BlockType()];
//            PageItem item;
//            item.setType(PageItem::TEXTLINE);
//            item.addPoint(Point{x1, y1});
//            item.addPoint(Point{x2, y2});
//            all_items.push_back(item);
//         } while (it->Next(level));
//       }
      //cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/test_layout.png",  input_image);
    pixDestroy(&pix);
    qDebug() <<"finish tesseract";
    api->End();
    return all_items;
 }
void TesseractHandler::close(){
}
TesseractHandler::~TesseractHandler(){
    qDebug()<< "析构函数";
}
}

