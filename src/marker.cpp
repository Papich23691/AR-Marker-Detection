
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"

#include <GLUT/glut.h>

#include "marker.h"
#include "calibration.h"

cv::Vec3f trans,rot;
extern cv::VideoCapture vid(0);
static cv::Mat cam_mat,distortion;
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

void timer(int)
{     
    glutPostRedisplay();
    glutTimerFunc(1000/60,timer,0);
    cv::Mat frame;
    std::vector<int> marker_id;
    std::vector<std::vector<cv::Point2f>> corners, rejected;
    cv::aruco::DetectorParameters params;
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
            cv::imshow("cam",frame);
            cv::waitKey(30);
    }

}


void DrawCube(void)
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(-1 *(trans[0])/square_dim,-1 * (trans[1])/square_dim,-1 * trans[2]/(square_dim* 5.7));
    glRotatef(180,rot[0]/square_dim,rot[1]/square_dim,rot[2]/square_dim);
    glutWireTeapot(0.5);
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