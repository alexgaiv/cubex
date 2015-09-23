#include "cubeblock.h"

void CubeBlock::Render()
{
	glPushMatrix();
	this->ApplyTransform();

	float s = size / 2;

	glInitNames();
	glPushName(0);
	glBegin(GL_QUADS);
		glColor3f(0.3f, 0.3f, 0.3f);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-s, -s, -s);
		glVertex3f(s, -s, -s);
		glVertex3f(s, -s, s);
		glVertex3f(-s, -s, s);

		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-s, -s, -s);
		glVertex3f(-s, s, -s);
		glVertex3f(s, s, -s);
		glVertex3f(s, -s, -s);
		
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(s, -s, -s);
		glVertex3f(s, s, -s);
		glVertex3f(s, s, s);
		glVertex3f(s, -s, s);
	glEnd();
		
	glLoadName(GetSideId(0));
	glBegin(GL_QUADS);
		if (numSides >= 1)
			glColor3ubv(clr.sides[0].data);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(s, -s, s);
		glVertex3f(s, s, s);
		glVertex3f(-s, s, s);
		glVertex3f(-s, -s, s);
	glEnd();
	
	glLoadName(GetSideId(1));
	glBegin(GL_QUADS);
		if (numSides == 3)
			glColor3ubv(clr.sides[swapSides ? 1 : 2].data);
		else glColor3f(0.3f, 0.3f, 0.3f);
		
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-s, -s, s);
		glVertex3f(-s, s, s);
		glVertex3f(-s, s, -s);
		glVertex3f(-s, -s, -s);
	glEnd();

	glLoadName(GetSideId(2));
	glBegin(GL_QUADS);
		if (numSides >= 2)
			glColor3ubv(clr.sides[swapSides ? 2 : 1].data);
		else glColor3f(0.3f, 0.3f, 0.3f);
		
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-s, s, -s);
		glVertex3f(-s, s, s);
		glVertex3f(s, s, s);
		glVertex3f(s, s, -s);
	glEnd();

	glPopMatrix();
}