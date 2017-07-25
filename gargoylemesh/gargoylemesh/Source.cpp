
#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<sstream>
#include<cstring>
#include<string>
#include<fstream>
#include<vector>

#include<GL/glut.h>
#include <GL/glm/glm.hpp>
#include <windows.h> 
#include <GL/Leap.h>
#include "vertex.h"

using std::vector;
using namespace std;
using namespace Leap;

#define NUM_LIGHTS 3

//Global Variables
int num_of_faces = 0;
int num_of_vertices = 0;
float  x_angle = 0.0f,y_angle = 0.0f, xinc = 0.0f, yinc = 0.0f, xscc= 1.0f, yscc = 1.0f, zscc = 1.0f,xsc = 1.0f, ysc = 1.0f, zsc = 1.0f;
typedef float lightMats[4];
float  meshlength, meshheight, meshwidth, x_min, x_max, y_min, y_max, z_min, z_max, camera_top, x_axis_width, y_axis_width, z_axis_width;
static float farZ;


// Lightning Parameters
//Refernce: https://www.cse.msu.edu/~cse872/tutorial3.html

float  lightposition[NUM_LIGHTS][4] = { { 0.2f, 0.8f,0.8f, 0.0f}, {0.3f,-0.7f,0.5f, 0.0f} , { -1.0f,0.2f,0.2f, 0.0f } };

float lightcolors_ads[3][4] = { { 0.7f, 0.7f, 0.7f, 1.0f },{ 0.7f, 0.7f, 0.7f, 1.0f },{ 0.9f, 0.9f, 0.9f, 1.0f } };

GLfloat cyan[] = { 0.f, .8f, .8f, 1.f };
GLfloat white[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat red[] = {0.0f,0.0f, 0.0f, 1.0f };
GLfloat shininess= { 50 };


class SampleListener : public Listener {
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};


void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;

	controller.enableGesture(Gesture::TYPE_SWIPE);
}


void SampleListener::onFrame(const Controller& controller)
{
	const Frame frame = controller.frame();
	HandList hands = frame.hands();
	if (!frame.hands().isEmpty())
	{
		if (hands.count() == 1)
		{
			const Hand hand = *hands.begin();
			const Vector normal = hand.palmNormal();
			const Vector direction1 = hand.direction();
			if (hand.isLeft())
			{
				if (hand.sphereRadius() > 40/* && hand.sphereRadius() > 0*/)
				{
					xinc = direction1[0];
					yinc = direction1[1];
				}
			}
		}
		else if (hands.count() == 2)
		{
			const Hand hand = hands.rightmost();
			const Vector direction = hand.direction();
			if (hand.sphereRadius() < 40 && hand.sphereRadius() > 0)
			{
				if (direction[1] > 0)
				{
					xsc = (direction[1]);
					ysc = (direction[1]);
					zsc = (direction[1]);
				}
			}
			else
			{
				y_angle = direction.pitch() * RAD_TO_DEG;
				x_angle = direction.yaw() * RAD_TO_DEG;
			}
		}
	}
}

SampleListener listener;
Controller controller;
	
void import_data() {
	string line;
	int i = 0;
	string type;
	ifstream myfile("./bunny.m");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			istringstream in(line);
			string type;
			in >> type;
			if (type == "Vertex")
			{
				in >> vertices[num_of_vertices].vertexID >> vertices[num_of_vertices].x >> vertices[num_of_vertices].y >> vertices[num_of_vertices].z;
				if (num_of_vertices == 0)
				{
					x_min = vertices[num_of_vertices].x;
					x_max = vertices[num_of_vertices].x;
					y_min = vertices[num_of_vertices].y;
					y_max = vertices[num_of_vertices].y;
					z_min = vertices[num_of_vertices].z;
					z_max = vertices[num_of_vertices].z;
				}
				else
				{
					if (vertices[num_of_vertices].x> x_max)
						x_max = vertices[num_of_vertices].x;
					if (vertices[num_of_vertices].x < x_min)
						x_min = vertices[i].x;

					if (vertices[num_of_vertices].y > y_max)
						y_max = vertices[num_of_vertices].y;
					if (vertices[num_of_vertices].y <y_min)
						y_min = vertices[i].y;

					if (vertices[num_of_vertices].z > z_max);
					z_max = vertices[num_of_vertices].z;
					if (vertices[num_of_vertices].z< z_min)
						z_min = vertices[num_of_vertices].z;
				}
				size_t pos0 = line.find_first_of('(');
				size_t pos1 = line.find_last_not_of(')');
				line = line.substr(pos0 + 1, pos1);
				istringstream in2(line);
				in2 >> vertices[num_of_vertices].Vertex_norm.x >> vertices[num_of_vertices].Vertex_norm.y >> vertices[num_of_vertices].Vertex_norm.z;
				num_of_vertices++;
			}
			else if (type == "Face") {
				in >> Faces[num_of_faces].faceID >> Faces[num_of_faces].x >> Faces[num_of_faces].y >> Faces[num_of_faces].z;
				num_of_faces++;
			}
			else {
			}
		}
		myfile.close();
		meshlength = x_max-x_min;
		meshwidth = z_max-z_min;
		meshheight = y_max-y_min;
		x_axis_width = meshlength*1.8;
		y_axis_width = meshheight*1.8;
		z_axis_width = meshwidth*1.8;
	}
	else
	{
		std::cout << "Error opening file!"<<std::endl;
	}
}

void initialize() {
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (z_axis_width < 1) {
		yscc = xscc = zscc = 1 / z_axis_width;
		camera_top = 2.44;
	}
	else {
		yscc = xscc = zscc = 1.0f;
		camera_top = z_axis_width;
	}
	gluLookAt(x_axis_width, y_axis_width, camera_top, x_max, y_max, z_max, 0, 1, 0);
}

void doReshape(int w, int h) {
	/*
	if ((x_axis_width + y_axis_width + z_axis_width) < 100) 
	{
		farZ = 100;
	}
	else 
	{
		farZ = 10 * (x_axis_width + y_axis_width + z_axis_width);
	}
	if (h == 0) 
	{
		h = 1;
	}
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, farZ);
	*/
	if (h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
	}


void setup()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT0, GL_POSITION,lightposition[0]);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightcolors_ads[0]);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolors_ads[1]);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcolors_ads[2]);

	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT1, GL_POSITION, lightposition[1]);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightcolors_ads[0]);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcolors_ads[0]);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightcolors_ads[0]);
	
	glEnable(GL_LIGHT2);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT2, GL_POSITION, lightposition[2]);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightcolors_ads[0]);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightcolors_ads[0]);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightcolors_ads[0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}
void draw_grid_n_axes()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glLineWidth(6);
	glBegin(GL_LINES);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(x_axis_width, 0, 0);


	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, y_axis_width, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, z_axis_width);

	glEnd();
	glEnable(GL_LINE_SMOOTH);
	
	glLineWidth(2);
	glDisable(GL_LINE_SMOOTH);

	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for (float i = -x_axis_width; i <= x_axis_width; i = i + (x_axis_width / 10))
	{
		glVertex3f(i, 0, x_axis_width);
		glVertex3f(i, 0, -x_axis_width);
		glVertex3f(x_axis_width, 0, i);
		glVertex3f(-x_axis_width, 0, i);
	}
	glEnd();
	glEnable(GL_LINE_SMOOTH);
}

void draw_mesh() {

	int vertex_id, i, j;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	for (i = 0; i< num_of_faces; i++)
	{
		glShadeModel(GL_SMOOTH);
		glBegin(GL_TRIANGLES);
		for (j = 0; j < 3; j++)
		{
			switch (j)
			{
			case 0:
				vertex_id = Faces[i].x;
				break;
			case 1:
				vertex_id = Faces[i].y;
				break;
			case 2:
				vertex_id = Faces[i].z;
				break;
			}
			glNormal3f(vertices[vertex_id - 1].Vertex_norm.x, vertices[vertex_id - 1].Vertex_norm.y, vertices[vertex_id - 1].Vertex_norm.y);
			glVertex3f(vertices[vertex_id - 1].x, vertices[vertex_id - 1].y, vertices[vertex_id - 1].z);
		}
		glEnd();
	}
}

void display() {
	initialize();
	setup();
	glPushMatrix();
	glScalef(0.9, 0.9, 0.9);
	glScalef(xscc,yscc,zscc);
	glTranslatef(0, -0.034, 0);
	draw_grid_n_axes();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(xinc, yinc, 0.0f);
	glRotatef(x_angle, 0, 1, 0);
	glRotatef(y_angle, 1, 0, 0);
	glScalef(xsc, ysc, zsc);
	glScalef(xscc, yscc, zscc);
	draw_mesh();
	glutPostRedisplay();
	glPopMatrix();
	glutSwapBuffers();
}

//Main
int main(int argc, char** argv) 
{
	import_data();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
    glutCreateWindow("Bunny Mesh Viewer");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glutDisplayFunc(display); 
	glutReshapeFunc(doReshape);
	controller.addListener(listener);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}


