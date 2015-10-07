#include "cubeblock.h"

Color3b CubeBlock::colors[6] =
{
	Color3b(0,255,0),
	Color3b(255,150,0),
	Color3b(0,0,255),
	Color3b(255,0,0),
	Color3b(255,255,255),
	Color3b(255,255,0),
};

const float CubeBlock::size = 30.0f;
bool CubeBlock::fRenderPickMode = false;

VertexBuffer *CubeBlock::edges = NULL;
VertexBuffer *CubeBlock::faces = NULL;
VertexBuffer *CubeBlock::faces_pickMode = NULL;

CubeBlock::CubeBlock(UINT pickId)
	: pickId(pickId), numSides(0)
{
	memset(&clr, 0, sizeof(clr));
	memset(coloredSides, 0, sizeof(coloredSides));
	for (int i = 0; i < 6; i++)
		sideId[i] = pickId | (1 << (i+5));

	static int init = InitVertices();
}

CubeBlock::~CubeBlock()
{
	if (edges) {
		delete edges;
		edges = NULL;
	}
	if (faces) {
		delete faces;
		faces = NULL;
	}
	if (faces_pickMode) {
		delete faces;
		faces = NULL;
	}
}

int CubeBlock::InitVertices()
{
	float s = size / 2;
	float s2 = s - 1.45f;
	float edges_verts[][3] =
	{
		-s2, -s2, s,
		-s2, s2, s,
		-s, s2, s2,
		-s, -s2, s2,
		-s2, s2, -s,
		-s2, -s2, -s,
		-s, -s2, -s2,
		-s, s2, -s2,
		-s2, s, s2,
		-s2, s, -s2,
		-s, s2, -s2,
		-s, s2, s2,
		-s2, -s, -s2,
		-s2, -s, s2,
		-s, -s2, s2,
		-s, -s2, -s2,
		-s2, s, -s2,
		s2, s, -s2,
		s2, s2, -s,
		-s2, s2, -s,
		-s2, -s2, -s,
		s2, -s2, -s,
		s2, -s, -s2,
		-s2, -s, -s2,
		s2, -s2, -s,
		s2, s2, -s,
		s, s2, -s2,
		s, -s2, -s2,
		s2, s, -s2,
		s2, s, s2,
		s, s2, s2,
		s, s2, -s2,
		s2, s2, s,
		s2, -s2, s,
		s, -s2, s2,
		s, s2, s2,
		s, -s2, -s2,
		s, -s2, s2,
		s2, -s, s2,
		s2, -s, -s2,
		s2, s, s2,
		-s2, s, s2,
		-s2, s2, s,
		s2, s2, s,
		s2, -s2, s,
		-s2, -s2, s,
		-s2, -s, s2,
		s2, -s, s2,

		-s, -s2, s2,
		-s2, -s, s2,
		-s2, -s2, s,
		-s, s2, s2,
		-s2, s2, s,
		-s2, s, s2,
		-s, s2, -s2,
		-s2, s, -s2,
		-s2, s2, -s,
		-s, -s2, -s2,
		-s2, -s2, -s,
		-s2, -s, -s2,
		s, -s2, -s2,
		s2, -s, -s2,
		s2, -s2, -s,
		s, s2, -s2,
		s2, s2, -s,
		s2, s, -s2,
		s, s2, s2,
		s2, s, s2,
		s2, s2, s,
		s, -s2, s2,
		s2, -s2, s,
		s2, -s, s2,
	};

	float face_verts[][3] =
	{
		-s, -s2, s2,
		-s, s2, s2,
		-s, s2, -s2,
		-s, -s2, -s2,
		-s2, -s2, -s,
		-s2, s2, -s,
		s2, s2, -s,
		s2, -s2, -s,
		s, -s2, -s2,
		s, s2, -s2,
		s, s2, s2,
		s, -s2, s2,
		s2, -s2, s,
		s2, s2, s,
		-s2, s2, s,
		-s2, -s2, s,
		-s2, s, -s2,
		-s2, s, s2,
		s2, s, s2,
		s2, s, -s2,
		-s2, -s, -s2,
		s2, -s, -s2,
		s2, -s, s2,
		-s2, -s, s2,
	};

	float face_pick_verts[][3] =
	{
		-s, -s, s,
		-s, s, s,
		-s, s, -s,
		-s, -s, -s,
		-s, -s, -s,
		-s, s, -s,
		s, s, -s,
		s, -s, -s,
		s, -s, -s,
		s, s, -s,
		s, s, s,
		s, -s, s,
		s, -s, s,
		s, s, s,
		-s, s, s,
		-s, -s, s,
		-s, s, -s,
		-s, s, s,
		s, s, s,
		s, s, -s,
		-s, -s, -s,
		s, -s, -s,
		s, -s, s,
		-s, -s, s,
	};

	edges = new VertexBuffer;
	faces = new VertexBuffer;
	faces_pickMode = new VertexBuffer;

	edges->Bind(GL_ARRAY_BUFFER);
	edges->SetData(sizeof(edges_verts), edges_verts, GL_STATIC_DRAW);

	faces->Bind(GL_ARRAY_BUFFER);
	faces->SetData(sizeof(face_verts), face_verts, GL_STATIC_DRAW);

	faces_pickMode->Bind(GL_ARRAY_BUFFER);
	faces_pickMode->SetData(sizeof(face_pick_verts), face_pick_verts, GL_STATIC_DRAW);
	return 0;
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

	glEnableClientState(GL_VERTEX_ARRAY);
	(fRenderPickMode ? faces_pickMode : faces)->Bind(GL_ARRAY_BUFFER);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	
	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3b *c = GetSideColor(i);
			if (c) {
				glColor3ubv(c->data);
			} else glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			GLubyte r = sideId[i] & 0xff;
			GLubyte g = sideId[i] >> 8;
			glColor3ub(r, g, 1);
		}

		glDrawArrays(GL_QUADS, i*4, 4);
	}

	if (!fRenderPickMode) {
		glColor3f(0.0f, 0.0f, 0.0f);
		edges->Bind(GL_ARRAY_BUFFER);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_QUADS, 0, 48);
		glDrawArrays(GL_TRIANGLES, 48, 24);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}