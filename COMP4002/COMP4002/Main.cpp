
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <glew.h>
#include <glut.h>
#include <unordered_map>

#include "Shader.h"
#include "camera.h"
#include "Renderable.h"

// Shader program

Camera cam;
Matrix4 projectionMatrix;
std::vector<Entity*> entities = std::vector<Entity*>();
bool keys[128] = { false };
bool specials[256] = { false };
int oldtime = 0;
std::unordered_map<std::string, GLuint> images;


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
	auto time = glutGet(GLUT_ELAPSED_TIME);
	auto delta = (time - oldtime) / 1000.0;
	oldtime = time;

	float mspeed = 300 * delta;
	float tspeed = 50 * delta;

	if (specials[GLUT_KEY_LEFT])	cam.yaw(tspeed);
	if (specials[GLUT_KEY_RIGHT])	cam.yaw(-tspeed);
	if (specials[GLUT_KEY_UP])		cam.pitch(tspeed);
	if (specials[GLUT_KEY_DOWN])	cam.pitch(-tspeed);
	if (keys['a'])					cam.strafe(-mspeed);
	if (keys['d'])					cam.strafe(mspeed);
	if (keys['w'])					cam.move(mspeed);
	if (keys['s'])					cam.move(-mspeed);

	/*
	if (keys['i'])	entities[0]->position.z += 0.1;
	if (keys['k'])	entities[0]->position.z -= 0.1;
	if (keys['j'])	entities[0]->position.x += 0.1;
	if (keys['l'])	entities[0]->position.x -= 0.1;

	auto spin = Quaternion();
	spin.fromHeadPitchRoll(0, 0, 0.1);
	entities[0]->children[0]->orientation *= spin;
	entities[0]->children[1]->orientation *= spin * spin;
	*/

}

void renderWin(void) {
	updateState();

	static int frame = 0;
	static int timebase = 0;
	char s[32];
	frame++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s, "Graphics FPS:%4.2f",
			frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
		glutSetWindowTitle(s);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

GLuint setupShaders(char* vert,char* frag) {
	GLuint p;

	Shader shader = Shader();
	if (shader.createShaderProgram(vert,frag, &p)) {
		printf("Couldn't create shader");
		exit(-1);
	}

	return(p);
}

void loadImages()
{
	images["nature_bark.png"] = SOIL_load_OGL_texture("nature_bark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	images["templeaf.png"] = SOIL_load_OGL_texture("templeaf.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}

void createEntities() {
	entities.push_back(new Entity(Vector3(0, 0, 0), new Plane(2000, 2000, 0, shader1, false)));
	entities.back()->orientation.fromAxisAngle(Vector3(1, 0, 0), 90);

	entities.push_back(new Entity(Vector3(0, 0, -100), new Leaf(images["templeaf.png"])));
	/*
	for (auto i = 0; i < 5; ++i) {
		auto tree = new TreeNaive(Vector3(-400 + i*200, 0, -400), 15, 0.5, 20, 3, 1, 30, shader2, true, images["nature_bark.png"],images["templeaf.png"]);
		entities.push_back(tree->root);
	}
	*/
	auto tree = new TreeLSystem(0, 0, -300, 20, shader2, true, images["nature_bark.png"], images["templeaf.png"]);
	entities.push_back(tree->root);
}

int main(int argc, char **argv) {
	srand(10);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(650, 500);
	glutCreateWindow("Graphics");

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

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);

	cam = Camera(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	cam.setPosition(Vector3(100, 100, 10));

	shader1 = setupShaders("shader.vert", "shader.frag");
	shader2 = setupShaders("shader2.vert", "shader2.frag");
	loadImages();
	//bark_img = SOIL_load_image("nature_bark.png", &bark_img_width, &bark_img_height, NULL, 0);
	//leaf_img = SOIL_load_image("templeaf.png", &leaf_image_width, &leaf_image_height, NULL, 0);
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