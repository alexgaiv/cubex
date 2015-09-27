#include "appwindow.h"
#include <string>
#include <sstream>

int AppWindow::rot[2][3][3][2] =
{
	{
		{{0,2}, {1,2}, {2,2}},
		{{0,1}, {1,1}, {2,1}},
		{{0,0}, {1,0}, {2,0}},
	}, {
		{{2,0}, {1,0}, {0,0}},
		{{2,1}, {1,1}, {0,1}},
		{{2,2}, {1,2}, {0,2}}
	}
};

AppWindow::AppWindow()
	: mouseFlag(false), rotateScene(false),
	blockSize(30.0f), blockSpace(2.0f), rotateSpeed(12.0f)
{
	ZeroMemory(blocks, sizeof(blocks));
	ZeroMemory(&curFace, sizeof(curFace));
	ZeroMemory(&drag, sizeof(drag));
	curFace.mRot.LoadIdentity();

	this->CreateParam("Cubex", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600);
	InitBlocks();
}

void AppWindow::RotateFace(Axis faceNormal, int index, bool clockWise)
{
	if (curFace.anim) return;
	
	curFace.angle = 0.0;
	curFace.normal = faceNormal;
	curFace.index = index;
	curFace.dir = clockWise ? 1 : -1;
	curFace.mRot.LoadIdentity();
	curFace.anim = true;
}

bool AppWindow::CheckIsSolved() const
{
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				const CubeBlock *b = blocks[x][y][z];
				for (int i = 0; i < b->numSides; i++) {
					if (b->clr.colorIndices[i] != b->coloredSides[i])
						return false;
				}
			}
	return true;
}

void AppWindow::InitBlocks()
{
	const float d = blockSize + blockSpace;
	UINT pickId = 0;
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				CubeBlock *&b = blocks[x][y][z];
				b = new CubeBlock(blockSize);
				b->location = Vector3f((x-1)*d, (y-1)*d, (z-1)*d);
				b->pickId = pickId++;
				
				InitBlockSides(b, x, y, z);
			}
}

void AppWindow::InitBlockSides(CubeBlock *cb, int x, int y, int z)
{
	int i = 0;
	if (x != 1)
		cb->coloredSides[i++] = x;

	if (y == 0)
		cb->coloredSides[i++] = 5;
	else if (y == 2)
		cb->coloredSides[i++] = 4;

	if (z == 0)
		cb->coloredSides[i++] = 1;
	else if (z == 2)
		cb->coloredSides[i++] = 3;

	cb->numSides = i;
	for (i = 0; i < cb->numSides; i++) {
		cb->clr.colorIndices[i] = cb->coloredSides[i];
	}
}

void AppWindow::TransformColors()
{
	BlockColor tmp[3][3] = { };
	memset(tmp, -1, sizeof(tmp));

	int x, y, z;
	for (int a = 0; a < 3; a++)
		for (int b = 0; b < 3; b++)
		{
			int s = curFace.normal % 2 ? -1 : 1;
			const int *r = (curFace.dir*s > 0 ? rot[0] : rot[1])[a][b];

			switch (curFace.normal) {
				case AXIS_X: x = curFace.index; y = a; z = b; break;
				case AXIS_Y: x = a; y = curFace.index; z = b; break;
				case AXIS_Z: x = a; y = b; z = curFace.index; break;
			}
			CubeBlock *cb = blocks[x][y][z];

			tmp[a][b] = cb->clr;
			BlockColor &tmpColor = tmp[r[0]][r[1]];
			
			if (tmpColor.colorIndices[0] != -1)
				cb->clr = tmpColor;
			else
			{
				CubeBlock *block = NULL;
				switch (curFace.normal) {
					case AXIS_X: block = blocks[x][r[0]][r[1]]; break;
					case AXIS_Y: block = blocks[r[0]][y][r[1]]; break;
					case AXIS_Z: block = blocks[r[0]][r[1]][z]; break;
				}
				cb->clr = block->clr;
			}

			OrientateColors(cb, x, y, z);
		}
}

void AppWindow::OrientateColors(CubeBlock *cb, int x, int y, int z)
{
	BlockColor &c = cb->clr;
	if (curFace.normal == AXIS_X) {
		if (x == 1 && cb->numSides == 2)
			Swap(c.colorIndices[0], c.colorIndices[1]);
		else if (cb->numSides == 3)
			Swap(c.colorIndices[1], c.colorIndices[2]);
	}
	else if (curFace.normal == AXIS_Y) {
		if (cb->numSides == 3)
			Swap(c.colorIndices[0], c.colorIndices[2]);
		else if (cb->numSides == 2)
			Swap(c.colorIndices[0], c.colorIndices[1]);
	}
	else {
		if (cb->numSides == 3 || z == 1 && cb->numSides == 2)
			Swap(c.colorIndices[0], c.colorIndices[1]);
	}
}

void AppWindow::AnimationStep()
{
	curFace.angle += curFace.dir*rotateSpeed;
	if (curFace.angle > 90.0f || curFace.angle < -90)
	{
		curFace.angle = 0.0f;
		curFace.anim = false;
		curFace.mRot.LoadIdentity();
		TransformColors();
			
		//RotateFace((Axis)(rand() % 3), rand()%3, (bool)(rand() % 2));
	}
	else {
		Vector3f axis;
		axis.data[curFace.normal] = 1.0f;
		Quaternion qRot(axis, curFace.angle);
		qRot.ToMatrix(curFace.mRot);
	}
}

void AppWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	float lightPos[4] = { 0.0f, 10.0f, 10.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	viewer.ApplyTransform();

	glPushMatrix();
		glMultMatrixf(curFace.mRot.data);
		int x, y, z;
		for (int a = 0; a < 3; a++)
			for (int b = 0; b < 3; b++)
			{
				switch (curFace.normal) {
					case AXIS_X: x = curFace.index; y = a; z = b; break;
					case AXIS_Y: x = a; y = curFace.index; z = b; break;
					case AXIS_Z: x = a; y = b; z = curFace.index; break;
				}

				CubeBlock *block = blocks[x][y][z];
				block->Render();
			}
	glPopMatrix();

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				if (curFace.normal == AXIS_X && x == curFace.index ||
					curFace.normal == AXIS_Y && y == curFace.index ||
					curFace.normal == AXIS_Z && z == curFace.index)
						continue;
					CubeBlock *block = blocks[x][y][z];
					block->Render();
			}

	glPopMatrix();
}

void AppWindow::OnCreate()
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
}

void AppWindow::OnDisplay()
{
	RenderScene();
}

void AppWindow::OnTimer()
{
	if (curFace.anim) {
		AnimationStep();
		RedrawWindow();
	}
}

void AppWindow::OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	viewer.SetPerspective(60.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -200.0f), w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool AppWindow::GetBlockUnderMouse(int winX, int winY, int &x, int &y, int &z, int &side)
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
	viewer.SetPerspective(60.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -200.0f), viewport[2], viewport[3]);

	glDisable(GL_CULL_FACE);
	RenderScene();
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	hitCount = glRenderMode(GL_RENDER);
	if (hitCount <= 0) return false;

	UINT minZ = UINT_MAX, id = 0;
	for (int i = 0; i < hitCount; i++) {
		UINT z = selectBuffer[i*4+1];
		if (z <= minZ) {
			id = selectBuffer[i*4+3];
			minZ = z;
		}
	}

	int blockIndex = id & 0x1f;
	x = blockIndex / 9;
	y = (blockIndex / 3) % 3;
	z = blockIndex % 3;

	side = -1;
	for (int i = 0 ; i < 6; i++)
		if (id & (1 << (i+5))) {
			side = i;
			break;
		}
	return true;
}

void AppWindow::InitDrag(int x, int y, int z, int side, Point3<int> &a, Point3<int> &b)
{
	if (side == 0 || side == 2) {
		a.x = b.x = x;

		a.y = y == 2 ? 1 : y + 1;
		a.z = z;
		b.z = z == 2 ? 1 : z + 1;
		b.y = y;

		drag.face1 = AXIS_Z;
		drag.index1 = z;
		drag.face2 = AXIS_Y;
		drag.index2 = y;
	}
	else if (side == 1 || side == 3) {
		a.z = b.z = z;

		a.y = y == 2 ? 1: y + 1;
		a.x = x;
		b.x = x == 2 ? 1 : x + 1;
		b.y = y;

		drag.face1 = AXIS_X;
		drag.index1 = x;
		drag.face2 = AXIS_Y;
		drag.index2 = y;
	}
	else if (side == 4 || side == 5) {
		a.y = b.y = y;

		a.z = z == 2 ? 1: z + 1;
		a.x = x;
		b.x = x == 2 ? 1 : x + 1;
		b.z = z;

		drag.face1 = AXIS_X;
		drag.index1 = x;
		drag.face2 = AXIS_Z;
		drag.index2 = z;
	}
}

void AppWindow::OnMouseDown(MouseButton button, int winX, int winY)
{
	if (button == MouseButton::RBUTTON) {
		viewer.BeginRotate(winX, winY);
	}
	else if (button == MouseButton::LBUTTON)
	{
		int x, y, z, side;
		if (!GetBlockUnderMouse(winX, winY, x, y, z, side)) {
			viewer.BeginRotate(winX, winY);
			rotateScene = true;
		}
		else {
			Point3<int> a,b;
			Vector3d target;
			Vector3f l;

			InitDrag(x, y, z, side, a, b);

			Matrix44d modelview(viewer.Modelview());
			double proj[16];
			int viewport[4];
			glGetDoublev(GL_PROJECTION_MATRIX, proj);
			glGetIntegerv(GL_VIEWPORT, viewport);

			static const float d = (blockSize+blockSpace) * 0.5f;
			l = blocks[x][y][z]->location;
			l.x -= d;
			gluProject(l.x, l.y, l.z, modelview.data, proj, viewport, &target.x, &target.y, &target.z);

			l = blocks[a.x][a.y][a.z]->location;
			l.x -= d;
			gluProject(l.x, l.y, l.z, modelview.data, proj, viewport, &drag.dir1.x, &drag.dir1.y, &drag.dir1.z);

			l = blocks[b.x][b.y][b.z]->location;
			l.x -= d;
			gluProject(l.x, l.y, l.z, modelview.data, proj, viewport, &drag.dir2.x, &drag.dir2.y, &drag.dir2.z);

			target.y = viewport[3] - target.y;
			drag.dir1.y = viewport[3] - drag.dir1.y;
			drag.dir2.y = viewport[3] - drag.dir2.y;
			drag.dir1 -= target;
			drag.dir2 -= target;
			drag.dir1.Normalize();
			drag.dir2.Normalize();

			if (side == 0 || side == 2) {
				if (y == 2) drag.dir1 = -drag.dir1;
				if (z == 2) drag.dir2 = -drag.dir2;
			}
			else if (side == 1 || side == 3) {
				if (y == 2) drag.dir1 = -drag.dir1;
				if (x == 2) drag.dir2 = -drag.dir2;
			}
			else if (side == 4 || side == 5) {
				if (z == 2) drag.dir1 = -drag.dir1;
				if (x == 2) drag.dir2 = -drag.dir2;
			}

			if (side == 2 || side == 1 || side == 4) {
				drag.dir1 = -drag.dir1;
				drag.dir2 = -drag.dir2;
			}

			mousePos = Point2i(winX, winY);
			mouseFlag = true;

			char s[100] = "";
			sprintf_s(s, "(%d, %d)", winX, winY);
			SetWindowTextA(m_hwnd, s);
		}
	}
}

void AppWindow::OnMouseUp(MouseButton button, int x, int y)
{
	rotateScene = false;
}

void AppWindow::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (keysPressed & KeyModifiers::KM_RBUTTON || rotateScene) {
		viewer.Rotate(x, y);
		RedrawWindow();
	}
	else if (keysPressed & KeyModifiers::KM_LBUTTON && mouseFlag) {
		
		Vector3d v(x - mousePos.x, y - mousePos.y, 0);
		if (v.Length() >= 10)
		{
			v.Normalize();
			double a1 = acos(v.Dot(v, drag.dir1));
			double a2 = acos(v.Dot(v, drag.dir2));

			double c[4] = {
				a1, M_PI - a1,
				a2, M_PI - a2,
			};

			int k = 0;
			for (int i = 0; i < 4; i++) {
				if (c[i] < c[k]) k = i;
			}

			if (k <= 1)
				RotateFace(drag.face1, drag.index1, k == 1);
			else RotateFace(drag.face2, drag.index2, k == 2);

			mouseFlag = false;
		}
	}
}

void AppWindow::OnDestroy()
{
	KillTimer(m_hwnd, 1);
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				delete blocks[x][y][z];
			}
}