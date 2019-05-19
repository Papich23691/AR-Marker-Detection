
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <sstream>
#include <iostream>
#include <fstream>

GLfloat xRotated, yRotated, zRotated;
cv::Vec3f trans,rot;
cv::Mat cam_mat= cv::Mat::eye(3,3,CV_64F);
cv::Mat distortion;
cv::VideoCapture vid(0);
const double square_dim=0.057;


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
               // printf("%d t - <%f,%f,%f>\n",i,translated[i][0],translated[i][1],translated[i][2]);
                trans = translated[i];
                rot = rotate[i];
                printf("%d r - <%f,%f,%f>\n",i,rotate[i][0],rotate[i][1],rotate[i][2]);
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
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(-1 *(trans[0])/square_dim,-1 * (trans[1])/square_dim,-1 * trans[2]/(square_dim* 5.7));
    glRotatef(180,rot[0]/square_dim,rot[1]/square_dim,rot[2]/square_dim);
    /*cv::Mat rmat;
    cv::Rodrigues(rot, rmat);
    float mat[rmat.rows * rmat.cols];
    int m_index=0;
    printf("____\n");
    for (int i=0;i<rmat.rows;i++)
    {
        for (int j=0;j<rmat.cols;j++)
        {
            mat[m_index] = rmat.at<float>(j,i);
            printf("%f\t",mat[m_index]);
        }
    } 
    printf("____\n");
    glMultMatrixf(mat);*/
    glutWireTeapot(0.5);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
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

void reshape(int x, int y)
{
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    gluPerspective(60.0,1,0.5,200.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  
}

int main(int argc, char** argv){
glutInit(&argc, argv);
//we initizlilze the glut. functions
glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
glutInitWindowPosition(100, 100);
glutInitWindowSize(1800,2880);
glutCreateWindow(argv[0]);
glClearColor(0,0,0,0);
load_cal("Calibration",cam_mat,distortion);
trans[0] = 0;
trans[1] =0;
trans[2] = 0;
glutDisplayFunc(DrawCube);
glutReshapeFunc(reshape);
if (!vid.isOpened())
    return 1;
glutTimerFunc(0,timer,0);
//Set the function for the animation.
glutMainLoop();
return 0;
}
 