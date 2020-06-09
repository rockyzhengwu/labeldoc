#include "binary.h"


namespace imb {
void wolf(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k){

    cv::Mat im_sum, im_sum_sq;
    cv::integral(im_gray,im_sum,im_sum_sq,CV_64F);
    int window_width = window_size.width;
    int window_height = window_size.height;

    int window_half_width = window_width / 2;
    int window_half_height = window_height / 2;

    int image_height = im_gray.size().height;
    int image_width = im_gray.size().width;

    double max_stdev=0.0;
    double min_pixel = 255.0;
    for(int i=0; i< image_height; i++){
      for(int j=0; j<image_width; j++){

        int min_x = std::max(0, i - window_half_height);
        int min_y = std::max(0, j - window_half_width);

        int max_x = std::min(i + window_half_width, image_height-1);
        int max_y = std::min(j + window_half_width, image_width-1);

        double window_sum = im_sum.at<double>(max_x, max_y) + im_sum.at<double>(min_x, min_y)
           - im_sum.at<double>(min_x, max_y) - im_sum.at<double>(max_x, min_y);

        double window_sum_sq =   im_sum_sq.at<double>(max_x, max_y) + im_sum_sq.at<double>(min_x, min_y) -
           im_sum_sq.at<double>(min_x, max_y)-im_sum_sq.at<double>(max_x, min_y);

        int window_area = (max_y-min_y) *(max_x - min_x) + 1;
        double window_mean = window_sum / window_area;

        double window_stdev  = sqrt ((window_sum_sq - window_mean*window_sum)/window_area);
        if(window_stdev > max_stdev){
            max_stdev = window_stdev;
        }
        if(im_gray.at<unsigned char>(i, j) < min_pixel){
            min_pixel = im_gray.at<unsigned char>(i, j);
        }
      }
    }

    for(int i=0; i< image_height; i++){
      for(int j=0; j<image_width; j++){

        int min_x = std::max(0, i - window_half_height);
        int min_y = std::max(0, j - window_half_width);

        int max_x = std::min(i + window_half_width, image_height-1);
        int max_y = std::min(j + window_half_width, image_width-1);

        double window_sum = im_sum.at<double>(max_x, max_y) + im_sum.at<double>(min_x, min_y)
           - im_sum.at<double>(min_x, max_y) - im_sum.at<double>(max_x, min_y);

        double window_sum_sq =   im_sum_sq.at<double>(max_x, max_y) + im_sum_sq.at<double>(min_x, min_y) -
           im_sum_sq.at<double>(min_x, max_y)-im_sum_sq.at<double>(max_x, min_y);

        int window_area = (max_y-min_y) *(max_x - min_x) + 1;
        double window_mean = window_sum / window_area;
        double window_stdev  = sqrt ((window_sum_sq - window_mean*window_sum)/window_area);
        double threshold = window_mean - k*(1-(window_stdev/ max_stdev)) *(window_mean -min_pixel);
        if(im_gray.at<unsigned char>(i, j) < threshold){
          dest.at<unsigned char>(i, j) = 0;
        }else{
          dest.at<unsigned char>(i, j) = 255;
        }
      }
    }
}
}
