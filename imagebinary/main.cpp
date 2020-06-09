#include <iostream>
#include <fstream>
#include "binary.h"

int main(){
  std::string image_name= "/Users/zhengwu/workspace/oschina/imagebinary/test.png";
  //  image_name= "/Users/zhengwu/workspace/oschina/imagebinary/build/sample.jpg";
  cv::Mat src = cv::imread( image_name, cv::IMREAD_COLOR ); // Load an image
  if (src.empty())
  {
      std::cout << "Cannot read the image: " << image_name<< std::endl;
      return -1;
  }
  cv::Mat src_gray;
  cvtColor( src, src_gray, cv::COLOR_BGR2GRAY ); // Convert the image to Gray
  cv::Mat binary_dest(src_gray.size().height, src_gray.size().width, CV_8U );
  // imb::otsu(src_gray, binary_dest);
  cv::Size window_size(250, 250);
  double k = -1.0;
  imb::niblack(src_gray, binary_dest, window_size, k);
  std::cout << "binary rows "<<binary_dest.size() ;
  std::ofstream outputFile;
  outputFile.open("binary.txt");
    // Iterate through pixels.
    for (int r = 0; r < binary_dest.rows; r++)
    {
        for (int c = 0; c < binary_dest.cols; c++)
        {
            int pixel = binary_dest.at<uchar>(r,c);
            outputFile << pixel << '\t';
        }
        outputFile << std::endl;
    }
    // Close the file.
  outputFile.close();
  cv::imwrite("binary.jpg", binary_dest);
}