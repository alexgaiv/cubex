#include "glframe.h"
#include "resources.h"
#include "transform.h"
#include "text2d.h"
#include <strsafe.h>
#include <string>
#include <ShlObj.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

GLFrame::GLFrame()
{
	ctx = NULL;
	camera = NULL;
	program = NULL;
	scoreMsg = NULL;
	showTimer = false;
	whiteBorders = false;

	ZeroMemory(cubes, sizeof(cubes));
	ZeroMemory(&drag, sizeof(drag));

	faceDrag = sceneDrag = false;
	needRedraw = false;
}

bool GLFrame::ChangeCubeSize(int size)
{
	if (size < 2 || size > 7 || ctx->cube->size == size)
		return false;
	
	ctx->timer.Suspend();
	ctx->qRotation = camera->qRotation;
	ctx = cubes[size - 2];
	ctx->timer.Resume();
	camera->qRotation = ctx->qRotation;

	if (showTimer) ConstructText();

	faceDrag = sceneDrag = false;
	needRedraw = false;
	ZeroMemory(&drag, sizeof(drag));

	CubeBlock::fUseReducedModel = size >= 5;

	if (size >= 2 && size <= 7) {
		float scale[6] = { 1.1f, 1.0f, 0.86f, 0.66f, 0.56f, 0.48f };
		camera->SetScale(scale[size - 2]);
	}

	Redraw();
	return true;
}

void GLFrame::ResetCube()
{
	if (!resetAnim.IsComplete()) return;
	if (ctx->IsSolved())
		camera->qRotation *= Quaternion(Vector3f(1.0f, 1.0f, 1.0f), ctx->rotAngle);
	ctx->Reset();
	resetAnim.Setup(camera->qRotation, CubeContext::qResetView, 0.1f);
	if (showTimer) ConstructText();
}

void GLFrame::ScrambleCube()
{
	if (ctx->IsSolved() && resetAnim.IsComplete())
		camera->qRotation *= Quaternion(Vector3f(1.0f, 1.0f, 1.0f), ctx->rotAngle);
	ctx->Scramble();
	if (showTimer) ConstructText();
}

void GLFrame::SetCubeStyle(bool whiteBorders)
{
	program->Use();
	this->whiteBorders = whiteBorders;
	CubeBlock::DrawWhiteBorders(m_rc, whiteBorders);
	Redraw();
}

void GLFrame::SetPerspective(int w, int h) {
	camera->SetPerspective(30.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -270.0f), w, h);
}

void GLFrame::CancelMove() {
	ctx->CancelMove();
}

void GLFrame::SaveConfig()
{
	ctx->timer.Suspend();

	WCHAR path[MAX_PATH] = L"";
	SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
	PathAppendW(path, L"\\Cubex");
	CreateDirectoryW(path, NULL);
	PathAppendW(path, L"\\1.2.1");
	CreateDirectoryW(path, NULL);
	PathAppendW(path, L"\\save.dat");

	ofstream file(path);
	if (!file) return;

	file << whiteBorders << ' ' << showTimer << ' ' << ctx->cube->size << ' ';
	ctx->qRotation = camera->qRotation;
	for (int i = 0; i < 6; i++)
		cubes[i]->Serialize(file);
	file.close();
}

void GLFrame::LoadConfig()
{
	WCHAR path[MAX_PATH] = L"";
	SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path);
	PathAppendW(path, L"\\Cubex\\1.2.1\\save.dat");

	ifstream file(path);
	if (!file) {
		ctx = cubes[1];
		return;
	}

	file >> whiteBorders >> showTimer;
	if (whiteBorders) CubeBlock::DrawWhiteBorders(m_rc, true);

	int size = 0;
	file >> size;
	if (size >= 2 && size <= 7) {
		ctx = cubes[size - 2];
		float scale[6] = { 1.1f, 1.0f, 0.86f, 0.66f, 0.56f, 0.48f };
		camera->SetScale(scale[size - 2]);
	}
	else ctx = cubes[1];

	for (int i = 0; i < 6; i++)
		cubes[i]->Deserialize(file);
	file.close();
}

void GLFrame::ConstructText()
{
	WCHAR score[50] = L"";
	ctx->GetScoreStr(score, 50);
	scoreMsg->SetText(score);
}

void GLFrame::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	m_rc->PushModelView();

	camera->ApplyTransform();
	if (ctx->IsSolved()) {
		m_rc->MultModelView(Rotate(ctx->rotAngle, 1, 1, 1));
	}

	if (program) program->Use();
	m_rc->PushModelView();
		m_rc->MultModelView(Scale(1.37f,1.37f,1.37f));
		ctx->cube->Render();
	m_rc->PopModelView();

	if (!CubeBlock::fRenderPickMode)
	{
		if (showTimer || ctx->IsSolved()) {
			if (ctx->timer.IsStarted()) ConstructText();

			RECT r = { };
			GetClientRect(m_hwnd, &r);
			scoreMsg->Draw(r.right - 180, r.bottom - 80);
		}
	}

	m_rc->PopModelView();
}

void GLFrame::OnCreate()
{
	glewInit();

	if (GL_ARB_vertex_array_object && glGenVertexArrays && glBindVertexArray) {
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.82f, 0.85f, 0.96f, 1.0f);
	//glClearColor(1,1,1,1);

	camera = new TrackballCamera(m_rc);
	camera->SetConstRotationSpeed(2.0f);

	for (int i = 0; i < 6; i++)
		cubes[i] = new CubeContext(m_rc, i + 2);

	if (GLEW_ARB_shader_objects)
	{
		char *source = 0;
		int length = 0;

		GetTextResource("cube.vert.glsl", source, length);
		Shader vertShader(GL_VERTEX_SHADER);
		vertShader.CompileSource(source, length);

		GetTextResource("cube.frag.glsl", source, length);
		Shader fragShader(GL_FRAGMENT_SHADER);
		fragShader.CompileSource(source, length);

		program = new ProgramObject(m_rc);
		program->AttachShader(vertShader);
		program->AttachShader(fragShader);
		program->Link();

		if (program->IsLinked()) {
			program->Uniform("ColorMap", 0);
			program->Uniform("NormalMap", 1);
			program->Uniform("SpecularMap", 2);
			program->Uniform("FrontMaterial.specular", 1, Color3f(0.4f).data);
			program->Use();
		}
		else {
			delete program;
			program = NULL;
			__GLEW_ARB_shader_objects = NULL;
		}
	}

	CubeBlock::InitStatic(m_rc);
	LoadConfig();
	camera->qRotation = ctx->qRotation;

	Font2D font("font.fnt");
	font.SetColor(Color4f(0.07f, 0.31f, 0.76f));
	scoreMsg = new Text2D(m_rc, font);
	ConstructText();

	SetTimer(m_hwnd, 1, 25, NULL);
	ctx->timer.Resume();
}

void GLFrame::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	SetPerspective(w, h);
}

void GLFrame::OnMouseDown(MouseButton button, int x, int y)
{
	if (ctx->IsSolved()) return;
	if (button == MouseButton::RBUTTON) {
		camera->BeginRotate(x, y);
	}
	else if (button == MouseButton::LBUTTON)
	{
		BlockDesc block;
		ZeroMemory(&drag, sizeof(drag));
		if (!GetBlockUnderMouse(x, y, block)) {
			camera->BeginRotate(x, y);
			sceneDrag = true;
		}
		else if (!ctx->cube->IsAnim())
		{
			int s = ctx->cube->size;
			if (block.pos.x < s && block.pos.y < s && block.pos.z < s)
			{
				CubeBlock *b = ctx->cube->blocks[block.pos.x][block.pos.y][block.pos.z];
				if (b->IsSideColored(block.side)) {
					CalcRotDirections(block);
					mousePos = Point2i(x, y);
					faceDrag = true;
				}
			}
		}
	}
}

void GLFrame::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (ctx->IsSolved()) return;
	if (keysPressed & KeyModifiers::KM_RBUTTON || sceneDrag)
	{
		if (!resetAnim.IsComplete()) resetAnim = QSlerp();
		camera->Rotate(x, y);
		if (!ctx->cube->IsAnim()) Redraw();
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
	bool solved = false;

	if (!resetAnim.IsComplete()) {
		camera->qRotation = resetAnim.Next();
		needRedraw = true;
	}

	bool redraw = false;
	ctx->OnTimer(redraw, solved);
	needRedraw |= redraw;

	if (showTimer && !ctx->IsSolved() && !needRedraw) {
		static time_t t = 0;
		time_t cur = time(NULL);
		if (cur - t >= 1) {
			needRedraw = true;
			t = cur;
		}
	}

	if (solved) OnCubeSolved();

	if (needRedraw) {
		Redraw();
		needRedraw = false;
	}
}

void GLFrame::OnDestroy()
{
	SaveConfig();
	KillTimer(m_hwnd, 1);
	CubeBlock::FreeStatic();

	for (int i = 0; i < 6; i++)
		delete cubes[i];
	delete camera;
	delete program;
	delete scoreMsg;
}

void GLFrame::OnCubeSolved()
{
	ConstructText();
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
	ctx->cube->GetBlockById(id, b);
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

	n1[i1] = p[i1] == ctx->cube->size - 1 ? 0 : p[i1] + 1;
	n1[i2] = p[i2];
	n2[i2] = p[i2] == ctx->cube->size - 1 ? 0 : p[i2] + 1;
	n2[i1] = p[i1];

	drag.face[0] = (Axis)i1;
	drag.index[0] = p[i1];
	drag.face[1] = (Axis)i2;
	drag.index[1] = p[i2];

	if (p[i1] == ctx->cube->size - 1) drag.neg[0] = true;
	if (p[i2] == ctx->cube->size - 1) drag.neg[1] = true;
}

Vector3f GLFrame::CalcBlockWinPos(const Point3i &b,
	const Matrix44f &modelview, const Matrix44f &projection, int viewport[4])
{
	Vector3f l = ctx->cube->blocks[b.x][b.y][b.z]->location;
	l.x -= ctx->cube->blockSize * 0.5f;
	return Project(l, modelview, projection, viewport);
};

void GLFrame::CalcRotDirections(const BlockDesc &b)
{
	Point3i n1, n2;
	Vector3f target;

	GetNeighbors(b, n1, n2);

	Matrix44f modelview = camera->GetViewMatrix();
	Matrix44f projection = m_rc->GetProjection();
	int viewport[4];
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

void GLFrame::MouseRot(const Vector3f &mouseDir)
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

	ctx->BeginRotateFace(m);
	SendMessage(GetParent(m_hwnd), WM_ROTATEFACE, 0, 0);
}