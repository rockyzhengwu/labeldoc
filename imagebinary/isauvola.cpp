#include "binary.h"
#include <unordered_set>
#include <QDebug>

namespace imb {
void highContrastImage(cv::Mat &im_gray, cv::Mat &high_contrast_image){
    const int window_size= 3;
    int img_width = im_gray.size().width;
    int img_height = im_gray.size().height;
    cv::Mat min_image = cv::Mat(img_height, img_width, CV_8UC1);
    cv::Mat max_image = cv::Mat(img_height, img_width, CV_8UC1);

    cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                          cv::Size(window_size*2+1, window_size*2+1),
                          cv::Point( window_size, window_size) );

    cv::erode(im_gray, min_image, element);
    cv::dilate(im_gray, max_image, element);
    uchar* min_row_ptr;
    uchar* max_row_ptr;
    for(int i=0; i< img_height; i++){
        min_row_ptr= min_image.ptr(i);
        max_row_ptr = max_image.ptr(i);
        for(int j=0; j< img_width; j++){
            unsigned char max_pixel = max_row_ptr[j];
            unsigned char min_pixel = min_row_ptr[j];
            double p = (max_pixel-min_pixel)/(max_pixel + min_pixel + 0.00001);
            high_contrast_image.at<unsigned char>(i, j) = p * 255;
        }
    }
    otsu(high_contrast_image, high_contrast_image);
}

struct pair_hash
{
    template<class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

void spider(cv::Mat &im_in, cv::Mat &im_out, std::pair<int, int> start_point){
    std::unordered_set<int> positions;
    int img_width = im_in.cols;
    int img_height = im_in.rows;
    positions.insert(start_point.first* img_width + start_point.second);
    while (!positions.empty()) {
        int p = *positions.begin();

        int y = p / img_width;
        int x = p % img_width;

        int start_x = std::max(0, x-1);
        int start_y = std::max(0, y-1);
        int end_x = std::min(x+1, img_width-1);
        int end_y = std::min(y+1, img_height-1);

        for(int i=start_y; i<=end_y; i++){
            for(int j=start_x;j<=end_x; j++){
                if(im_in.at<uchar>(i, j)==0 && im_out.at<uchar>(i,j)==255){
                    im_out.at<uchar>(i,j) = 0;
                    positions.insert(i*img_width + j);
                }
            }
        }
        positions.erase(p);
    }
}

void isauvola(cv::Mat im_gray, cv::Mat &dest, cv::Size window_size, double k){
    cv::Mat high_contrast_image(im_gray.rows, im_gray.cols, CV_8UC1, cv::Scalar(255));
    highContrastImage(im_gray, high_contrast_image);
    cv::Mat sauvola_image(im_gray.rows, im_gray.cols, CV_8UC1, cv::Scalar(0));
    sauvola(im_gray, sauvola_image, window_size, k);

    // Sequential Combination;
    int img_rows = im_gray.rows;
    int img_cols = im_gray.cols;
    uchar *high_contrast_ptr;
    uchar *sauvola_contrast_ptr;
    uchar *dest_ptr;
    for(int i=0; i< img_rows; i++){
        high_contrast_ptr = high_contrast_image.ptr(i);
        sauvola_contrast_ptr= sauvola_image.ptr(i);
        dest_ptr = dest.ptr(i);
        for(int j=0; j< img_cols ; j++){
            if(high_contrast_ptr[j]==255 && sauvola_contrast_ptr[j]==0){
                if(dest_ptr[j]==255){
                    spider(sauvola_image, dest, std::make_pair(i, j));
                }
            }
        }
    }
}
}
