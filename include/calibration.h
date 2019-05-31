#ifndef CAL
#define CAL

#include "opencv2/core.hpp"

#include <iostream>

/**
 * Finding the physical position of each corner using the known square size
 */
void create_known(cv::Size board, float square_length,std::vector<cv::Point3f> &corners);

/**
 * Using OpenCV detection function in order to find the chessboard corners in image matrix (Position in the image)
 */
void get_corners(std::vector<cv::Mat> img , std::vector<std::vector<cv::Point2f>> &found_cor, bool show=false);

/**
 * Using the physical positions and the positions in the image as
 * arguments in OpenCV's calibration function
 */
void camera_cal(std::vector<cv::Mat> cal_img,cv::Size board,float square, cv::Mat& cam_mat,cv::Mat &distortion);

/**
 * Saving the camera matrix as well as the camera distortion matrix
 * as ./Calibration
 */
bool save_cal(std::string name, cv::Mat cam_mat, cv::Mat distortion);

/**
 * Loading the ./Calibration file
 */
bool load_cal (std::string name, cv::Mat &cam_mat, cv::Mat &distortion);

/**
 * Function created to easily calibrate the camera in real time
 * Saving images in matrix vector and using them to calibrate the camera
 */
bool camera_cal_real_time(cv::Mat &cam_mat,cv::Mat &distortion,float square_dim);

#endif