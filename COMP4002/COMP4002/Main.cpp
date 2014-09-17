/*
 * OpenGLSamples (openglsamples.sf.net) Examples
 * VC++ users should create a Win32 Console project and link 
 * the program with glut32.lib, glu32.lib, opengl32.lib
 *
 * GLUT can be downloaded from http://www.xmission.com/~nate/glut.html
 * OpenGL is by default installed on your system.
 * For an installation of glut on windows for MS Visual Studio 2010 see: http://nafsadh.wordpress.com/2010/08/20/glut-in-ms-visual-studio-2010-msvs10/
 *
 */

#include <stdio.h>
#include <windows.h>	   // Standard header for MS Windows applications
#include <GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <glut.h>	   // The GL Utility Toolkit (GLUT) Header
#include <string>
#include <ctime>

#define RAND_MAX 1

using namespace std;

#include "Model.h"
//#include "Camera.h"

#define KEY_ESCAPE 27

typedef struct {
    int width;
	int height;
	char* title;

	float field_of_view_angle;
	float z_near;
	float z_far;
	bool fullscreen;
} glutWindow;

glutWindow win;
Model model;
float angle;
//Camera camera;

int mousex;
int mousey;

void printtext(int x, int y, string String)
{
	//(x,y) is from the bottom left of the window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, win.width, 0, win.height, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	for (int i = 0; i<String.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, String[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		     // Clear Screen and Depth Buffer
	glLoadIdentity();

	char string[64];
	sprintf(string, "Angle: %f", angle);
	printtext(10, 10, string);

	gluLookAt(0,0,10,0,0,0,0,1,0);

	glColor3f(0.5f, 0.5f, 0.5f);

	for (int i = 0; i < 10; i++){
		glBegin(GL_TRIANGLE_STRIP);
			for (int j = 0; j < 10; j++){
				glColor3f(0.5f + (i % 3 * 0.1f), 0.5f + (i % 3 * 0.1f), 0.5f + (i % 3 * 0.1f));
				glVertex3f(i, -2.0f+(i%3 * 0.1f), j);
				glColor3f(0.5f + ((i + 1) % 3 * 0.1f), 0.5f + ((i + 1) % 3 * 0.1f), 0.5f + ((i + 1) % 3 * 0.1f));
				glVertex3f(i+1, -2.0f+((i+1)%3 * 0.1f), j);
			}
		glEnd();
	}

	//glTranslatef(0.0f,0.0f,-3.0f);			
 
	/*
	 * Triangle code starts here
	 * 3 verteces, 3 colors.
	 */
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);					
		glColor3f(0.0f,0.0f,1.0f);			
		glVertex3f( 0.0f, 1.0f, 0.0f);		
		glColor3f(0.0f,1.0f,0.0f);			
		glVertex3f(-1.0f,-1.0f, 0.0f);		
		glColor3f(1.0f,0.0f,0.0f);			
		glVertex3f( 1.0f,-1.0f, 0.0f);
	glEnd();				

	//glPushMatrix();

	glTranslatef(2.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
		model.Display();
	glEnd();


	glTranslatef(-4.0f,0.0f,0.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	
	//glutSolidTeapot(1);
	//glutSolidIcosahedron();
	//glutWireIcosahedron();
	//glutWireTeapot(1);
	glutWireSphere(1,20,20);

	//glPopMatrix();

	glutSwapBuffers();
	angle += 0.01f;
}


void initialize () 
{
	srand(std::time(0));
    glMatrixMode(GL_PROJECTION);												// select projection matrix
    glViewport(0, 0, win.width, win.height);									// set the viewport
    glMatrixMode(GL_PROJECTION);												// set matrix mode
    glLoadIdentity();															// reset projection matrix
    GLfloat aspect = (GLfloat) win.width / win.height;
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);		// set up a perspective projection matrix
    glMatrixMode(GL_MODELVIEW);													// specify which matrix is the current matrix
    glShadeModel( GL_SMOOTH );
    glClearDepth( 1.0f );														// specify the clear value for the depth buffer
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );						// specify implementation-specific hints
	glClearColor(0.0, 0.0, 0.0, 1.0);											// specify clear values for the color buffers	
	model.Initialize();
}


void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )		
{ 
  switch ( key ) 
  {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;

	case 'f':
		win.fullscreen = !win.fullscreen;
		if (win.fullscreen)	glutFullScreen();
		else {
			glutPositionWindow(9, 30);
			glutReshapeWindow(win.width, win.height);
		}
		break;

    default:      
      break;
  }
}

void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_LEFT:
			//camera.YawPitchRoll(-0.05f, 0.0f, 0.0f);
			break;
		case GLUT_KEY_RIGHT:
			//camera.YawPitchRoll(0.05f, 0.0f, 0.0f);
			break;
	}
}

void changeSize(int w, int h)
{
	if (h == 0){
		h = 1;
	}

	float ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(win.field_of_view_angle, ratio, win.z_near, win.z_far);
	glMatrixMode(GL_MODELVIEW);
	if (!win.fullscreen){
		win.width = w;
		win.height = h;
	}
}

void mouseMove(int x, int y){
	
	if (mousex >= 0) {
		//camera.YawPitchRoll(0.005f * (x - mousex), 0.0f, 0.0f);
		mousex = x;
	}
	if (mousey >= 0) {
		//camera.YawPitchRoll(0.0f, 0.005f * (y - mousey), 0.0f);
		mousey = y;
	}

}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON){
		if (state == GLUT_UP){
			mousex = -1;
			mousey = -1;
		}
		else if (state == GLUT_DOWN){
			mousex = x;
			mousey = y;
		}
	}
}

int main(int argc, char **argv) 
{
	// set window values
	win.width = 640;
	win.height = 480;
	win.title = "OpenGL/GLUT Example. Visit http://openglsamples.sf.net ";
	win.field_of_view_angle = 45;
	win.z_near = 1.0f;
	win.z_far = 500.0f;

	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	
	glutDisplayFunc(display);									// register Display Function
	glutReshapeFunc(changeSize);

	glutIdleFunc( display );									// register Idle Function
    glutKeyboardFunc( keyboard );								// register Keyboard Handler
	glutSpecialFunc(processSpecialKeys);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	initialize();
	glutMainLoop();												// run GLUT mainloop
	return 0;
}