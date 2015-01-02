#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

void display(void) {
	//clear white, draw with black
	glClearColor(0, 0, 0, 1);
	glColor3d(0, 0, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//this draws a square using vertices
	// glBegin(GL_QUADS);
	// glVertex2i(0, 0);
	// glVertex2i(0, 128);
	// glVertex2i(128, 128);
	// glVertex2i(128, 0);
	// glEnd();

	//a more useful helper
	// glRecti(200, 200, 250, 250);

	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//set the coordinate system, with the origin in the top left
	gluOrtho2D(0, width, height, 0);	// or (0,width,0,hieght)?
	glMatrixMode(GL_MODELVIEW);
}

void idle(void) {
	glutPostRedisplay();
}

void init(const char* title, int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow(title);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutMainLoop();
}