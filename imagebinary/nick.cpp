#include "binary.h"
namespace imb {

void nick(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k){
    localThreshold(im_gray, dest, window_size, k, "nick");
}
}
