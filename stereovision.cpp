#include "stereovision.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>
#include <bitset>

#define max(X,Y) ((X>Y)?X:Y)
#define min(X,Y) ((X>Y)?Y:X)
//#define min(w,x,y,z) (min(w,min(x,min(y,min(z)))))

typedef std::function<void(int,int)> ParallelDisparityFunction;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<double,std::milli> Duration;

StereoVision::StereoVision() :
    image_cb(nullptr),
    neighbor(3),
    window_size(7),
    max_disparity(50),
    disparity_step(1),
    match(SAD_MATCH)
{
#if 0
    std::bitset<32> test1("10010110101111001");
    std::bitset<32> test2("00010111101010001");
    std::cout << "test1 : " << test1 << " count : " << test1.count() << '\n'
              << "test2 : " << test2 << " count : " << test2.count() << '\n'
              << "test1 ^ test2 : " << (test1^test2) << '\n';
#endif
}

bool StereoVision::setFiles(std::__cxx11::string left_, std::__cxx11::string right_)
{
    left_dir = left_;
    right_dir = right_;
    left = cv::imread(left_dir);
    right = cv::imread(right_dir);
    cv::cvtColor(left,left_gray,CV_RGB2GRAY);
    cv::cvtColor(right,right_gray,CV_RGB2GRAY);
    if(left.empty() || right.empty())
        return false;
    if(image_cb)
        image_cb(left,right);
    std::cout << "left : " << imageType(left) << '\n'
              << "right : " << imageType(right) << '\n'
              << "left gray : " << imageType(left_gray) << '\n'
              << "right gray : " << imageType(right_gray) << '\n';
//    auto disp = disparity();
//    cv::imshow("disparity",disp);
//    cv::imshow("gray ref",left_gray);
//    cv::imshow("gray match", right_gray);
    this->matching_cost.clear();
    this->sum_cost.clear();
    this->matching_cost.resize(left_gray.rows);
    this->sum_cost.resize(left_gray.rows);
    for(auto& w : matching_cost)
    {
        w.resize(left_gray.cols);
        for(auto& d : w)
            d.resize(max_disparity);
    }
    for(auto& w : sum_cost)
    {
        w.resize(left_gray.cols);
        for(auto& d : w)
            d.resize(max_disparity);
    }
    return true;
}

void StereoVision::connectImageCallback(StereoVision::ImageCallback im_cb_)
{
    image_cb = im_cb_;
}

void StereoVision::setMaxDisparity(int max_)
{
    max_disparity = max_;
}

void StereoVision::setWindow(int neighbor_)
{
    neighbor = neighbor_;
}

void StereoVision::setDisparityStep(int step)
{
    disparity_step = step;
}

void StereoVision::setMatchingMethod(std::__cxx11::string match_)
{
    if(match_ == std::string("CENSUS"))
        match = CENSUS_MATCH;
    else if(match_ == std::string("SAD"))
        match = SAD_MATCH;
}

std::vector<int> StereoVision::matchCost(int row, int col)
{
    std::vector<int> cost;
    if((row<=left_gray.rows) || (col<=left_gray.cols))
        for(int i=0; i<max_disparity; i++)
            cost.push_back(matching_cost[row][col][i]);
    return cost;
}

StereoVision::disparity_fn::disparity_fn(cv::Mat &left, cv::Mat &right, cv::Mat &disp_, StereoVision *stereo_)
    : left_gray(left),
      right_gray(right),
      disp(disp_),
      stereo(stereo_)
{

}

void StereoVision::disparity_fn::f(int start, int end, MatchingMethod match)
{
    switch (match) {
    case SAD_MATCH:
    {
        std::vector<uchar *> row_ref, row_match;
        int row_window_sum[left_gray.cols][stereo->max_disparity][stereo->window_size-1];
        int row_window_sum_index = 0;
        for(int i=start; i<end; i++)
        {
            row_ref.clear();
            row_match.clear();
            for(int w=(i-stereo->neighbor); w<=(i+stereo->neighbor); w++)
            {
                row_ref.push_back(left_gray.ptr<uchar>(w));
                row_match.push_back(right_gray.ptr<uchar>(w));
            }

            if(i==start)
                for(int j=stereo->neighbor; j<(left_gray.cols-stereo->neighbor); j++)
                {
                    int sad_min = 10000000;
                    int dmax = min(j,stereo->max_disparity);
                    int d_true = 0;
                    for(int d=0; d<dmax; d+=stereo->disparity_step)
                    {
                        int sad = 0;
                        for(size_t k=0; k<row_ref.size(); k++)
                        {
                            int row_sum = 0;
                            for(int l=(-stereo->neighbor); l<=(stereo->neighbor); l++)
                            {
                                auto ad = std::abs(row_ref[k][j+l] - row_match[k][j+l-d]);
                                sad += ad;
                                row_sum += ad;
                            }
                            if(k<(stereo->window_size) && k>0)
                                row_window_sum[j][d][k-1] = row_sum;
                        }
                        stereo->matching_cost[i][j][d] = sad;
                        if(sad<sad_min)
                        {
                            sad_min = sad;
                            d_true = d;
                        }
                    }
                    disp.data[i*disp.cols+j] = (d_true);
                }

            else
                for(int j=stereo->neighbor; j<(left_gray.cols-stereo->neighbor); j++)
                {
                    int sad_min = 10000000;
                    int dmax = min(j,stereo->max_disparity);
                    int d_true = 0;

                    for(int d=0; d<dmax; d+=stereo->disparity_step)
                    {
                        int sad = 0;
                        for(size_t k=0; k<row_ref.size(); k++)
                            sad += row_window_sum[j][d][k];

                        row_window_sum[j][d][row_window_sum_index] = 0;

                        int k = row_ref.size()-1;
                        for(int l=(-stereo->neighbor); l<=(stereo->neighbor); l++)
                        {
                            int ad = std::abs(row_ref[k][j+l] - row_match[k][j+l-d]);
                            row_window_sum[j][d][row_window_sum_index] += ad;
                            sad += ad;
                        }
                        stereo->matching_cost[i][j][d] = sad;
                        if(sad<sad_min)
                        {
                            sad_min = sad;
                            d_true = d;
                        }
                    }
                    disp.data[i*disp.cols+j] = (d_true);
                }
            row_window_sum_index++;
            if(row_window_sum_index>=stereo->window_size)
                row_window_sum_index = 0;
        }
    }
        break;
    case CENSUS_MATCH:
    {
        std::vector<uchar*>row_ref, row_match;
        std::vector<std::bitset<512>> match_bits;
        for(int i=0; i<stereo->max_disparity/stereo->disparity_step; ++i)
            match_bits.push_back(std::bitset<512>());
        int match_bits_index(0);
        for(int i=start; i<end; i++)
        {
            row_ref.clear();
            row_match.clear();
            for(int w=(i-stereo->neighbor); w<=(i+stereo->neighbor); w++)
            {
                row_ref.push_back(left_gray.ptr<uchar>(w));
                row_match.push_back(right_gray.ptr<uchar>(w));
            }
            match_bits_index = 0;
            for(int j=stereo->neighbor; j<(left_gray.cols); ++j)
            {
                std::bitset<512> ref_bits, m_bits;
                int distance_min(512);
                int dmax = min(j,stereo->max_disparity);
                auto window = row_ref.size();
                auto row = stereo->neighbor;
                for(size_t k=0; k<window; k++)
                    for(int l=(-stereo->neighbor); l<=(stereo->neighbor); l++)
                        ref_bits[k*window+l+stereo->neighbor] = (row_ref[row][j] > row_ref[k][j+l]);
//                if(j==stereo->neighbor)
                    for(int d=0; d<dmax; d+=stereo->disparity_step)
                    {
                        for(size_t k=0; k<stereo->neighbor; k++)
                            for(int l=(-stereo->neighbor); l<=0; l++)
                            {
                                m_bits[k*window+l+stereo->neighbor] =
                                        (row_match[row][j-d] > row_match[k][j-d+l]);
                                m_bits[k*2*window+l+stereo->neighbor] =
                                        (row_match[row][j-d] > row_match[k*2][j-d+l]);
                                m_bits[k*window-l+stereo->neighbor] =
                                        (row_match[row][j-d] > row_match[k][j-d-l]);
                                m_bits[k*2*window-l+stereo->neighbor] =
                                        (row_match[row][j-d] > row_match[k*2][j-d-l]);
                            }
//                        match_bits[d/stereo->disparity_step] = m_bits;
                        auto diff = ref_bits;
                        diff ^= m_bits;
                        int hamming_dis = diff.count();
                        stereo->matching_cost[i][j][d] = hamming_dis;
                        if(hamming_dis<distance_min)
                        {
                            distance_min = hamming_dis;
                            disp.data[i*disp.cols+j] = d;
                        }
                    }
//                else
//                    for(int d=0; d<dmax; d+=stereo->disparity_step)
//                    {
//                        int idx = d/stereo->disparity_step-(1+match_bits_index);
//                        if(idx>=match_bits.size())
//                            idx = 0;
//                        m_bits = match_bits[idx];
//                        if(d>=dmax-stereo->disparity_step)
//                        {
//                            for(size_t k=0; k<window; k++)
//                                for(int l=(-stereo->neighbor); l<=(stereo->neighbor); l++)
//                                    m_bits[k*window+l+stereo->neighbor] =
//                                            (row_match[row][j-d] > row_match[k][j-d+l]);
//                            match_bits[d/stereo->disparity_step + match_bits_index] = m_bits;
//                            match_bits_index++;
//                            if(match_bits_index>=match_bits.size())
//                                match_bits_index = 0;
//                        }
//                        auto diff = ref_bits;
//                        diff ^= m_bits;
//                        int hamming_dis = diff.count();
//                        if(hamming_dis<distance_min)
//                        {
//                            distance_min = hamming_dis;
//                            disp.data[i*disp.cols+j] = d;
//                        }
//                    }

                /*
                if(j==stereo->neighbor)
                {
                    match_bits_index = 0;
                    for(int d=0; d<dmax; d+=stereo->disparity_step)
                    {
                        for(size_t k=0; k<window; k++)
                            for(int l=(-stereo->neighbor); l<=(stereo->neighbor); l++)
                                match_bits[d/stereo->disparity_step][k*window+l+stereo->neighbor] =
                                        (row_match[row][j-d] > row_match[k][j+l-d]);
                        auto diff = ref_bits;
                        diff ^= match_bits[d/stereo->disparity_step];
                        int hamming_dis = diff.count();
                        if(hamming_dis<distance_min)
                        {
                            distance_min = hamming_dis;
                            disp.data[i*disp.cols+j] = d;
                        }
                    }
                }
                else
                {
                    for(int d=0; d<dmax; d+=stereo->disparity_step)
                    {
                        auto diff = ref_bits;
                        if(d>=(dmax-stereo->disparity_step))
                        {
                            for(size_t k=0; k<window; k++)
                                for(int l(-stereo->neighbor); l<=(stereo->neighbor); ++l)
                                    match_bits[match_bits_index][k*window+l+stereo->neighbor] =
                                            (row_match[row][j-d] > row_match[k][j+l-d]);
                            ++match_bits_index;
                            if(match_bits_index>=(stereo->max_disparity/stereo->disparity_step))
                                match_bits_index=0;
                        }
                        int match_index = match_bits_index + d/stereo->disparity_step;
                        if(match_index>=stereo->max_disparity/stereo->disparity_step)
                            match_index -= stereo->max_disparity/stereo->disparity_step;
                        diff ^= match_bits[match_index];
                        int hamming_dis = diff.count();
                        if(hamming_dis<distance_min)
                        {
                            distance_min = hamming_dis;
                            disp.data[i*disp.cols+j] = d;
                        }
                    }
                }
                */
            }
        }
    }
    default:
        break;
    }
}

cv::Mat StereoVision::disparity()
{
    auto t0 = Clock::now();
    cv::Mat disp = cv::Mat::zeros(left.rows,left.cols,CV_8UC1);
    window_size = 2*neighbor+1;
    auto row_index = neighbor;

    int num_cpu = std::thread::hardware_concurrency();
    std::vector<std::thread> ft(num_cpu);

#if 0
    std::vector<ParallelDisparityFunction> f(num_cpu);
    for(size_t i=0; i<f.size(); ++i)
    {
        f[i] = [&](int start, int end)
        {
            std::vector<uchar *> row_ref, row_match;
            int row_window_sum[left_gray.cols][max_disparity][window_size-1];
            int row_window_sum_index = 0;
            for(int i=start; i<end; i++)
            {
                row_ref.clear();
                row_match.clear();
                for(int w=(i-neighbor); w<=(i+neighbor); w++)
                {
                    row_ref.push_back(left_gray.ptr<uchar>(w));
                    row_match.push_back(right_gray.ptr<uchar>(w));
                }

                if(i==start)
                    for(int j=neighbor; j<(left_gray.cols-neighbor); j++)
                    {
                        int sad_min = 10000000;
                        int dmax = min(j,max_disparity);
                        int d_true = 0;
                        for(int d=0; d<dmax; d+=disparity_step)
                        {
                            int sad = 0;
                            for(int k=0; k<row_ref.size(); k++)
                            {
                                int row_sum = 0;
                                for(int l=0; l<window_size; l++)
                                {
                                    auto ad = std::abs(row_ref[k][j+l] - row_match[k][j+l-d]);
                                    sad += ad;
                                    row_sum += ad;
                                }
                                if(k<(window_size) && k>0)
                                    row_window_sum[j][d][k-1] = row_sum;
                            }
                            if(sad<sad_min)
                            {
                                sad_min = sad;
                                d_true = d;
                            }
                        }
                        disp.data[i*disp.cols+j] = (d_true);
                    }

                else
                    for(int j=neighbor; j<(left_gray.cols-neighbor); j++)
                    {
                        int sad_min = 10000000;
                        int dmax = min(j,max_disparity);
                        int d_true = 0;

                        for(int d=0; d<dmax; d+=disparity_step)
                        {
                            int sad = 0;
                            for(int k=0; k<row_ref.size(); k++)
                                sad += row_window_sum[j][d][k];

                            row_window_sum[j][d][row_window_sum_index] = 0;

                            int k = row_ref.size()-1;
                            for(int l=0; l<window_size; l++)
                            {
                                int ad = std::abs(row_ref[k][j+l] - row_match[k][j+l-d]);
                                row_window_sum[j][d][row_window_sum_index] += ad;
                                sad += ad;
                            }

                            if(sad<sad_min)
                            {
                                sad_min = sad;
                                d_true = d;
                            }
                        }
                        disp.data[i*disp.cols+j] = (d_true);
                    }
                row_window_sum_index++;
                if(row_window_sum_index>=window_size)
                    row_window_sum_index = 0;
            }
        };
    }
#endif
    int range = (left_gray.rows-2*neighbor)/num_cpu;

    std::vector<disparity_fn> disp_fn;
    for(size_t i=0; i<ft.size(); ++i)
    {
        disp_fn.push_back(disparity_fn(left_gray,right_gray,disp,this));
        auto start = i*range+row_index;
        auto end = start+range;
        if(i==(ft.size()-1))
            end = (left_gray.rows-neighbor);
#if 0
        ft[i] = std::thread(f[i],start,end);
#endif
        ft[i] = std::thread(std::bind(&disparity_fn::f,disp_fn[i],start,end,match));
    }

    for(int i=0; i<ft.size(); ++i)
    {
        if(ft[i].joinable())
            ft[i].join();
    }

    auto t_end = Clock::now();
    Duration dt = t_end-t0;
    std::cout << "time : " << dt.count() << " ms\n";
    return disp;
}

inline
void StereoVision::SGMMatch()
{
    //path accumulation
    auto path_acc = [&](int row, int col, int d, int rx, int ry, int& cost)
    {
        if(row>=0 && row<left_gray.rows)
            if(col>=0 && col<left_gray.cols)
                if(d>=0 && d<max_disparity)
                {
                    cost += matching_cost[row][col][d];
                }
    };


    //forward
//    int lr[3][left_gray.cols][max_disparity];
//    for(int i=0; i<left_gray.rows; i++)
//        for(int j=0; j<left_gray.cols; j++)
//            for(int d=0; d<dmax; d++)
//            {
//                if(i==0)
//                {
//                    if(d==0)
//                    {

//                    }
//                }
//            }
}

std::__cxx11::string StereoVision::imageType(const cv::Mat &mat)
{
    std::string ret;
    auto type = mat.type();
    auto depth = type & CV_MAT_DEPTH_MASK;
    auto channels = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U:
        ret += "8UC";
        break;
    case CV_8S:
        ret += "8SC";
        break;
    case CV_16U:
        ret += "16UC";
        break;
    case CV_16S:
        ret += "16SC";
        break;
    case CV_32S:
        ret += "32SC";
        break;
    case CV_32F:
        ret += "32FC";
        break;
    case CV_64F:
        ret += "64FC";
        break;
    default:
        ret += "userC";
        break;
    }
    ret += (channels+'0');

    return ret;
}
