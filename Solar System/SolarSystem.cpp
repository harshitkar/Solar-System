#include<stdio.h> 
#include<stdlib.h> 
#include<glut.h> 
#include<math.h> 
#include <SOIL.h>

static int m = 0, M = 0, v = 0, V = 0, E = 0, e = 0, r = 0, R = 0, j = 0, J = 0, s = 0, S = 0, U = 0, u = 0, n = 0, N = 0, x = 0, z = 0;

static GLint axis = 2;
float zoom = 5.0; // Camera distance for zooming
float angleX = 0.0f, angleY = 0.0f; // Camera angles for rotation
float lastMouseX = 0.0f, lastMouseY = 0.0f; // Track last mouse position
int isDragging = 0; // To check if mouse is dragging

GLfloat diffuseMaterial[4] = { 0.5,0.5,0.5,1.0 };

void drawStars() {
	int numStars = 100;
	glPointSize(2.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < numStars; i++) {
		float xCord = (rand() % 201 - 100) / 10.0;
		float yCord = (rand() % 201 - 100) / 10.0;
		float zCord = (rand() % 201 - 100) / 10.0;

		glColor3f(1.0, 1.0, 1.0);
		glVertex3f(xCord, yCord, zCord);
	}
	glEnd();
}

// initialize material property, light source, lighting model, and depth buffer
void myinit(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	// Material properties
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  // White light for Sun
	GLfloat mat_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat mat_shininess = 25.0;

	// Light properties
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 }; // Sun's position
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  // White light
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; // White specular reflection
	GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };  // Dim ambient light

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void display(void) {
	GLfloat position[] = { 0.0, 0.0, 1.5, 1.0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.5, 0.0);

	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 }; // Sun's position
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Adjusting the view for zoom and rotation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Apply camera rotation
	glRotatef(angleY, 1.0, 0.0, 0.0); // Rotate vertically
	glRotatef(angleX, 0.0, 1.0, 0.0); // Rotate horizontally

	// Draw the Sun
	glPushMatrix();
	glRotatef((float)z, 1.0f, 1.0f, 1.0f); // Rotate sun
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.8, 0.0); // Yellow-orange color for the sun's surface
	glutSolidSphere(0.5, 40, 16); // Render the sphere
	glEnable(GL_LIGHTING);
	glPopMatrix();

	// Mercury
	glPushMatrix();
	glRotatef((float)M, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(1.2f, 0.0f, 0.0f); // Position Mercury
	glRotatef((float)m, 0.0f, 1.0f, 0.0f); // Rotate Mercury
	glColor3f(0.55, 0.57, 0.58); // Mercury color: Gray
	glutSolidSphere(0.04, 20, 8); // Render Mercury
	glPopMatrix();

	// Venus
	glPushMatrix();
	glRotatef((float)V, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(2.0f, 0.0f, 0.0f); // Position Venus
	glRotatef((float)v, 0.0f, 1.0f, 0.0f); // Rotate Venus
	glColor3f(0.76, 0.69, 0.50); // Venus color: Yellowish
	glutSolidSphere(0.05, 20, 8); // Render Venus
	glPopMatrix();

	// Earth
	glPushMatrix();
	glRotatef((float)E, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(2.8f, 0.0f, 0.0f); // Position Earth
	glRotatef((float)e, 0.0f, 1.0f, 0.0f); // Rotate Earth
	glColor3f(0.2, 0.5, 1.0); // Earth color: Blue
	glutSolidSphere(0.053, 20, 8); // Render Earth
	glPopMatrix();

	// Mars
	glPushMatrix();
	glRotatef((float)R, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(3.5f, 0.0f, 0.0f); // Position Mars
	glRotatef((float)r, 0.0f, 1.0f, 0.0f); // Rotate Mars
	glColor3f(1.0, 0.0, 0.0); // Mars color: Red
	glutSolidSphere(0.04, 20, 8); // Render Mars
	glPopMatrix();

	// Jupiter
	glPushMatrix();
	glRotatef((float)J, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(4.5f, 0.0f, 0.0f); // Position Jupiter
	glRotatef((float)j, 0.0f, 1.0f, 0.0f); // Rotate Jupiter
	glColor3f(0.8, 0.5, 0.2); // Jupiter color: Orange-brown
	glutSolidSphere(0.18, 20, 8); // Render Jupiter
	glPopMatrix();

	// Saturn
	glPushMatrix();
	glRotatef((float)S, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(5.5f, 0.0f, 0.0f); // Position Saturn
	glRotatef((float)s, 0.0f, 1.0f, 0.0f); // Rotate Saturn
	glColor3f(0.8, 0.8, 0.5); // Saturn color: Pale yellow
	glutSolidSphere(0.15, 20, 8); // Render Saturn

	// Draw rings
	glColor3f(0.6, 0.6, 0.6); // Saturn's ring color: Gray
	glPushMatrix();
	glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate ring to be horizontal
	glutWireTorus(0.02, 0.25, 30, 30); // Render ring
	glPopMatrix();

	glPopMatrix();

	// Uranus
	glPushMatrix();
	glRotatef((float)U, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(6.5f, 0.0f, 0.0f); // Position Uranus
	glRotatef((float)u, 0.0f, 1.0f, 0.0f); // Rotate Uranus
	glColor3f(0.4, 0.8, 1.0); // Uranus color: Light blue
	glutSolidSphere(0.12, 20, 8); // Render Uranus
	glPopMatrix();

	// Neptune
	glPushMatrix();
	glRotatef((float)N, 0.0f, 1.0f, 0.0f); // Orbit around the sun
	glTranslatef(7.5f, 0.0f, 0.0f); // Position Neptune
	glRotatef((float)n, 0.0f, 1.0f, 0.0f); // Rotate Neptune
	glColor3f(0.0, 0.0, 1.0); // Neptune color: Dark blue
	glutSolidSphere(0.12, 20, 8); // Render Neptune
	glPopMatrix();

	// Draw stars with sun-like color
	drawStars();

	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	int mod = glutGetModifiers();  // Get current modifiers (Ctrl, Shift, Alt)
	float zoomFactor = 0.1f;
	switch (key) {
	case 'z':
		z = (z + 50) % 360;
		glutPostRedisplay();
		break;
	case 'm':
		m = (m + 3) % 360;
		glutPostRedisplay();
		break;
	case 'M':
		M = (M + 12) % 360;
		glutPostRedisplay();
		break;
	case 'v':
		v = (v + 2) % 360;
		glutPostRedisplay();
		break;
	case 'V':
		V = (V + 10) % 360;
		glutPostRedisplay();
		break;
	case 'e':
		e = (e + 5) % 360;
		glutPostRedisplay();
		break;
	case 'E':
		E = (E + 8) % 360;
		glutPostRedisplay();
		break;
	case 'r':
		r = (r + 6) % 360;
		glutPostRedisplay();
		break;
	case 'R':
		R = (R + 6) % 360;
		glutPostRedisplay();
		break;
	case 'j':
		j = (j + 10) % 360;
		glutPostRedisplay();
		break;
	case 'J':
		J = (J + 4) % 360;
		glutPostRedisplay();
		break;
	case 's':
		s = (s + 9) % 360;
		glutPostRedisplay();
		break;
	case 'S':
		S = (S + 3) % 360;
		glutPostRedisplay();
		break;
	case 'u':
		u = (u + 8) % 360;
		glutPostRedisplay();
		break;
	case 'U':
		U = (U + 2) % 360;
		glutPostRedisplay();
		break;
	case 'n':
		n = (n + 7) % 360;
		glutPostRedisplay();
		break;
	case 'N':
		N = (N + 1) % 360;
		glutPostRedisplay();
		break;
	case 'x':
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
		glutPostRedisplay();
		break;
		break;
	case 13:
		//Enter
		z = (z + 50) % 360;
		m = (m + 3) % 360;
		M = (M + 12) % 360;
		v = (v + 2) % 360;
		V = (V - 10) % 360;
		e = (e + 5) % 360;
		E = (E + 8) % 360;
		r = (r + 6) % 360;
		R = (R + 6) % 360;
		j = (j + 10) % 360;
		J = (J + 4) % 360;
		s = (s + 9) % 360;
		S = (S + 3) % 360;
		u = (u + 8) % 360;
		U = (U - 2) % 360;
		n = (n + 7) % 360;
		N = (N + 1) % 360;
		glutPostRedisplay();
		break;
	case 8:
		//backspace
		z = (z - 50) % 360;
		m = (m - 3) % 360;
		M = (M - 12) % 360;
		v = (v - 2) % 360;
		V = (V + 10) % 360;
		e = (e - 5) % 360;
		E = (E - 8) % 360;
		r = (r - 6) % 360;
		R = (R - 6) % 360;
		j = (j - 10) % 360;
		J = (J - 4) % 360;
		s = (s - 9) % 360;
		S = (S - 3) % 360;
		u = (u - 8) % 360;
		U = (U + 2) % 360;
		n = (n - 7) % 360;
		N = (N - 1) % 360;
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		isDragging = 1;  // Start dragging
		lastMouseX = x;
		lastMouseY = y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		isDragging = 0;  // Stop dragging
	}

	// Zoom functionality
	if (button == 3) { // Scroll up
		zoom -= 0.2;
		if (zoom < 1.0) zoom = 1.0; // Limit zoom-in
	}
	else if (button == 4) { // Scroll down
		zoom += 0.2;
		if (zoom > 15.0) zoom = 15.0; // Limit zoom-out
	}
	glutPostRedisplay();
}

void motion(int x, int y) {
	if (isDragging) {
		float deltaX = (x - lastMouseX);
		float deltaY = (y - lastMouseY);

		angleX += deltaX * 0.2f; // Adjust sensitivity as needed
		angleY += deltaY * 0.2f;

		// Limit vertical rotation to avoid flipping
		if (angleY > 90.0f) angleY = 90.0f;
		if (angleY < -90.0f) angleY = -90.0f;

		lastMouseX = x;
		lastMouseY = y;

		glutPostRedisplay(); // Redraw with updated angles
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Solar System");
	myinit();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);  // Register mouse function
	glutMotionFunc(motion); // Register motion function for dragging
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}