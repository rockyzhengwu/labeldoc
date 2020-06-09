#include "binary.h"
#include <iostream>

namespace imb {
void sauvola(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k){
    localThreshold(im_gray, dest, window_size, k, "sauvola");
  }
}
