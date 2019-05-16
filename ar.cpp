#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <sstream>
#include <iostream>
#include <fstream>

const float square_dim = 0.026f;

void create_aruco_markers(){
    cv::Mat out_marker;
    cv::Ptr<cv::aruco::Dictionary> markerdic = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

    for (int i=0;i<50;i++){
        cv::aruco::drawMarker(markerdic,i,500,out_marker,1);
        std::ostringstream convert;
        std::string img_name = "4x4Marker_";
        convert << img_name << i << ".jpg";
        cv::imwrite(convert.str(),out_marker);
    }

}

int monitor(const cv::Mat &cam_mat,const cv::Mat &distance,float square_dim){
    cv::Mat frame;
    std::vector<int> marker_id;
    std::vector<std::vector<cv::Point2f>> corners, rejected;
    cv::aruco::DetectorParameters params;

    cv::Ptr<cv::aruco::Dictionary> marker_dic = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

    cv::VideoCapture vid(0);

    if (!vid.isOpened())
        return -1;

    cv::namedWindow("Cam",cv::WINDOW_AUTOSIZE);

    std::vector<cv::Vec3d> rotate,translated;

    while(1)
    {
        if (!vid.read(frame))
            break;
        cv::aruco::detectMarkers(frame,marker_dic,corners,marker_id);
        cv::aruco::estimatePoseSingleMarkers(corners, square_dim,cam_mat,distance,rotate,translated);
        for (int i=0;i<marker_id.size();i++){
            cv::aruco::drawAxis(frame,cam_mat,distance,rotate[i],translated[i],0.3f);
        }
        cv::imshow("Cam",frame);
        if (cv::waitKey(30) >= 0) break;
        
    }
    return 1;
}

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

void get_corners(std::vector<cv::Mat> img , std::vector<std::vector<cv::Point2f>> &found_cor, bool show=false)
{
    for (std::vector<cv::Mat>::iterator i = img.begin(); i !=img.end() ; i++)
    {
        std::vector<cv::Point2f> points;
        bool found = cv::findChessboardCorners(*i,cv::Size(9,6),points,cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_ADAPTIVE_THRESH);

        if (found)
            found_cor.push_back(points);
        
    }
}

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

bool camera_cal_real_time(cv::Mat &cam_mat,cv::Mat &distortion)
{
    cv::Mat frame;
    cv::Mat draw;

    std::vector<cv::Mat> saved;
    std::vector<std::vector<cv::Point2f>> corners,rejected;
    cv::VideoCapture vid(0);

    if (!vid.isOpened())
        return false;
    int fps = 60;
    cv::namedWindow("Cam",cv::WINDOW_AUTOSIZE);
    while(1)
    {
        if (!vid.read(frame))
            break;

        std::vector<cv::Vec2f> found_points;
        bool found = false;
        found = cv::findChessboardCorners(frame,cv::Size(9,6),found_points,cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE |cv::CALIB_CB_FAST_CHECK);
        frame.copyTo(draw);
        cv::drawChessboardCorners(draw,cv::Size(9,6),found_points,found);
        if (found)
            cv::imshow("CAM",draw);
        else
            cv::imshow("CAM",frame);
        char c = cv::waitKey(1000/fps);

        switch (c)
        {
            case ' ':
                if (found)
                {
                    cv::Mat tmp;
                    frame.copyTo(tmp);
                    saved.push_back(tmp);
                }
                break;
            case 13:
                if (saved.size() >15)
                {
                    camera_cal(saved,cv::Size(9,6),square_dim,cam_mat,distortion);
                    save_cal("Calibration",cam_mat,distortion);
                }
                break;
            case 27:
                return true;
        }
    }
    return true;
}

int main(){
    cv::Mat cam_mat= cv::Mat::eye(3,3,CV_64F);

    cv::Mat distortion;
    char c;
    std::cin  >> c ; 
    if (c=='a')
        camera_cal_real_time(cam_mat,distortion);
    else{
        load_cal("Calibration",cam_mat,distortion);
        monitor(cam_mat,distortion,0.57f);
    }

    return 0;
}