#include "commutil.h"

namespace ocrmodel{


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
}
