#ifndef COMMUTIL_H
#define COMMUTIL_H

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <allheaders.h>

namespace ocrmodel {
   Pix *mat8ToPix(cv::Mat &mat8);
   cv::Mat pix8ToMat(Pix *pix8);
}

#endif // COMMUTIL_H
