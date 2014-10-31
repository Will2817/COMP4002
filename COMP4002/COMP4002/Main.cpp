
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <glew.h>
#include <glut.h>


#include "Shader.h"
#include "camera.h"
#include "Renderable.h"

// Shader program
GLuint shader;

Camera cam;
Matrix4 projectionMatrix;
std::vector<Renderable*> entities = std::vector<Renderable*>();
bool keys[128] = { false };
bool specials[256] = { false };




/*****************************************************************************/
void Check_GPU_Status();

void resizeWin(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	ratio = (1.0f * w) / h;
	projectionMatrix = Matrix4::projection(60, ratio, 1, 10000);
}

void updateState() {
	if (specials[GLUT_KEY_LEFT])	cam.yaw(Math::degreesToRadians(-2));
	if (specials[GLUT_KEY_RIGHT])	cam.yaw(Math::degreesToRadians(2));
	if (specials[GLUT_KEY_UP])		cam.pitch(Math::degreesToRadians(-2));
	if (specials[GLUT_KEY_DOWN])	cam.pitch(Math::degreesToRadians(2));
	if (keys['a'])					cam.strafe(-0.2f);
	if (keys['d'])					cam.strafe(0.2f);
	if (keys['w'])					cam.move(-0.2);
	if (keys['s'])					cam.move(0.2);

	if (keys['i'])	entities[0]->position.z += 0.1;
	if (keys['k'])	entities[0]->position.z -= 0.1;
	if (keys['j'])	entities[0]->position.x += 0.1;
	if (keys['l'])	entities[0]->position.x -= 0.1;

	auto spin = Quaternion();
	spin.fromHeadPitchRoll(0, 0, 0.1);
	entities[0]->children[0]->orientation *= spin;
	entities[0]->children[1]->orientation *= spin * spin;

}

void renderWin(void) {
	updateState();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader);

	auto mvMatrix = projectionMatrix * cam.getViewMatrix();

	for (auto it = entities.begin(); it != entities.end(); ++it) {
		(*it)->render(mvMatrix);
	}

	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) {
	keys[key] = true;
}

void onKeyUp(unsigned char key, int x, int y) {
	keys[key] = false;
}

void onSpecialDown(int key, int x, int y) {
	specials[key] = true;
}

void onSpecialUp(int key, int x, int y) {
	specials[key] = false;
}

GLuint setupShaders() {
	GLuint p;

	Shader shader = Shader();
	if (shader.createShaderProgram("shader.vert", "shader.frag", &p)) {
		printf("Couldn't create shader");
		exit(-1);
	}

	vertexLoc = glGetAttribLocation(p, "position");
	colorLoc = glGetAttribLocation(p, "color");

	mvpMatrixLoc = glGetUniformLocation(p, "mvpMatrix");

	return(p);
}

void createEntities() {
	entities.push_back(new Box(100, 0, -100, 60, 30, 30));
	entities.push_back(new Box(0, 100, -100, 60, 30, 30));
	entities.push_back(new Box(0, 0, -100, 60, 30, 30));
	entities.push_back(new Box(0, 0, 100, 60, 30, 30));
	entities.push_back(new Box(100, 0, 100, 60, 30, 30));
	entities.push_back(new Box(0, 100, 100, 60, 30, 30));
	entities.push_back(new Box(100, 100, 100, 60, 30, 30));
	entities[0]->children.push_back(new Sphere(20, 30, 0, 10));
	entities[0]->children.push_back(new Sphere(-20, 30, 0, 10));

	entities.push_back(new Sphere(0, 0, -100, 10));
	entities.push_back(new Plane(0, -10, 0, 1000, 1000, 0));
	entities.back()->orientation.fromAxisAngle(Vector3(1, 0, 0), 90);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(650, 500);
	glutCreateWindow("Assignment 2");

	glutDisplayFunc(renderWin);
	glutIdleFunc(renderWin);
	glutReshapeFunc(resizeWin);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialFunc(onSpecialDown);
	glutSpecialUpFunc(onSpecialUp);

	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 1.0, 1.0, 1.0);

	cam.setPosition(100, 100, 0);

	shader = setupShaders();
	createEntities();

	glutMainLoop();

	return(0);
}


/************************************************************************************/


void Check_GPU_Status()
{
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	fprintf(stderr, " ----------------- Checking graphics capability ...\n");
	fprintf(stderr, " GL Vendor: %s \n", glGetString(GL_VENDOR));
	fprintf(stderr, " GL Renderer: %s \n", glGetString(GL_RENDERER));
	fprintf(stderr, " GL version: %s\n", glGetString(GL_VERSION));
	fprintf(stderr, " GL version: %d.%d\n", major, minor);
	fprintf(stderr, " GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	//Another way to query the shaders support
	if (glewGetExtension("GL_ARB_fragment_shader") != GL_TRUE ||
		glewGetExtension("GL_ARB_Vertex_shader") != GL_TRUE ||
		glewGetExtension("GL_ARB_shader_objects") != GL_TRUE ||
		glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
	{
		fprintf(stderr, "Driver does not support OpenGL Shading Language\n");
		exit(1);
	}
	else fprintf(stderr, "GLSL is supported !\n");

}