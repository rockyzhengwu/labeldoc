#ifndef MSERTEXTDETECTOR_H
#define MSERTEXTDETECTOR_H

#include "mser.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
namespace ocrmodel {
  class MserTextDetector
  {
  public:
      MserTextDetector();
      void getmserRegions(cv::Mat gray);
      void drawEllipse(const MSER::Region &region, int width, int height, int depth, cv::Mat img,  cv::Scalar color);
      double getNormVariation(MSER::Region *region);
      std::vector<MSER::Region> removeDuplicate(std::vector<MSER::Region>);
      MSER::Region* linearReduction(MSER::Region*);
      std::vector<MSER::Region> treeReduction(MSER::Region*);
      void cleanRegion(MSER::Region *);


  private:
      double theta1_;
      double theta2_;
      double maxA_ ;
      double minA_;
  };
}

#endif // MSERTEXTDETECTOR_H
