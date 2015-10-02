#include "glFrame.h"
#include <string>
#include <sstream>


GLFrame::GLFrame()
	: faceRot(false), sceneRot(false)
{
	ZeroMemory(&drag, sizeof(drag));
	cube = new Cube();
}

void GLFrame::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	float lightPos[4] = { 0.0f, 10.0f, 10.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	viewer.ApplyTransform();
	cube->Render();
	glPopMatrix();
}

void GLFrame::GetBlockById(int id, BlockDesc &b)
{
	int blockIndex = id & 0x1f;
	b.pos = Point3<int>(
		blockIndex / 9,
		(blockIndex / 3) % 3,
		blockIndex % 3
	);

	b.side = 0;
	for (int i = 0 ; i < 6; i++)
		if (id & (1 << (i+5))) {
			b.side = i;
			break;
		}
}

bool GLFrame::GetBlockUnderMouse(int winX, int winY, BlockDesc &b)
{
	const int selBufferSize = 27*3*4;
	static UINT selectBuffer[selBufferSize] = { };
	int hitCount = 0, viewport[4] = { };

	glSelectBuffer(selBufferSize, selectBuffer);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glRenderMode(GL_SELECT);

	glLoadIdentity();
	gluPickMatrix(winX, viewport[3] - winY, 2, 2, viewport);
	SetPerspective(viewport[2], viewport[3]);

	glDisable(GL_CULL_FACE);
	RenderScene();
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	hitCount = glRenderMode(GL_RENDER);
	if (hitCount <= 0) return false;

	UINT minZ = UINT_MAX;
	UINT id = 0;
	for (int i = 0; i < hitCount; i++) {
		UINT z = selectBuffer[i*4+1];
		if (z <= minZ) {
			id = selectBuffer[i*4+3];
			minZ = z;
		}
	}

	GetBlockById(id, b);
	return true;
}

void GLFrame::GetNeighbors(const BlockDesc &b, Point3<int> &neighbor1, Point3<int> &neighbor2)
{
	int i1, i2, i3;
	const int *p = b.pos.data;
	int *n1 = neighbor1.data, *n2 = neighbor2.data;

	if (b.side == 0 || b.side == 2) {
		i1 = 1; i2 = 2; i3 = 0;
	} else if (b.side == 1 || b.side == 3) {
		i1 = 1; i2 = 0; i3 = 2;
	} else {
		i1 = 2; i2 = 0; i3 = 1;
	}

	n1[i3] = n2[i3] = p[i3];

	n1[i1] = p[i1] == 2 ? 1 : p[i1] + 1;
	n1[i2] = p[i2];
	n2[i2] = p[i2] == 2 ? 1 : p[i2] + 1;
	n2[i1] = p[i1];

	drag.face[0] = (Axis)i1;
	drag.index[0] = p[i1];
	drag.face[1] = (Axis)i2;
	drag.index[1] = p[i2];

	if (p[i1] == 2) drag.neg[0] = true;
	if (p[i2] == 2) drag.neg[1] = true;
}

Vector3d GLFrame::CalcBlockWinPos(const Point3<int> &b,
	const Matrix44d &modelview, const Matrix44d &projection, int viewport[4])
{
	static const float d = (cube->blockSize + cube->blockSpace) * 0.5f;

	Vector3d ret;
	Vector3f l = cube->blocks[b.x][b.y][b.z]->location;
	l.x -= d;
	gluProject(l.x, l.y, l.z, modelview.data, projection.data, viewport, &ret.x, &ret.y, &ret.z);
	return ret;
};

void GLFrame::CalcRotDirections(const BlockDesc &b)
{
	Point3<int> n1, n2;
	Vector3d target;

	GetNeighbors(b, n1, n2);

	Matrix44d modelview(viewer.Modelview());
	double projection[16];
	int viewport[4];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	target = CalcBlockWinPos(b.pos, modelview, projection, viewport);
	target.y = viewport[3] - target.y;

	for (int i = 0; i < 2; i++)
	{
		drag.dir[i] = CalcBlockWinPos(i ? n2 : n1, modelview, projection, viewport);
		drag.dir[i].y = viewport[3] - drag.dir[i].y;
		drag.dir[i] -= target;
		drag.dir[i].Normalize();

		if (drag.neg[i]) drag.dir[i] = -drag.dir[i];

		if (b.side == 1 || b.side == 2 || b.side == 4) {
			drag.dir[i] = -drag.dir[i];
		}
	}
}

void GLFrame::MouseRot(const Vector3d &mouseDir)
{
	double a1 = acos(Vector3d::Dot(mouseDir, drag.dir[0]));
	double a2 = acos(Vector3d::Dot(mouseDir, drag.dir[1]));

	double c[4] = {
		a1, M_PI - a1,
		a2, M_PI - a2,
	};

	int k = 0;
	for (int i = 0; i < 4; i++) {
		if (c[i] < c[k]) k = i;
	}

	MoveDesc m = { };
	if (k <= 1) {
		m.normal = drag.face[1];
		m.index = drag.index[1];
		m.clockWise = k == 1;
	}
	else {
		m.normal = drag.face[0];
		m.index = drag.index[0];
		m.clockWise = k == 2;
	}
	cube->BeginRotateFace(m.normal, m.index, m.clockWise);
	history.Add(m);
	SendMessage(GetParent(m_hwnd), WM_ROTATEFACE, 0, 0);
}

void GLFrame::SetPerspective(int w, int h)
{
	viewer.SetPerspective(60.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -200.0f), w, h);
}

void GLFrame::OnCreate()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_FLAT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	float ambient[4]  = { 0.4f, 0.4f, 0.4f, 1.0f };
	float diffuse[4]  = { 0.8f, 0.8f, 0.8f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	SetTimer(m_hwnd, 1, 25, NULL);
	viewer.qRotation *= Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 30.0f);
	viewer.qRotation *= Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 45.0f);
}

void GLFrame::OnDisplay()
{
	RenderScene();
}

void GLFrame::OnTimer()
{
	if (cube->AnimationStep())
		RedrawWindow();
}

void GLFrame::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	SetPerspective(w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLFrame::OnMouseDown(MouseButton button, int winX, int winY)
{
	if (button == MouseButton::RBUTTON) {
		viewer.BeginRotate(winX, winY);
	}
	else if (button == MouseButton::LBUTTON)
	{
		BlockDesc block;
		ZeroMemory(&drag, sizeof(drag));
		if (!GetBlockUnderMouse(winX, winY, block)) {
			viewer.BeginRotate(winX, winY);
			sceneRot = true;
		}
		else if (!cube->IsAnim()) {
			CubeBlock *b = cube->blocks[block.pos.x][block.pos.y][block.pos.z];
			if (b->IsSideColored(block.side)) {
				CalcRotDirections(block);
				mousePos = Point2i(winX, winY);
				faceRot = true;
			}
		}
	}
}

void GLFrame::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (keysPressed & KeyModifiers::KM_RBUTTON || sceneRot) {
		viewer.Rotate(x, y);
		RedrawWindow();
	}
	else if (keysPressed & KeyModifiers::KM_LBUTTON && faceRot) {
		
		Vector3d v(x - mousePos.x, y - mousePos.y, 0);
		if (v.Length() >= 10)
		{
			v.Normalize();
			MouseRot(v);
			faceRot = false;
		}
	}
}

void GLFrame::OnMouseUp(MouseButton button, int x, int y)
{
	sceneRot = false;
}

void GLFrame::OnDestroy()
{
	KillTimer(m_hwnd, 1);
	delete cube;
}