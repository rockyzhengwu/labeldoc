#ifndef BINARY_H_
#define BINARY_H_
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <string>

namespace imb
{
//   void padding(cv::Mat &im_gray, cv::Size window_size);
   void otsu(cv::Mat im_gray, cv::Mat& dest);
   void localThreshold(cv::Mat &im_grah, cv::Mat& dest, cv::Size window_size, double k, std::string algName);
   void niblack(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k);
   void sauvola(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k);
   void nick(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k);
   void wolf(cv::Mat im_gray, cv::Mat& dest, cv::Size window_size, double k);

   void bernsen(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, unsigned char threshold, int contrast_limit);

   void highContrastImage(cv::Mat &im_gray, cv::Mat &high_contrast_image);

   void isauvola(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, double k);
   void wan(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, double k);
   void trsingh(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, double k);
}

#endif
