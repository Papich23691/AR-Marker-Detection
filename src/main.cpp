#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
    #include <windows.h>
#endif
#include <GL/glut.h>
#endif

#include <iostream>

#include "calibration.h"
#include "marker.h"

#include "opencv2/highgui.hpp"

cv::VideoCapture vid(0);

int main(int argc,char **argv){
    if (argc < 3){
        std::cerr << argv[0] << " <c for calibartion or else> <square size>" << std::endl;
        return EXIT_FAILURE;
    }
    cv::Mat cam_mat= cv::Mat::eye(3,3,CV_64F);
    cv::Mat distortion;
    char c = '\0';
    float square_dim;
    c = *argv[1];
    square_dim = atof(argv[2]);
    if (c=='c'){
        camera_cal_real_time(cam_mat,distortion,square_dim);
    }
    else
    {
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