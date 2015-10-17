#include "glframe.h"
#include <string>
#include <strsafe.h>
#include <mmsystem.h>

#pragma comment(lib, "Winmm.lib")

Quaternion GLFrame::qResetView =
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 25.0f) *
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 45.0f);

GLFrame::GLFrame()
{
	solveTime = 0;
	wasMixed = false;
	numMoves = 0;
	fSolvedAnim = false;
	rotAngle = 0.0f;
	faceDrag = sceneDrag = false;
	isFaceRotating = isMixing = false;
	needRedraw = false;
	ZeroMemory(&drag, sizeof(drag));
}

void GLFrame::ResetCube()
{
	if (!resetAnim.IsComplete()) return;

	if (fSolvedAnim)
		viewer.qRotation *= Quaternion(Vector3f(1.0f, 1.0f, 1.0f), rotAngle);

	isMixing = false;
	fSolvedAnim = false;
	wasMixed = false;
	numMoves = 0;
	solveTime = time(NULL);

	cube->Reset();
	history.Clear();
	resetAnim.Setup(viewer.qRotation, qResetView, 0.1f);
}

void GLFrame::MixUpCube()
{
	if (fSolvedAnim && resetAnim.IsComplete())
		viewer.qRotation *= Quaternion(Vector3f(1.0f, 1.0f, 1.0f), rotAngle);

	isMixing = true;
	fSolvedAnim = false;
	wasMixed = true;
	numMoves = 0;

	cube->MixUp();
	history.Clear();
}

void GLFrame::CancelMove() {
	if (!cube->IsAnim()) {
		MoveDesc *m = history.Pop();
		if (m) {
			cube->BeginRotateFace(m->normal, m->index, !m->clockWise);	
			numMoves--;
		}
	}
}

void GLFrame::SetPerspective(int w, int h) {
	viewer.SetPerspective(45.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -270.0f), w, h);
}

void GLFrame::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	viewer.ApplyTransform();
	if (fSolvedAnim) {
		glRotatef(rotAngle, 1.0f, 1.0f, 1.0f);
	}

	cube->Render();

	if (fSolvedAnim && !CubeBlock::fRenderPickMode)
	{
		RECT winRect = { };
		GetClientRect(m_hwnd, &winRect);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, winRect.right - winRect.left, winRect.bottom - winRect.top, 0.0, -1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(0.07f, 0.31f, 0.76f);
		glListBase(1);

		glRasterPos2i(50, 50);
		glCallLists(movesMsg.size(), GL_UNSIGNED_BYTE, movesMsg.c_str());
		glRasterPos2i(50, 70);
		glCallLists(timeMsg.size(), GL_UNSIGNED_BYTE, timeMsg.c_str());

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void GLFrame::OnCreate()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_FLAT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(0.82f, 0.85f, 0.96f, 1.0f);

	viewer.SetConstRotationSpeed(0.61f);
	viewer.qRotation = qResetView;
	SetTimer(m_hwnd, 1, 25, NULL);

	LOGFONT lf = { };
	lf.lfHeight = 20;
	lf.lfWeight = FW_BOLD;
	lf.lfCharSet = DEFAULT_CHARSET;
	StringCchCopy(lf.lfFaceName, LF_FACESIZE, "Arial");
	HFONT hFont = CreateFontIndirect(&lf);

	HFONT hPrevFont = (HFONT)SelectObject(m_hdc, hFont);
	wglUseFontBitmaps(m_hdc, 0, 255, 1);
	SelectObject(m_hdc, hPrevFont);
	DeleteObject(hFont);

	cube = new Cube();
	solveTime = time(NULL);
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
	if (fSolvedAnim) return;
	if (button == MouseButton::RBUTTON) {
		viewer.BeginRotate(winX, winY);
	}
	else if (button == MouseButton::LBUTTON)
	{
		BlockDesc block;
		ZeroMemory(&drag, sizeof(drag));
		if (!GetBlockUnderMouse(winX, winY, block)) {
			viewer.BeginRotate(winX, winY);
			sceneDrag = true;
		}
		else if (!cube->IsAnim()) {
			CubeBlock *b = cube->blocks[block.pos.x][block.pos.y][block.pos.z];
			if (b->IsSideColored(block.side)) {
				CalcRotDirections(block);
				mousePos = Point2i(winX, winY);
				faceDrag = true;
			}
		}
	}
}

void GLFrame::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (fSolvedAnim) return;
	if (keysPressed & KeyModifiers::KM_RBUTTON || sceneDrag) {
		viewer.Rotate(x, y);

		if (!cube->IsAnim()) RedrawWindow();
		else needRedraw = true;
	}
	else if (keysPressed & KeyModifiers::KM_LBUTTON && faceDrag) {
		
		Vector3d v(x - mousePos.x, y - mousePos.y, 0);
		if (v.Length() >= 10)
		{
			v.Normalize();
			MouseRot(v);
			faceDrag = false;
		}
	}
}

void GLFrame::OnMouseUp(MouseButton button, int x, int y)
{
	sceneDrag = false;
}

void GLFrame::OnTimer()
{
	if (fSolvedAnim) {
		rotAngle += 3.0f;
		if (rotAngle > 360.0f) rotAngle -= 360.0f;
		RedrawWindow();
		return;
	}

	bool fResetAnim = !resetAnim.IsComplete();
	bool fCubeAnim = cube->AnimationStep();

	if (fResetAnim) viewer.qRotation = resetAnim.Next();

	if (isFaceRotating && !fCubeAnim) {
		OnFaceRotated();
		isFaceRotating = false;
	}
	if (isMixing && !fCubeAnim) {
		OnMixed();
		isMixing = false;
	}

	if (needRedraw || fCubeAnim || fResetAnim) {
		RedrawWindow();
		needRedraw = false;
	}
}

void GLFrame::OnDestroy()
{
	KillTimer(m_hwnd, 1);
	delete cube;
}

void GLFrame::OnFaceRotated()
{
	if (cube->IsSolved()) {
		if (wasMixed || numMoves >= 20)
			OnCubeSolved();
		wasMixed = false;
	}
}

void GLFrame::OnMixed()
{
	solveTime = time(NULL);
}

void GLFrame::OnCubeSolved()
{
	char buf[100] = "";
	StringCchPrintfA(buf, 100, "Moves: %d", numMoves);
	movesMsg = buf;

	solveTime = time(NULL) - solveTime;
	int mins = (int)solveTime / 60;
	int secs = (int)(mins ? solveTime % mins : solveTime);

	StringCchPrintfA(buf, 100, "Time: %d min %d sec", mins, secs);
	timeMsg = buf;

	fSolvedAnim = true;
	rotAngle = 0.0f;
	SendMessage(GetParent(m_hwnd), WM_CUBESOLVED, 0, 0);
}

bool GLFrame::GetBlockUnderMouse(int winX, int winY, BlockDesc &b)
{
	CubeBlock::fRenderPickMode = true;
	RenderScene();
	CubeBlock::fRenderPickMode = false;

	int viewport[4] = { };
	GLubyte clr[3] = { };
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(winX, viewport[3] - winY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr);

	if (clr[2] != 1) return false;
	UINT id = clr[0] | (clr[1] << 8);
	cube->GetBlockById(id, b);
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
	Vector3d ret;
	Vector3f l = cube->blocks[b.x][b.y][b.z]->location;
	l.x -= cube->blockSize * 0.5f;
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
	history.Push(m);
	isFaceRotating = true;
	numMoves++;
	SendMessage(GetParent(m_hwnd), WM_ROTATEFACE, 0, 0);
}