#include <iostream>
#include <GL/glu.h>
#include <GL/gl.h>
#include <vector>
#include <cmath>
#include "../Libs/vec3f.h"

using namespace std;

//Global declarations
float ww=500.0, wh=500.0;
float speed = 1,camx=0,camy=0,camz=0,speedAngle=0.01;
float rotateX=0,rotateY=0,rotateZ=0;
int mode = 0;

class Cube
{
	public:
	    int side;
	    float sConst;
		bool hide;
        Vec3f color;
		Vec3f center;
		Vec3f rotateAxis;
		float angle;

		//faces
		Vec3f front[4];
		Vec3f back[4];
		Vec3f top[4];
		Vec3f bottom[4];
		Vec3f left[4];
		Vec3f right[4];


		Cube(int s=1, int c = 1, Vec3f x = Vec3f(0.0, 0.0, 0.0), Vec3f r = Vec3f(0.0, 0.0, 1.0) )
		{
		    side = s;
		    sConst = c;
			center = x;
			rotateAxis = r;
			hide = true;
            color = Vec3f(((rand()%10)/10), ((rand()%10)/10), ((rand()%10)/10));
            angle = 0;

			//front face
			front[0] = Vec3f(-side+center.v[0], -side+center.v[1], side+center.v[2]);
			front[1] = Vec3f(side+center.v[0], -side+center.v[1], side+center.v[2]);
			front[2] = Vec3f(side+center.v[0], side+center.v[1], side+center.v[2]);
			front[3] = Vec3f(-side+center.v[0], side+center.v[1], side+center.v[2]);

			//back face
			back[0] = Vec3f(-side+center.v[0], -side+center.v[1], -side+center.v[2]);
			back[1] = Vec3f(side+center.v[0], -side+center.v[1], -side+center.v[2]);
			back[2] = Vec3f(side+center.v[0], side+center.v[1], -side+center.v[2]);
			back[3] = Vec3f(-side+center.v[0], side+center.v[1], -side+center.v[2]);

        	//right face
			right[0] = Vec3f(side+center.v[0], -side+center.v[1], side+center.v[2]);
			right[1] = Vec3f(side+center.v[0], -side+center.v[1], -side+center.v[2]);
			right[2] = Vec3f(side+center.v[0], side+center.v[1], -side+center.v[2]);
			right[3] = Vec3f(side+center.v[0], side+center.v[1], side+center.v[2]);

        	//left face
			left[0] = Vec3f(-side+center.v[0], -side+center.v[1], side+center.v[2]);
			left[1] = Vec3f(-side+center.v[0], -side+center.v[1], -side+center.v[2]);
			left[2] = Vec3f(-side+center.v[0], side+center.v[1], -side+center.v[2]);
			left[3] = Vec3f(-side+center.v[0], side+center.v[1], side+center.v[2]);

        	//top face
			top[0] = Vec3f(-side+center.v[0], side+center.v[1], side+center.v[2]);
			top[1] = Vec3f(side+center.v[0], side+center.v[1], side+center.v[2]);
			top[2] = Vec3f(side+center.v[0], side+center.v[1], -side+center.v[2]);
			top[3] = Vec3f(-side+center.v[0], side+center.v[1], -side+center.v[2]);

        	//bottom
			bottom[0] = Vec3f(-side+center.v[0], -side+center.v[1], side+center.v[2]);
			bottom[1] = Vec3f(side+center.v[0], -side+center.v[1], side+center.v[2]);
			bottom[2] = Vec3f(side+center.v[0], -side+center.v[1], -side+center.v[2]);
			bottom[3] = Vec3f(-side+center.v[0], -side+center.v[1], -side+center.v[2]);

		}
};

Cube *c1, *shearc2;

void myInit()
{
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective(45, 1.0, 1, 200);
		//gluOrtho2D(-ww/2, ww/2, -wh/2, wh/2);
}

Vec3f getVertices(float mv[16], Vec3f ver)
{

        float xp = mv[0] * ver.v[0] + mv[4] * ver.v[1] + mv[8] * ver.v[2] + mv[12];
        float yp = mv[1] * ver.v[0] + mv[5] * ver.v[1] + mv[9] * ver.v[2] + mv[13];
        float zp = mv[2] * ver.v[0] + mv[6] * ver.v[1] + mv[10] * ver.v[2] + mv[14];
        float wp = mv[3] * ver.v[0] + mv[7] * ver.v[1] + mv[11] * ver.v[2] + mv[15];

        xp /= wp;
        yp /= wp;
        zp /= wp;

        return Vec3f(xp, yp, zp);
}

void getTranformedFace(float mv[16], Vec3f *f)
{

    *f = getVertices(mv, *f);
    *(f+1) = getVertices(mv, *(f+1));
    *(f+2) = getVertices(mv, *(f+2));
    *(f+3) = getVertices(mv, *(f+3));
}

void drawCubes()
{

        if(c1==NULL)             //if cube don't exists then return
            return ;

        float centerX = c1->center.v[0];
        float centerY = c1->center.v[1];
        float centerZ = c1->center.v[2];

        glColor3f(0.0, 0.0, 1.0);

        glPushMatrix();

        glTranslatef(centerX, centerY, centerZ);
        glRotatef(-rotateX, 1, 0, 0);
        glRotatef(-rotateY, 0, 1, 0);
        glRotatef(rotateZ, 0, 0, 1);
        glTranslatef(-centerX, -centerY, -centerZ);

        float mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        glPopMatrix();
        
        glTranslatef(0.0 , 0.0, -20.0);

        getTranformedFace(mv, c1->front);
		glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->front[0].v);
		glVertex3fv(c1->front[1].v);
		glVertex3fv(c1->front[2].v);
		glVertex3fv(c1->front[3].v);
        glEnd();

        getTranformedFace(mv, c1->back);
        glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->back[0].v);
		glVertex3fv(c1->back[1].v);
		glVertex3fv(c1->back[2].v);
		glVertex3fv(c1->back[3].v);
        glEnd();

        getTranformedFace(mv, c1->right);
        glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->right[0].v);
		glVertex3fv(c1->right[1].v);
		glVertex3fv(c1->right[2].v);
		glVertex3fv(c1->right[3].v);
        glEnd();

        getTranformedFace(mv, c1->left);
        glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->left[0].v);
		glVertex3fv(c1->left[1].v);
		glVertex3fv(c1->left[2].v);
		glVertex3fv(c1->left[3].v);
        glEnd();

        getTranformedFace(mv, c1->top);
        glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->top[0].v);
		glVertex3fv(c1->top[1].v);
		glVertex3fv(c1->top[2].v);
		glVertex3fv(c1->top[3].v);
        glEnd();

        getTranformedFace(mv, c1->bottom);
        glBegin( GL_LINE_LOOP );
		glVertex3fv(c1->bottom[0].v);
		glVertex3fv(c1->bottom[1].v);
		glVertex3fv(c1->bottom[2].v);
		glVertex3fv(c1->bottom[3].v);
        glEnd();

        rotateX = 0;
        rotateY = 0;
        rotateZ = 0;

        if(c1->hide)
        	return;
		float c = -1;

		glColor3f(1.0, 0.0, 0.0);


		//Bottom sheared face
		glBegin( GL_LINE_LOOP );
		glVertex3f(c1->bottom[0].v[0] + c*c1->bottom[0].v[2], c1->bottom[0].v[1] + c*c1->bottom[0].v[2], c1->bottom[0].v[2] );
		glVertex3f(c1->bottom[1].v[0] + c*c1->bottom[1].v[2], c1->bottom[1].v[1] + c*c1->bottom[1].v[2], c1->bottom[1].v[2] );
		glVertex3f(c1->bottom[2].v[0] + c*c1->bottom[2].v[2], c1->bottom[2].v[1] + c*c1->bottom[2].v[2], c1->bottom[2].v[2] );
		glVertex3f(c1->bottom[3].v[0] + c*c1->bottom[3].v[2], c1->bottom[3].v[1] + c*c1->bottom[3].v[2], c1->bottom[3].v[2] );
        glEnd();

        //top sheared face
        glBegin( GL_LINE_LOOP );
		glVertex3f(c1->top[0].v[0] + c*c1->top[0].v[2], c1->top[0].v[1] + c*c1->top[0].v[2], c1->top[0].v[2] );
		glVertex3f(c1->top[1].v[0] + c*c1->top[1].v[2], c1->top[1].v[1] + c*c1->top[1].v[2], c1->top[1].v[2] );
		glVertex3f(c1->top[2].v[0] + c*c1->top[2].v[2], c1->top[2].v[1] + c*c1->top[2].v[2], c1->top[2].v[2] );
		glVertex3f(c1->top[3].v[0] + c*c1->top[3].v[2], c1->top[3].v[1] + c*c1->top[3].v[2], c1->top[3].v[2] );
        glEnd();

        //right sheared face
        glBegin( GL_LINE_LOOP );
		glVertex3f(c1->right[0].v[0] + c*c1->right[0].v[2], c1->right[0].v[1] + c*c1->right[0].v[2], c1->right[0].v[2] );
		glVertex3f(c1->right[1].v[0] + c*c1->right[1].v[2], c1->right[1].v[1] + c*c1->right[1].v[2], c1->right[1].v[2] );
		glVertex3f(c1->right[2].v[0] + c*c1->right[2].v[2], c1->right[2].v[1] + c*c1->right[2].v[2], c1->right[2].v[2] );
		glVertex3f(c1->right[3].v[0] + c*c1->right[3].v[2], c1->right[3].v[1] + c*c1->right[3].v[2], c1->right[3].v[2] );
        glEnd();

        //left sheared face
        glBegin( GL_LINE_LOOP );
		glVertex3f(c1->left[0].v[0] + c*c1->left[0].v[2], c1->left[0].v[1] + c*c1->left[0].v[2], c1->left[0].v[2] );
		glVertex3f(c1->left[1].v[0] + c*c1->left[1].v[2], c1->left[1].v[1] + c*c1->left[1].v[2], c1->left[1].v[2] );
		glVertex3f(c1->left[2].v[0] + c*c1->left[2].v[2], c1->left[2].v[1] + c*c1->left[2].v[2], c1->left[2].v[2] );
		glVertex3f(c1->left[3].v[0] + c*c1->left[3].v[2], c1->left[3].v[1] + c*c1->left[3].v[2], c1->left[3].v[2] );
        glEnd();

        //front sheared face

        glBegin( GL_LINE_LOOP );
		glVertex3f(c1->front[0].v[0] + c*c1->front[0].v[2], c1->front[0].v[1] + c*c1->front[0].v[2], c1->front[0].v[2] );
		glVertex3f(c1->front[1].v[0] + c*c1->front[1].v[2], c1->front[1].v[1] + c*c1->front[1].v[2], c1->front[1].v[2] );
		glVertex3f(c1->front[2].v[0] + c*c1->front[2].v[2], c1->front[2].v[1] + c*c1->front[2].v[2], c1->front[2].v[2] );
		glVertex3f(c1->front[3].v[0] + c*c1->front[3].v[2], c1->front[3].v[1] + c*c1->front[3].v[2], c1->front[3].v[2] );
        glEnd();


        //back sheared face

        glBegin( GL_LINE_LOOP );
		glVertex3f(c1->back[0].v[0] + c*c1->back[0].v[2], c1->back[0].v[1] + c*c1->back[0].v[2], c1->back[0].v[2] );
		glVertex3f(c1->back[1].v[0] + c*c1->back[1].v[2], c1->back[1].v[1] + c*c1->back[1].v[2], c1->back[1].v[2] );
		glVertex3f(c1->back[2].v[0] + c*c1->back[2].v[2], c1->back[2].v[1] + c*c1->back[2].v[2], c1->back[2].v[2] );
		glVertex3f(c1->back[3].v[0] + c*c1->back[3].v[2], c1->back[3].v[1] + c*c1->back[3].v[2], c1->back[3].v[2] );
        glEnd();
}

void displayFunc()
{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		drawCubes();
		glFlush();
		glutSwapBuffers();
}

void reshapeFunc(int w, int h)
{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective(45, (GLfloat)w/(GLfloat)h, 1, 200);
		//gluOrtho2D(-w/2, w/2, -h/2, h/2);
		glViewport(0.0, 0.0, w, h);
		ww=w;
		wh=h;
}

void clearScreen()
{
    mode = 0;
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	glFlush();
	if(!c1)
    delete (c1);
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
	    case 'a':
				rotateY+=speed;
				glutPostRedisplay();
				break;
        case 'd':
				rotateY-=speed;
				glutPostRedisplay();
                break;
        case 'w':
				rotateX+=speed;
				glutPostRedisplay();
                break;
		case 's':
                rotateX-=speed;
				glutPostRedisplay();
                break;
        case '1':
                mode = 1;
                cout<<"mode = "<<mode<<endl;
                break;
        case '2':
                if(c1)
                {
                	c1->hide = !c1->hide;
                	cout<<"hide = "<<c1->hide<<endl;
                	glutPostRedisplay();
                }
                else
                	cout<<"create cube first press 1 "<<endl;
                break;
		case 'c':
				clearScreen();
				break;
        case 27:
                exit(0);
	}
}

void mouseFunc(int btn, int state, int x, int y)
{
    if(btn == GLUT_LEFT_BUTTON && mode == 1)
    {
        // y = wh - y;
        if(!c1)
            delete c1;
        int s;
        float c;
        cout<<"Enter side of cube and shear const";
        cin>>s>>c;
        //c1 = new Cube(s, Vec3f(x , y, 0.0));
        c1 = new Cube(s,c);
        if(c1)
        {
            cout<<"Cube created with center "<<endl;
            cout<<(cout,c1->center)<<endl;
        }
        else
            exit(0);
        glutPostRedisplay();
    }
    else
        cout<<"Press 1 for drawing cube"<<endl;
}
//void motionFunc(int x, int y);

int main(int argc, char **argv)
{
		glutInit(&argc,argv);
		glutInitWindowSize(ww, wh);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutCreateWindow("Polygon");
		myInit();
		glutDisplayFunc(displayFunc);
		glutReshapeFunc(reshapeFunc);
		glutKeyboardFunc(keyboardFunc);
        glutMouseFunc(mouseFunc);
        //glutMotionFunc(motionFunc);
		glutMainLoop();

		return 0;
}
