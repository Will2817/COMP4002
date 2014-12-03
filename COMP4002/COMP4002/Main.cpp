
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <glew.h>
#include <glut.h>
#include <unordered_map>

#include "Shader.h"
#include "camera.h"
#include "Renderable.h"
#include "Skybox.h"
// Shader program

Camera cam;
Matrix4 projectionMatrix;
std::vector<Entity*> entities = std::vector<Entity*>();
std::vector<Entity*> scene1 = std::vector<Entity*>();
std::vector<Entity*> scene2 = std::vector<Entity*>();

bool keys[128] = { false };
bool specials[256] = { false };
int oldtime = 0;
float worldSize = 1500, terrainHeight = 200;
std::unordered_map<std::string, GLuint> images;
Skybox *skybox;
HeightMap h_map;
Entity* terrain;
std::vector<Matrix4> modelMatrices;

/*****************************************************************************/
void Check_GPU_Status();

void makeTree() {
	auto x = randf();
	auto z = randf();
	auto mat = Matrix4::translation(x * worldSize, h_map.lookup(x, z) * terrainHeight - 5, z * worldSize);
	Quaternion rot; rot.fromAxisAngle(Vector3(0, 1, 0), randf() * 360);
	modelMatrices.push_back(mat * rot.toMatrix4());
}

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
int activeScene = 1;
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

	if (keys['1']) activeScene = 1;
	if (keys['2']) activeScene = 2;
	if (keys['3']) activeScene = 3;

}

void renderWin(void) {
	updateState();

	static int frame = 0;
	static int timebase = 0;
	char s[32];
	frame++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s, "Graphics FPS:%4.2f Tree: %d",
			frame*1000.0 / (time - timebase), modelMatrices.size());
		timebase = time;
		frame = 0;
		glutSetWindowTitle(s);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto mvMatrix = projectionMatrix * cam.getViewMatrix();
	auto rootMatrix = Matrix4::IDENTITY;
	skybox->render_self(mvMatrix,cam.getPosition());
	terrain->render(mvMatrix, rootMatrix, modelMatrices);

	if (activeScene == 1)
	{
		for (auto it = scene1.begin(); it != scene1.end(); ++it) {
			(*it)->render(mvMatrix, rootMatrix, modelMatrices);
		}
	}
	if (activeScene == 2)
	{
		std::vector<Matrix4> temp;
		temp.push_back(Matrix4::IDENTITY);
		for (auto it = scene2.begin(); it != scene2.end(); ++it) {
			(*it)->render(mvMatrix, rootMatrix, temp);
		}
	}
	if (activeScene == 3)
	{
		for (auto it = entities.begin(); it != entities.end(); ++it) {
			(*it)->render(mvMatrix, rootMatrix, modelMatrices);
		}
	}
	

	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) {
	keys[key] = true;

	if (key == 'c') makeTree();
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
	images["ground_texture.png"] = SOIL_load_OGL_texture("ground_texture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS);
	images["emerald_skybox"] = SOIL_load_OGL_cubemap("siege_right.jpg", "siege_left.jpg", "siege_top.jpg", "siege_front.jpg", "siege_front.jpg", "siege_back.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		NULL
		);
}

void createEntities() {
	h_map = HeightMap("heightmap3.jpg");
	terrain = new Entity(Vector3(0, 0, 0), new Terrain(shader2, h_map, true, images["ground_texture.png"], false, 200, 10.0f));
	terrain->setScale(Vector3(worldSize, terrainHeight, worldSize));

	for (int i = 0; i < 10; ++i) {
		makeTree();
	}

	std::vector<Matrix4> matrices;
	matrices.push_back(Matrix4::IDENTITY);

	entities.push_back(new TreeLSystem(Vector3(0, 0, 0), shader4, true, images["nature_bark.png"], images["templeaf.png"]));
	scene2.push_back(new TreeLSystem(Vector3(0, 0, 0), shader4, true, images["nature_bark.png"], images["templeaf.png"]));
	scene1.push_back(new TreeNaive(Vector3(0, 0, 0), shader2, true, images["nature_bark.png"], images["templeaf.png"]));
}

// Function called when timer ends
void timer(int id) {
	glutPostRedisplay(); // Tell GLUT to call it's glutDisplayFunc
}

// Set new timer and render
void display(void)
{
	glutTimerFunc(17, timer, 1);                   //Call timer function in at least 17 milliseconds
	
	renderWin();            //Call our render function
}

int main(int argc, char **argv) {
	srand(10);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(650, 500);
	glutCreateWindow("Graphics");

	glutDisplayFunc(display);
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
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);

	cam = Camera(Vector3(2000, 300, 200), Vector3(0, 0, 0), Vector3(0, 1, 0));
	// cam.setPosition(Vector3(2000, 300, 200));

	shader1 = setupShaders("shader.vert", "shader.frag");
	shader2 = setupShaders("shader2.vert", "shader2.frag");
	shader3 = setupShaders("skybox.vert", "skybox.frag");
	shader4 = setupShaders("instanceTextureShader.vert", "shader2.frag");
	shader5 = setupShaders("instanceTextureShader.vert", "shader5.frag");
	loadImages();
	skybox = new Skybox(shader3, images["emerald_skybox"]);

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