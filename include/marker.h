#ifndef MARKER
#define MARKER

#include "opencv2/core.hpp"

void create_aruco_markers();
void timer(int); 
void DrawCube(void);
void reshape(int x, int y);
void load(cv::Mat cMatrix,cv::Mat dMatrix, double sDim);

#endif
