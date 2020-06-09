#include "binary.h"

namespace imb{
  void otsu(cv::Mat im_gray, cv::Mat& dest){
    cv::threshold(im_gray, dest, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
  }
}
