#include "glframe.h"
#include "transform.h"
#include <string>
#include <strsafe.h>

Quaternion GLFrame::qResetView =
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 30.0f) *
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), -45.0f);

GLFrame::GLFrame() : program(NULL)
{
	solveTime = 0;
	wasMixed = false;
	numMoves = numActualMoves = 0;
	fSolvedAnim = false;
	rotAngle = 0.0f;
	faceDrag = sceneDrag = false;
	isFaceRotating = isMixing = false;
	needRedraw = false;
	ZeroMemory(&drag, sizeof(drag));

	viewer.SetConstRotationSpeed(0.61f);
	viewer.qRotation = qResetView;

	cube = new Cube(3);
}

GLFrame::~GLFrame()
{
	delete cube;
}

bool GLFrame::ChangeCubeSize(int size)
{
	if (cube->size == size) return false;
		
	solveTime = time(NULL);
	wasMixed = false;
	numMoves = numActualMoves = 0;
	fSolvedAnim = false;
	rotAngle = 0.0f;
	faceDrag = sceneDrag = false;
	isFaceRotating = isMixing = false;
	needRedraw = false;
	ZeroMemory(&drag, sizeof(drag));

	history.Clear();
	resetAnim.Setup(viewer.qRotation, qResetView, 0.1f);

	CubeBlock::fUseReducedModel = size >= 5;

	if (size >= 2 && size <= 7) {
		float scale[6] = { 1.1f, 1.0f, 0.86f, 0.66f, 0.56f, 0.48f };
		viewer.SetScale(scale[size - 2]);
	}

	delete cube;
	cube = new Cube(size);
	RedrawWindow();
	return true;
}

void GLFrame::ResetCube()
{
	if (!resetAnim.IsComplete()) return;

	if (fSolvedAnim)
		viewer.qRotation *= Quaternion(Vector3f(1.0f, 1.0f, 1.0f), rotAngle);

	isMixing = false;
	fSolvedAnim = false;
	wasMixed = false;
	numMoves = numActualMoves = 0;
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
	numMoves = numActualMoves = 0;

	cube->MixUp(cube->size < 7 ? 15 : 20);
	history.Clear();
}

void GLFrame::SetCubeStyle(bool whiteBorders)
{
	CubeBlock::DrawWhiteBorders(whiteBorders);
	RedrawWindow();
}

void GLFrame::CancelMove() {
	if (!cube->IsAnim()) {
		MoveDesc *m = history.Pop();
		if (m) {
			cube->BeginRotateFace(m->normal, m->index, !m->clockWise);
			numMoves--;
			if (numActualMoves > 0) numActualMoves--;
		}
	}
}

void GLFrame::SetPerspective(int w, int h) {
	viewer.SetPerspective(40.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -270.0f), w, h);
}

void GLFrame::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Global::PushModelView();

	viewer.ApplyTransform();
	if (fSolvedAnim) {
		Global::MultModelView(Rotate(rotAngle, 1, 1, 1));
	}

	Global::PushModelView();
		Global::MultModelView(Scale(1.8f,1.8f,1.8f));
		cube->Render();
	Global::PopModelView();

	if (fSolvedAnim && !CubeBlock::fRenderPickMode)
	{
		RECT winRect = { };
		GetClientRect(m_hwnd, &winRect);

		Global::PushProjection();
		Global::SetProjection(Ortho(0.0f, (float)winRect.right, (float)winRect.bottom, 0.0f, -1.0f, 1.0f));

		Global::PushModelView();
		Global::SetModelView(Matrix44f::Identity());

		glColor3f(0.07f, 0.31f, 0.76f);
		glListBase(1);

		glRasterPos2i(50, 50);
		glCallLists(movesMsg.size(), GL_UNSIGNED_BYTE, movesMsg.c_str());
		glRasterPos2i(50, 70);
		glCallLists(timeMsg.size(), GL_UNSIGNED_BYTE, timeMsg.c_str());

		Global::PopModelView();
		Global::PopProjection();
	}

	Global::PopModelView();
}

void GLFrame::OnCreate()
{
	glewInit();
	//__GLEW_ARB_shader_objects = NULL;

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.82f, 0.85f, 0.96f, 1.0f);

	if (GLEW_ARB_shader_objects) {
		program = new ProgramObject("shaders/cube.vert.glsl", "shaders/cube.frag.glsl");
		program->Uniform("ColorMap", 0);
		program->Uniform("NormalMap", 1);
		program->Uniform("SpecularMap", 2);
		program->Uniform("DecalMap", 3);
		program->Uniform("FrontMaterial.specular", 1, Color3f(0.4f).data);
		program->Use();
	}

	CubeBlock::InitStatic();

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

	SetTimer(m_hwnd, 1, 25, NULL);
	solveTime = time(NULL);
}

void GLFrame::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	SetPerspective(w, h);
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
	CubeBlock::FreeStatic();
	if (GLEW_ARB_shader_objects)
		delete program;
}

void GLFrame::OnFaceRotated()
{
	if (cube->IsSolved()) {
		if (wasMixed || numActualMoves >= cube->GOD_NUMBER)
			OnCubeSolved();
		wasMixed = false;
		numActualMoves = 0;
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
	glDisable(GL_MULTISAMPLE);
	RenderScene();
	CubeBlock::fRenderPickMode = false;
	glEnable(GL_MULTISAMPLE);

	int viewport[4] = { };
	GLubyte clr[3] = { };
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(winX, viewport[3] - winY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr);

	if (clr[2] != 1) return false;
	UINT id = clr[0] | (clr[1] << 8);
	cube->GetBlockById(id, b);
	return true;
}

void GLFrame::GetNeighbors(const BlockDesc &b, Point3i &neighbor1, Point3i &neighbor2)
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

	n1[i1] = p[i1] == cube->size - 1 ? 0 : p[i1] + 1;
	n1[i2] = p[i2];
	n2[i2] = p[i2] == cube->size - 1 ? 0 : p[i2] + 1;
	n2[i1] = p[i1];

	drag.face[0] = (Axis)i1;
	drag.index[0] = p[i1];
	drag.face[1] = (Axis)i2;
	drag.index[1] = p[i2];

	if (p[i1] == cube->size - 1) drag.neg[0] = true;
	if (p[i2] == cube->size - 1) drag.neg[1] = true;
}

Vector3d GLFrame::CalcBlockWinPos(const Point3i &b,
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
	Point3i n1, n2;
	Vector3d target;

	GetNeighbors(b, n1, n2);

	Matrix44d modelview(viewer.GetViewMatrix());
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
	double a1 = acos(Dot(mouseDir, drag.dir[0]));
	double a2 = acos(Dot(mouseDir, drag.dir[1]));

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
	numActualMoves++;
	SendMessage(GetParent(m_hwnd), WM_ROTATEFACE, 0, 0);
}