#ifndef MARKER
#define MARKER

#include "opencv2/core.hpp"

/**
 * Function which saves all of the aruco marker in the current folder 
 */
void create_aruco_markers();

/**
 * OpenGL timer function called by glutTimerFunc 
 * which detects a marker for each frame
 */
void timer(int); 

/**
 * Using glutDisplayFunc with Draw() to render the background 
 * image from the camera as well as rendering the teapot
 */
void Draw(void);

void reshape(int x, int y);

/**
 * Pass data from main.cpp
 */
void load(cv::Mat cMatrix,cv::Mat dMatrix, double sDim);

#endif
