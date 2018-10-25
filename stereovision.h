#ifndef STEREOVISION_H
#define STEREOVISION_H

#include <string>
#include <functional>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class StereoVision
{
public:
    typedef std::function<void(const cv::Mat&,const cv::Mat&)> ImageCallback;
    typedef std::function<void(const cv::Mat&)> DisparityCallback;
    typedef enum {
        SAD_MATCH,
        CENSUS_MATCH,
    } MatchingMethod;
    typedef std::vector<std::vector<std::vector<int>>> MatchingCostVector;
public:
    StereoVision();
    bool setFiles(std::string left_, std::string right_);
    void connectImageCallback(ImageCallback im_cb_);
    void setMaxDisparity(int max_);
    void setWindow(int neighbor_);
    void setDisparityStep(int step);
    void setMatchingMethod(std::string match_);
    std::vector<int> matchCost(int row, int col);
    cv::Mat disparity();
private:
    std::string left_dir, right_dir;
    cv::Mat left, right;
    cv::Mat left_gray, right_gray;
    ImageCallback image_cb;
    MatchingMethod match;
    int neighbor;
    int window_size;
    int max_disparity;
    int disparity_step;
    MatchingCostVector matching_cost;
    MatchingCostVector sum_cost;
    struct disparity_fn
    {
        disparity_fn(cv::Mat& left, cv::Mat& right, cv::Mat& disp_, StereoVision* stereo_);
        void f(int start, int end, MatchingMethod match = SAD_MATCH);
        cv::Mat& left_gray, right_gray;
        cv::Mat& disp;
        StereoVision *stereo;
    };

private:
    void SGMMatch();
    std::string imageType(const cv::Mat& mat);
};

#endif // STEREOVISION_H
