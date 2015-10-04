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

bool CubeBlock::fRenderPickMode = false;

CubeBlock::CubeBlock(float size, UINT pickId)
	: size(size), pickId(pickId), numSides(0), edgeSize(1.45f)
{
	memset(&clr, 0, sizeof(clr));
	memset(coloredSides, 0, sizeof(coloredSides));
	for (int i = 0; i < 6; i++)
		sideId[i] = pickId | (1 << (i+5));
}

bool CubeBlock::IsSideColored(int side)
{
	bool ret = false;
	for (int i = 0; i < numSides; i++)
		if (coloredSides[i] == side)
			return true;
	return false;
}

void CubeBlock::Render()
{
	glPushMatrix();
	this->ApplyTransform();

	Color3b *c = NULL;
	float s = size * 0.5f;
	float s2 = s - (fRenderPickMode ? 0.0f : edgeSize);

	if (!fRenderPickMode) DrawEdges(s, s2);

	glInitNames();
	glPushName(0);

	glLoadName(sideId[0]);
	if (c = GetSideColor(0))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-s, -s2, s2);
		glVertex3f(-s, s2, s2);
		glVertex3f(-s, s2, -s2);
		glVertex3f(-s, -s2, -s2);
	glEnd();

	glLoadName(sideId[1]);
	if (c = GetSideColor(1))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-s2, -s2, -s);
		glVertex3f(-s2, s2, -s);
		glVertex3f(s2, s2, -s);
		glVertex3f(s2, -s2, -s);
	glEnd();
	
	glLoadName(sideId[2]);
	if (c = GetSideColor(2))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(s, -s2, -s2);
		glVertex3f(s, s2, -s2);
		glVertex3f(s, s2, s2);
		glVertex3f(s, -s2, s2);
	glEnd();
		
	glLoadName(sideId[3]);
	if (c = GetSideColor(3))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(s2, -s2, s);
		glVertex3f(s2, s2, s);
		glVertex3f(-s2, s2, s);
		glVertex3f(-s2, -s2, s);
	glEnd();

	glLoadName(sideId[4]);
	if (c = GetSideColor(4))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-s2, s, -s2);
		glVertex3f(-s2, s, s2);
		glVertex3f(s2, s, s2);
		glVertex3f(s2, s, -s2);
	glEnd();

	glLoadName(sideId[5]);
	if (c = GetSideColor(5))
		glColor3ubv(c->data);
	else glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-s2, -s, -s2);
		glVertex3f(s2, -s, -s2);
		glVertex3f(s2, -s, s2);
		glVertex3f(-s2, -s, s2);
	glEnd();

	glPopMatrix();
}

void CubeBlock::DrawEdges(float s, float s2)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(-s2, -s2, s);
		glVertex3f(-s2, s2, s);
		glVertex3f(-s, s2, s2);
		glVertex3f(-s, -s2, s2);

		glVertex3f(-s2, s2, -s);
		glVertex3f(-s2, -s2, -s);
		glVertex3f(-s, -s2, -s2);
		glVertex3f(-s, s2, -s2);

		glVertex3f(-s2, s, s2);
		glVertex3f(-s2, s, -s2);
		glVertex3f(-s, s2, -s2);
		glVertex3f(-s, s2, s2);

		glVertex3f(-s2, -s, -s2);
		glVertex3f(-s2, -s, s2);
		glVertex3f(-s, -s2, s2);
		glVertex3f(-s, -s2, -s2);

		glVertex3f(-s2, s, -s2);
		glVertex3f(s2, s, -s2);
		glVertex3f(s2, s2, -s);
		glVertex3f(-s2, s2, -s);

		glVertex3f(-s2, -s2, -s);
		glVertex3f(s2, -s2, -s);
		glVertex3f(s2, -s, -s2);
		glVertex3f(-s2, -s, -s2);

		glVertex3f(s2, -s2, -s);
		glVertex3f(s2, s2, -s);
		glVertex3f(s, s2, -s2);
		glVertex3f(s, -s2, -s2);

		glVertex3f(s2, s, -s2);
		glVertex3f(s2, s, s2);
		glVertex3f(s, s2, s2);
		glVertex3f(s, s2, -s2);
		
		glVertex3f(s2, s2, s);
		glVertex3f(s2, -s2, s);
		glVertex3f(s, -s2, s2);
		glVertex3f(s, s2, s2);

		glVertex3f(s, -s2, -s2);
		glVertex3f(s, -s2, s2);
		glVertex3f(s2, -s, s2);
		glVertex3f(s2, -s, -s2);

		glVertex3f(s2, s, s2);
		glVertex3f(-s2, s, s2);
		glVertex3f(-s2, s2, s);
		glVertex3f(s2, s2, s);

		glVertex3f(s2, -s2, s);
		glVertex3f(-s2, -s2, s);
		glVertex3f(-s2, -s, s2);
		glVertex3f(s2, -s, s2);
	glEnd();

	glBegin(GL_TRIANGLES);
		glVertex3f(-s, -s2, s2);
		glVertex3f(-s2, -s, s2);
		glVertex3f(-s2, -s2, s);

		glVertex3f(-s, s2, s2);
		glVertex3f(-s2, s2, s);
		glVertex3f(-s2, s, s2);

		glVertex3f(-s, s2, -s2);
		glVertex3f(-s2, s, -s2);
		glVertex3f(-s2, s2, -s);

		glVertex3f(-s, -s2, -s2);
		glVertex3f(-s2, -s2, -s);
		glVertex3f(-s2, -s, -s2);

		glVertex3f(s, -s2, -s2);
		glVertex3f(s2, -s, -s2);
		glVertex3f(s2, -s2, -s);

		glVertex3f(s, s2, -s2);
		glVertex3f(s2, s2, -s);
		glVertex3f(s2, s, -s2);

		glVertex3f(s, s2, s2);
		glVertex3f(s2, s, s2);
		glVertex3f(s2, s2, s);

		glVertex3f(s, -s2, s2);
		glVertex3f(s2, -s2, s);
		glVertex3f(s2, -s, s2);
	glEnd();
}