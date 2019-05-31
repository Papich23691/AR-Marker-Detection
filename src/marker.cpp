#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
    #include <windows.h>
#endif
#include <GL/glut.h>
#endif

#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"

#include "marker.h"
#include "calibration.h"

/* The output vectors of the marker detection */
std::vector<cv::Vec3d> rotate,translated; 
extern cv::VideoCapture vid;
/* The camera and distortion matrices loaded from the ./Calibration file */
static cv::Mat cam_mat,distortion;
cv::Mat frame;
static double square_dim;

/**
 * Function which saves all of the aruco marker in the current folder 
 */
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

/**
 * Parsing the OpenCV matrix to OpenGL texture 
 */
void create_background()
{
    glEnable(GL_TEXTURE_2D);
    GLuint texture[1];                     
    glGenTextures(1, &texture[0]);                 
    glBindTexture(GL_TEXTURE_2D, texture[0]);               
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexImage2D(GL_TEXTURE_2D, 0, 3, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);
}

/**
 * OpenGL timer function called by glutTimerFunc 
 * which detects a marker for each frame
 */
void timer(int)
{     
    glutPostRedisplay();
    glutTimerFunc(1000/120,timer,0);
    std::vector<int> marker_id;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::Ptr<cv::aruco::Dictionary> marker_dic = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
    if (vid.read(frame))
    {
            cv::aruco::detectMarkers(frame,marker_dic,corners,marker_id);
            cv::aruco::estimatePoseSingleMarkers(corners, square_dim,cam_mat,distortion,rotate,translated);
            for (int i=0;i<marker_id.size();i++){
                cv::Mat rotation_mat;
              cv::aruco::drawAxis(frame,cam_mat,distortion,rotate[i],translated[i],0.02f);
            }
            cv::flip(frame,frame,1);
            cv::waitKey(30);
    }

}

/**
 * Using glutDisplayFunc with Draw() to render the background 
 * image from the camera as well as rendering the teapot
 */
void Draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    create_background();
    glPushMatrix();
    glTranslatef(0,0,-1.5);
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(-1,1);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(-1,-1);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(1,-1);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(1,1);
    glEnd();
    glPopMatrix();

    glLoadIdentity();
    for (int i=0;i<translated.size();i++){
        glPushMatrix();
        glTranslatef(-1 *(translated[i][0])/square_dim,-1 * (translated[i][1])/square_dim,-1 * translated[i][2]/(square_dim*square_dim*100));
        glRotatef(180,rotate[i][0]/square_dim,rotate[i][1]/square_dim,rotate[i][2]/square_dim);
        glColor3f(1.0f,0,0);
        glutWireTeapot(0.3);
        glPopMatrix();
    }

    glFlush();
    glutSwapBuffers();
}


void reshape(int x, int y)
{
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    gluPerspective(60.0,1,0.5,200.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  
}

/**
 * Pass data from main.cpp
 */
void load(cv::Mat cMatrix,cv::Mat dMatrix,double sDim)
{
    square_dim = sDim;
    cam_mat = cMatrix;
    distortion = dMatrix;
}