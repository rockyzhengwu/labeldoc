#include "msertextdetector.h"
//#include "erfilter.hpp"

#include <vector>
#include <QDebug>

#include <iostream>
#include <set>
namespace ocrmodel {

MserTextDetector::MserTextDetector()
{
    theta1_ = 0.01;
    theta2_ = 0.35;
    maxA_ = 1.1;
    minA_ = 0.3;
}


void MserTextDetector::drawEllipse(const MSER::Region &region, int width, int height, int depth,
                 cv::Mat img, cv::Scalar color)
{
    // Centroid (mean)
    const double x = region.moments_[0] / region.area_;
    const double y = region.moments_[1] / region.area_;

    // Covariance matrix [a b; b c]
    const double a = region.moments_[2] / region.area_ - x * x;
    const double b = region.moments_[3] / region.area_ - x * y;
    const double c = region.moments_[4] / region.area_ - y * y;

    // Eigenvalues of the covariance matrix
    const double d  = a + c;
    const double e  = a - c;
    const double f  = sqrt(4.0 * b * b + e * e);
    const double e0 = (d + f) / 2.0; // First eigenvalue
    const double e1 = (d - f) / 2.0; // Second eigenvalue

    // Desired norm of the eigenvectors
    const double e0sq = sqrt(e0);
    const double e1sq = sqrt(e1);
    // Eigenvectors
    double v0x = e0sq;
    double v0y = 0.0;
    double v1x = 0.0;
    double v1y = e1sq;

    if (b) {
        v0x = e0 - c;
        v0y = b;
        v1x = e1 - c;
        v1y = b;

        // Normalize the eigenvectors
        const double n0 = e0sq / sqrt(v0x * v0x + v0y * v0y);
        v0x *= n0;
        v0y *= n0;

        const double n1 = e1sq / sqrt(v1x * v1x + v1y * v1y);
        v1x *= n1;
        v1y *= n1;
    }

    for (double t = 0.0; t < 2.0 * M_PI; t += 0.001) {
        int x2 = x + (cos(t) * v0x + sin(t) * v1x) * 2.0 + 0.5;
        int y2 = y + (cos(t) * v0y + sin(t) * v1y) * 2.0 + 0.5;

        if ((x2 >= 0) && (x2 < width) && (y2 >= 0) && (y2 < height)){
            cv::Vec3b *ptr = img.ptr<cv::Vec3b>(y2);
            ptr[x2][0] = 0;
            ptr[x2][1] = 0;
            ptr[x2][2] = 255;
        }

    }
}
double MserTextDetector::getNormVariation(MSER::Region *region){
    double v = region->variation_;
    double aspect =  (double)region->boundRect_.width / (double)region->boundRect_.height;
//    qDebug() <<"aspect: " << aspect << " variation " << region->variation_ << " stable "<< region->stable_;
    if(aspect > maxA_){
        v = v + theta1_*(aspect - maxA_);
    }else if(aspect < minA_){
        v = v + theta2_ * (minA_ - aspect);
    }
    return  v;
}

MSER::Region *MserTextDetector::linearReduction(MSER::Region *root){
    if(!root->child_ ){
        return root;
    }

    if(!root->child_->next_){
        MSER::Region* c = linearReduction(root->child_);
        if(getNormVariation(root) < getNormVariation(c)){
            root->child_ = c->child_;
            return root;
        }
        else{
            return c;
        }
    }
    else{
        MSER::Region *ch = root->child_;
        root->child_ = nullptr;
        while (ch) {
            if(!ch->stable_){
                ch = ch->next_;
                continue;
            }
            MSER::Region *r  = linearReduction(ch);
            ch = ch->next_;
            r->next_= root->child_;
            root->child_ = r;
        }
        return  root;
    }
}

std::vector<MSER::Region> MserTextDetector::removeDuplicate(std::vector<MSER::Region> regions){
    std::sort(regions.begin(), regions.end(), [](const MSER::Region &r1, const MSER::Region &r2){return  r1.area_ > r2.area_ ;});
    int i;
    std::vector<MSER::Region> newRegions;
    std::set<int> pixelSets;
    for(i=0; i<regions.size(); i++){
        MSER::Region root = regions[i];
        if(pixelSets.find(root.pixel_)!=pixelSets.end()){
            continue;
        }
        double aspect = ((double)root.boundRect_.width) / ((double)root.boundRect_.height);
        if(aspect > 2.0  || aspect < 0.1  ){
            continue;
        }
        std::vector<MSER::Region *> childs;
        childs.push_back(&root);
        MSER::Region *node;
        MSER::Region *ch;
        while (!childs.empty()) {
            node = childs.back();
            childs.pop_back();
            pixelSets.insert(node->pixel_);
            ch = node->child_;
            while (ch) {
                childs.push_back(ch);
                pixelSets.insert(ch->pixel_);
                ch = ch->next_;
            }
        }
        cleanRegion(&root);
        newRegions.push_back(root);
    }
    return newRegions;
}

void MserTextDetector::cleanRegion(MSER::Region *root){
    MSER::Region *ch = root->child_;
    int c = 0;
    while (ch) {
        c += 1;
        if(ch->stable_){
            qDebug()<<"stable var:" << ch->variation_;
        }
        ch = ch->next_;
    }
    qDebug()<< "child size " << c;
}

std::vector<MSER::Region> MserTextDetector::treeReduction(MSER::Region* root){
    std::vector<MSER::Region> words;
    if(!root->child_){
        words.push_back(*root);
        return words;
    }

    if(!root->child_->next_ ){
//        qDebug()<<"imposiable";
    }

    MSER::Region *ch;
    ch = root->child_;
    while (ch) {
        if(!ch->stable_){
            ch = ch->next_;
            continue;
        }
        std::vector<MSER::Region> res = treeReduction(ch);
        qDebug() << "res size " << res.size();
        words.insert(words.end(), res.begin(), res.end());
        ch = ch->next_;
    }
    double minV = 1000000;
    for(int i=0; i<words.size(); i++){
        double v = getNormVariation(&words[i]);
        if(v < minV){
            minV = v;
        }
    }
    qDebug() << getNormVariation(root) << " min "  << minV;
    if(words.size()==0){
        words.push_back(*root);
        return words;
    }
    qDebug() << "root: " ;
    if(getNormVariation(root) <= minV ){
        words.clear();
        words.push_back(*root);
        return words;
    }else{
        qDebug() <<"user return children";
        return words;
    }
}


void MserTextDetector::getmserRegions(cv::Mat image){
    int i, j;
    int width = image.size().width;
    int height = image.size().height;

    cv::Mat gray ;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    MSER mser8(2, 0.00005, 0.005, 0.5, 0.5, true);
    std::vector<MSER::Region> regions;
    mser8.getRegions(gray, regions);

    std::vector<MSER::Region> trees = removeDuplicate(regions);
    qDebug() << "start reduction n" << trees.size() ;

//    return ;
//    for(int i=0; i<trees.size() ; i++){
//        trees[i] = *(linearReduction(&trees[i]));
//    }

//    std::vector<MSER::Region> candWords ;
//    for(int i=0; i<trees.size(); i++){
//        std::vector<MSER::Region> tmp = treeReduction(&trees[i]);
//        candWords.insert(candWords.end(), tmp.begin(), tmp.end());
//    }
//    qDebug() << "start draw";
//    cv::Mat vis = image.clone();
//    for(i=0; i<candWords.size(); i++){
//        drawEllipse(candWords[i], width, height, 3, vis, {0, 255, 255});
//    }
//    for(int i=0; i< regions.size(); i++){
////        qDebug() << "regions stable" << regions[i].stable_ << " variation " << regions[i].variation_;
//    }

    for(i=0; i<trees.size(); i++){
        drawEllipse(trees[i], width, height, 3, image, {0, 255, 255});
    }

    cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/mser.png", image);
//    cv::imwrite("/Users/zhengwu/workspace/qtprojects/github/mser_reduction.png", vis);
//    qDebug() << "regions size: " << candWords.size();
}
}
