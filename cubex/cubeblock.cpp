#include "cubeblock.h"
#include "resources.h"

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

float *CubeBlock::verts = NULL;
VertexBuffer *CubeBlock::faces = NULL;
VertexBuffer *CubeBlock::faces_pickMode = NULL;
VertexBuffer *CubeBlock::edges = NULL;

CubeBlock::CubeBlock(UINT pickId)
	: pickId(pickId), numSides(0)
{
	memset(&clr, 0, sizeof(clr));
	memset(coloredSides, 0, sizeof(coloredSides));
}

void CubeBlock::InitStatic()
{
	verts = ReadVertexData(GetModuleHandle(NULL), IDR_BLOCKDATA);

	if (GLEW_ARB_vertex_buffer_object) {
		faces = new VertexBuffer;
		faces_pickMode = new VertexBuffer;
		edges = new VertexBuffer;

		faces->Bind();
		faces->SetData(72*sizeof(float), verts, GL_STATIC_DRAW);

		faces_pickMode->Bind();
		faces_pickMode->SetData(72*sizeof(float), verts+72, GL_STATIC_DRAW);

		edges->Bind();
		edges->SetData(216*sizeof(float), verts+144, GL_STATIC_DRAW);

		delete [] verts;
		verts = NULL;
	}
}

void CubeBlock::FreeStatic()
{
	if (faces) {
		delete faces; faces = NULL;
	}
	if (faces_pickMode) {
		delete faces; faces = NULL;
	}
	if (edges) {
		delete edges; edges = NULL;
	}
	if (verts) {
		delete verts; verts = NULL;
	}
}

float *CubeBlock::ReadVertexData(HINSTANCE hInst, DWORD resourceId)
{
	HRSRC hResInfo = FindResource(hInst, MAKEINTRESOURCE(resourceId), RT_RCDATA);
	BYTE *inp = (BYTE *)LockResource(LoadResource(hInst, hResInfo));

	int inpSize = SizeofResource(hInst, hResInfo);
	int i = 0;
	int vp = 0;

	int size = 0;
	while (isdigit(inp[i]))
		size = inp[i++] - '0' + size*10;
	i++;

	float *vertexData = new float[size];

	while (i < inpSize) 
	{
		int sign = 1;
		int n = 0;
		char frac[10] = "0.";
		int j = 2;

		if (inp[i] == '-') {
			sign = -1; i++;
		}
		if (isdigit(inp[i])) {
			do {
				n = (inp[i++] - '0') + n*10;
			} while (isdigit(inp[i]));
		}
		else {
			i++; continue;
		}

		if (inp[i] == '.') {
			i++;
			while (isdigit(inp[i])) {
				frac[j++] = inp[i++];
			}
		}
		i++;
		vertexData[vp++] = sign * (n + (float)atof(frac));
	}
	return vertexData;
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
	if (GLEW_ARB_vertex_buffer_object) {
		(fRenderPickMode ? faces_pickMode : faces)->Bind();
		glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	else glVertexPointer(3, GL_FLOAT, 0, verts + (fRenderPickMode ? 72 : 0));
	
	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3b *c = GetSideColor(i);
			if (c) {
				glColor3ubv(c->data);
			} else glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			int id = pickId | (1 << (i+10));
			GLubyte r = id & 0xff;
			GLubyte g = id >> 8;
			glColor3ub(r, g, 1);
		}

		glDrawArrays(GL_QUADS, i*4, 4);
	}

	if (!fRenderPickMode) {
		glColor3f(0.0f, 0.0f, 0.0f);

		if (GLEW_ARB_vertex_buffer_object) {
			edges->Bind();
			glVertexPointer(3, GL_FLOAT, 0, 0);
		}
		else glVertexPointer(3, GL_FLOAT, 0, verts + 144);

		glDrawArrays(GL_QUADS, 0, 48);
		glDrawArrays(GL_TRIANGLES, 48, 24);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}