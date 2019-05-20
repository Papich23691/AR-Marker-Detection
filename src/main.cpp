#include <iostream>

#include "calibration.h"
#include "marker.h"

#include "opencv2/highgui.hpp"

#include <GLUT/glut.h>

int main(int argc,char **argv){
    cv::Mat cam_mat= cv::Mat::eye(3,3,CV_64F);
    cv::Mat distortion;
    char c = '\0';
    float square_dim;
    printf("Type c for calibration\nType everything else for marker detection\n");
    scanf(" %c",&c);
    if (c=='c'){
        scanf(" %f",&square_dim);
        camera_cal_real_time(cam_mat,distortion,square_dim);
    }
    else
    {
        scanf(" %f",&square_dim);
        cv::VideoCapture vid(0);
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
        glutInitWindowPosition(100, 100);
        glutInitWindowSize(1800,2880);
        glutCreateWindow(argv[0]);
        glClearColor(0,0,0,0);
        load_cal("Calibration",cam_mat,distortion);
        load(cam_mat,distortion,square_dim);
        glutDisplayFunc(DrawCube);
        glutReshapeFunc(reshape);
        if (!vid.isOpened())
            return 1;
        glutTimerFunc(0,timer,0);
        glutMainLoop();
    }

    return 0;
}