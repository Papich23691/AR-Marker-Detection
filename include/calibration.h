#ifndef CAL
#define CAL

#include "opencv2/core.hpp"

#include <iostream>


void create_known(cv::Size board, float square_length,std::vector<cv::Point3f> &corners);
void get_corners(std::vector<cv::Mat> img , std::vector<std::vector<cv::Point2f>> &found_cor, bool show=false);
void camera_cal(std::vector<cv::Mat> cal_img,cv::Size board,float square, cv::Mat& cam_mat,cv::Mat &distortion);
bool save_cal(std::string name, cv::Mat cam_mat, cv::Mat distortion);
bool load_cal (std::string name, cv::Mat &cam_mat, cv::Mat &distortion);
bool camera_cal_real_time(cv::Mat &cam_mat,cv::Mat &distortion,float square_dim);

#endif