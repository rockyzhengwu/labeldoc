#include "binary.h"

namespace imb {
void bernsen(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, unsigned char threshold,  int contrast_limit){
    cv::Mat min_image;
    cv::Mat max_image;
    cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                          window_size ,
                          cv::Point( window_size.width/2, window_size.height/2) );

    cv::erode(im_gray, min_image, element);
    cv::dilate(im_gray, max_image, element);
    int image_height = im_gray.size().height;
    int image_width = im_gray.size().width;
    for(int i=0; i<image_height; i++){
        for(int j=0; j<image_width; j++){
            unsigned char max_v = max_image.at<unsigned char>(i, j);
            unsigned char min_v = min_image.at<unsigned char>(i, j);
            unsigned char th = (max_v - min_v) > contrast_limit ? (max_v + min_v)/2 : threshold;
            dest.at<unsigned char>(i, j) = im_gray.at<unsigned char>(i, j) > th ? 255 : 0;
        }
    }
}
}
