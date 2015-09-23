#include "cubeblock.h"

Color3b CubeBlock::colors[6] =
{
	Color3b(0,250,0),
	Color3b(255,90,0),
	Color3b(0,0,255),
	Color3b(255,0,0),
	Color3b(255,255,255),
	Color3b(255,255,0),
};

void CubeBlock::Render()
{
	glPushMatrix();
	this->ApplyTransform();

	float s = size / 2;
	Color3b *c = NULL;

	glInitNames();
	glPushName(42);

	glLoadName(GetSideId(0));
	if (c = GetSideColor(0))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-s, -s, s);
		glVertex3f(-s, s, s);
		glVertex3f(-s, s, -s);
		glVertex3f(-s, -s, -s);
	glEnd();

	glLoadName(GetSideId(1));
	if (c = GetSideColor(1))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-s, -s, -s);
		glVertex3f(-s, s, -s);
		glVertex3f(s, s, -s);
		glVertex3f(s, -s, -s);
	glEnd();
	
	glLoadName(GetSideId(2));
	if (c = GetSideColor(2))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(s, -s, -s);
		glVertex3f(s, s, -s);
		glVertex3f(s, s, s);
		glVertex3f(s, -s, s);
	glEnd();
		
	glLoadName(GetSideId(3));
	if (c = GetSideColor(3))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(s, -s, s);
		glVertex3f(s, s, s);
		glVertex3f(-s, s, s);
		glVertex3f(-s, -s, s);
	glEnd();

	glLoadName(GetSideId(4));
	if (c = GetSideColor(4))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-s, s, -s);
		glVertex3f(-s, s, s);
		glVertex3f(s, s, s);
		glVertex3f(s, s, -s);
	glEnd();

	glLoadName(GetSideId(5));
	if (c = GetSideColor(5))
		glColor3ubv(c->data);
	else glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-s, -s, -s);
		glVertex3f(s, -s, -s);
		glVertex3f(s, -s, s);
		glVertex3f(-s, -s, s);
	glEnd();

	glPopMatrix();
}