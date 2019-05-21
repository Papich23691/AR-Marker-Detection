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

cv::Vec3f trans,rot;
extern cv::VideoCapture vid(0);
static cv::Mat cam_mat,distortion,frame;
static double square_dim;

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

void timer(int)
{     
    glutPostRedisplay();
    glutTimerFunc(1000/120,timer,0);
    std::vector<int> marker_id;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::Ptr<cv::aruco::Dictionary> marker_dic = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);
    std::vector<cv::Vec3d> rotate,translated; 
    if (vid.read(frame))
    {
            cv::aruco::detectMarkers(frame,marker_dic,corners,marker_id);
            cv::aruco::estimatePoseSingleMarkers(corners, square_dim,cam_mat,distortion,rotate,translated);
            for (int i=0;i<marker_id.size();i++){
                cv::Mat rotation_mat;
                trans = translated[i];
                rot = rotate[i];
                printf("%d r - <%f,%f,%f>\n",i,rot[0],rot[1],rot[2]);
                printf("%d t - <%f,%f,%f>\n",i,trans[0],trans[1],trans[2]);
              cv::aruco::drawAxis(frame,cam_mat,distortion,rotate[i],translated[i],0.02f);
            }
            cv::flip(frame,frame,1);
            cv::waitKey(30);
    }

}


void DrawCube(void)
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
    glPushMatrix();
    glTranslatef(-1 *(trans[0])/square_dim,-1 * (trans[1])/square_dim,-1 * trans[2]/(square_dim*square_dim*100));
    glRotatef(180,rot[0]/square_dim,rot[1]/square_dim,rot[2]/square_dim);
    glColor3f(1.0f,0,0);
    glutWireTeapot(0.3);
    glPopMatrix();

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

void load(cv::Mat cMatrix,cv::Mat dMatrix,double sDim)
{
    square_dim = sDim;
    cam_mat = cMatrix;
    distortion = dMatrix;
}