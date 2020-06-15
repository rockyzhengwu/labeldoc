#ifndef TESSERACT_MODEL_H
#define TESSERACT_MODEL_H
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include "pageitem.h"


namespace ocrmodel {
   // input_image is binary at there
class TesseractHandler{
public:
    TesseractHandler();
    ~TesseractHandler();
   std::vector<PageItem> tesseract_analysis(cv::Mat input_image);
   std::vector<PageItem> tesseract_analysis_rects(cv::Mat input_image, std::vector<cv::Rect> areas);
   void close();


private:
     tesseract::TessBaseAPI *api;
     std::vector<PageItem> analysis_textline(cv::Mat image, tesseract::TessBaseAPI * api=nullptr);
};
}

#endif
