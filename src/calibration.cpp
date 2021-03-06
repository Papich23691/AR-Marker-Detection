
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc.hpp"

#include "calibration.h"

#include <sstream>
#include <iostream>
#include <fstream>

extern cv::VideoCapture vid;

#define MAX 25
#define FPS 120

/**
 * Finding the physical position of each corner using the known square size
 */
void create_known(cv::Size board, float square_length,std::vector<cv::Point3f> &corners)
{
    for(int i=0;i<board.height; i++)
    {
        for(int j=0;j<board.width; j++)
        {
            corners.push_back(cv::Point3f(j * square_length , i*square_length,0.0f));
        }
    }
}

/**
 * Using the physical positions and the positions in the image as
 * arguments in OpenCV's calibration function
 */
void camera_cal(std::vector<cv::Mat> cal_img,cv::Size board,float square, cv::Mat& cam_mat,cv::Mat &distortion)
{
    std::vector<std::vector<cv::Point2f>> cheackboard_img_points;
    get_corners(cal_img,cheackboard_img_points,false);

    std::vector<std::vector<cv::Point3f>> space_corners_points(1);

    create_known(board,square,space_corners_points[0]);

    space_corners_points.resize(cheackboard_img_points.size(),space_corners_points[0]);

    std::vector<cv::Mat> rVectors, tVectors;
    distortion = cv::Mat::zeros(8,1,CV_64F);

    cv::calibrateCamera(space_corners_points,cheackboard_img_points,board,cam_mat,distortion,rVectors,tVectors);

}

/**
 * Using OpenCV detection function in order to find the chessboard corners in image matrix (Position in the image)
 */
void get_corners(std::vector<cv::Mat> img , std::vector<std::vector<cv::Point2f>> &found_cor, bool show)
{
    for (std::vector<cv::Mat>::iterator i = img.begin(); i !=img.end() ; i++)
    {
        std::vector<cv::Point2f> points;
        bool found = cv::findChessboardCorners(*i,cv::Size(9,6),points,cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_ADAPTIVE_THRESH);

        if (found)
            found_cor.push_back(points);
        
    }
}

/**
 * Saving the camera matrix as well as the camera distortion matrix
 * as ./Calibration
 */
bool save_cal(std::string name, cv::Mat cam_mat, cv::Mat distortion)
{
    std::ofstream out(name);
    if (out)
    {
        uint16_t rows = cam_mat.rows;
        uint16_t cols = cam_mat.cols;
    
        printf("%u / %u",rows,cols);

        out << rows << std::endl;
        out << cols << std::endl;
        
        for (int r=0;r<rows;r++)
        {
            for (int c=0;c<cols;c++)
            {
                double val = cam_mat.at<double>(r,c);
                out << val << std::endl;
            }
        }
        rows = distortion.rows;
        cols = distortion.cols;

        out << rows << std::endl;
        out << cols << std::endl;

        for (int r=0;r<rows;r++)
        {
            for (int c=0;c<cols;c++)
            {
                double val = cam_mat.at<double>(r,c);
                out << val << std::endl;
            }
        }
        out.close();
    }
    return true;
}

/**
 * Loading the ./Calibration file
 */
bool load_cal (std::string name, cv::Mat &cam_mat, cv::Mat &distortion){
    std::ifstream in(name);
    if (in)
    {
        uint16_t rows;
        uint16_t cols;

        in >> rows;
        in >> cols; 
        
        cam_mat = cv::Mat(cv::Size(cols,rows),CV_64F);

        for (int r=0;r<rows;r++)
        {
            for (int c=0;c<cols;c++)
            {
                double read = 0.0f;
                in >> read;
                cam_mat.at<double>(r,c) = read;
                printf("%f\n",cam_mat.at<double>(r,c));
            }
        }

        in >> rows;
        in >> cols; 

        distortion = cv::Mat::zeros(rows,cols,CV_64F);

        for (int r=0;r<rows;r++)
        {
            for (int c=0;c<cols;c++)
            {
                double read = 0.0f;
                in >> read;
                distortion.at<double>(r,c) = read;
                printf("%f\n",distortion.at<double>(r,c));
            }
        }
        in.close();
        return true;
    }
    return false;
}

/**
 * Function created to easily calibrate the camera in real time
 * Saving images in matrix vector and using them to calibrate the camera
 */
bool camera_cal_real_time(cv::Mat &cam_mat,cv::Mat &distortion,float square_dim)
{
    cv::Mat frame;
    cv::Mat draw;
    std::string text = "0";
    cv::Scalar color;
    std::vector<cv::Mat> saved;
    std::vector<std::vector<cv::Point2f>> corners,rejected;

    if (!vid.isOpened())
        return false;

    cv::namedWindow("CAM",cv::WINDOW_AUTOSIZE);
    while(1)
    {   
        text = std::to_string(saved.size());
        if (saved.size() > MAX)
        {
            text.append(" - Press Enter to save");
            color = *(new cv::Scalar(0,255,0,255));
        }
        else
            color = *(new cv::Scalar(0,0,255,255));
        

        if (!vid.read(frame))
            break;
            
        std::vector<cv::Vec2f> found_points;
        bool found = false;
        found = cv::findChessboardCorners(frame,cv::Size(9,6),found_points,cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE |cv::CALIB_CB_FAST_CHECK);
        frame.copyTo(draw);
        cv::drawChessboardCorners(draw,cv::Size(9,6),found_points,found);
        
        if (found){
            cv::putText(draw, text , cv::Point2f(100,100), 0, 2,color,6);
            cv::imshow("CAM",draw);
        }
        else
        {
            cv::putText(frame, text , cv::Point2f(100,100), 0, 2,color,6);
            cv::imshow("CAM",frame);
        }
        char c = cv::waitKey(1000/FPS);

        switch (c)
        {
            case ' ':
                if (found)
                {
                    std::ostringstream convert;
                    cv::Mat tmp;
                    frame.copyTo(tmp);
                    saved.push_back(tmp);
                }
                break;
            case 13:
                if (saved.size() > MAX)
                {
                    camera_cal(saved,cv::Size(9,6),square_dim,cam_mat,distortion);
                    save_cal("../Calibration",cam_mat,distortion);
                }
                break;
            case 27:
                return true;
        }
    }
    return true;
}
