#include "appwindow.h"
#include <string>
#include <sstream>

Color3b AppWindow::colors[6] =
{
	Color3b(0,250,0),
	Color3b(255,90,0),
	Color3b(0,0,255),
	Color3b(255,0,0),
	Color3b(255,255,255),
	Color3b(255,255,0),
};

int AppWindow::colorMap[3][3][3][3] =
{
	{
		{{0,1,5}, {0,5}, {0,3,5}},
		{{0,1}, {0}, {0,3}},
		{{0,1,4}, {0,4}, {0,3,4}},
	}, {
		{{1,5}, {5}, {3,5}},
		{{1}, {}, {3}},
		{{1,4}, {4}, {3,4}},
	}, {
		{{2,1,5}, {2,5}, {2,3,5}},
		{{2,1}, {2}, {2,3}},
		{{2,1,4}, {2,4}, {2,3,4}},
	},
};

int AppWindow::orientation[3][3][3][4] =
{
	{
		{{AXIS_Y, -90, AXIS_Z, 90}, {AXIS_Y, -90, AXIS_Z, 180}, {AXIS_Y, -90, AXIS_Z, 180}},
		{{AXIS_Y, -90, AXIS_Z, 90}, {AXIS_Y, -90}, {AXIS_Y, -90, AXIS_Z, -90}},
		{{AXIS_Y, -90}, {AXIS_Y, -90}, {AXIS_Y, -90, AXIS_Z, -90}},
	}, {
		{{AXIS_X, 180}, {AXIS_X, 90}, {AXIS_Z, 180}},
		{{AXIS_Y, 180}, {}, {}},
		{{AXIS_Y, 180}, {AXIS_X, -90}, {}},
	}, {
		{{AXIS_Y, 90, AXIS_Z, 180},{AXIS_Y, 90, AXIS_Z, 180},{AXIS_Y, 90, AXIS_Z, 90}},
		{{AXIS_Y, 90, AXIS_Z, -90},{AXIS_Y, 90},{AXIS_Y, 90, AXIS_Z, 90}},
		{{AXIS_Y, 90, AXIS_Z, -90},{AXIS_Y, 90},{AXIS_Y, 90}}
	}
};

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
{
	ZeroMemory(blocks, sizeof(blocks));
	ZeroMemory(&curPlane, sizeof(curPlane));
	curPlane.mRot.LoadIdentity();

	this->CreateParam("Cubex", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600);
	InitBlocks();
}

void AppWindow::RotatePlane(Axis plane, int index, bool clockWise)
{
	if (curPlane.anim) return;
	
	curPlane.angle = 0.0;
	curPlane.normal = plane;
	curPlane.index = index;
	curPlane.dir = clockWise ? 1 : -1;
	curPlane.mRot.LoadIdentity();
	curPlane.anim = true;
}

bool AppWindow::CheckIsSolved() const
{
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				const BlockColor &c = blocks[x][y][z]->clr;
				for (int i = 0; i < 3; i++) {
					if (c.sides[0] != colors[colorMap[x][y][z][0]])
						return false;
				}
			}
	return true;
}

void AppWindow::InitBlocks()
{
	const float blockSize = 30.0f, d = 10.0f;
	UINT pickId = 0;
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				CubeBlock *&b = blocks[x][y][z];
				b = new CubeBlock(blockSize);
				b->location = Vector3f((x-1)*(blockSize+d), (y-1)*(blockSize+d), (z-1)*(blockSize+d));
				b->pickId = pickId++;
				
				InitBlockSides(b, x, y, z);

				const int *c = colorMap[x][y][z];
				for (int i = 0; i < 3; i++)
					b->clr.sides[i] = colors[c[i]];

				int *o = orientation[x][y][z];
				for (int i = 0; i <= 2 && o[i+1]; i += 2) {
					Vector3f axis;
					axis.data[o[i]] = 1.0f;
					b->rotation *= Quaternion(axis, (float)o[i+1]);
				}

			}
}

void AppWindow::InitBlockSides(CubeBlock *cb, int x, int y, int z)
{
	if (x != 1) {
		if (y != 1 && z != 1) {
			cb->numSides = 3;
			if (x == 0) {
				if (y + z == 2) cb->swapSides = true;
			} else {
				if (z == y) cb->swapSides = true;
			}
		}
		else if (y == 1 && z == 1) {
			cb->numSides = 1;
		}
		else cb->numSides = 2;
	}
	else {
		if (y != 1 && z != 1) {
			cb->numSides = 2;
		}
		else if (y == 1 && z == 1) {
			cb->numSides = 0;
		}
		else cb->numSides = 1;
	}
}

void AppWindow::TransformColors()
{
	BlockColor tmp[3][3] = { };
	memset(tmp, 0, sizeof(tmp));

	int x, y, z;
	for (int a = 0; a < 3; a++)
		for (int b = 0; b < 3; b++)
		{
			int s = curPlane.normal % 2 ? -1 : 1;
			const int *r = (curPlane.dir*s > 0 ? rot[0] : rot[1])[a][b];

			switch (curPlane.normal) {
				case AXIS_X: x = curPlane.index; y = a; z = b; break;
				case AXIS_Y: x = a; y = curPlane.index; z = b; break;
				case AXIS_Z: x = a; y = b; z = curPlane.index; break;
			}

			tmp[a][b] = blocks[x][y][z]->clr;
			BlockColor &tmpColor = tmp[r[0]][r[1]];

			CubeBlock *cb = blocks[x][y][z];
			if (tmpColor.sides[0] != Color3b())
				cb->clr = tmpColor;
			else
			{
				CubeBlock *block = NULL;
				switch (curPlane.normal) {
					case AXIS_X: block = blocks[x][r[0]][r[1]]; break;
					case AXIS_Y: block = blocks[r[0]][y][r[1]]; break;
					case AXIS_Z: block = blocks[r[0]][r[1]][z]; break;
				}
				cb->clr = block->clr;
			}

			OrientateColors(cb, x, y);
		}
}

void AppWindow::OrientateColors(CubeBlock *cb, int x, int y)
{
	if (curPlane.normal == AXIS_X) {
		if (x == 1 && cb->numSides == 2)
			Swap(cb->clr.sides[0], cb->clr.sides[1]);
		else if (cb->numSides == 3)
			Swap(cb->clr.sides[1], cb->clr.sides[2]);
	}
	else if (curPlane.normal == AXIS_Y) {
		if (cb->numSides == 3 || y == 1 && cb->numSides == 2)
			Swap(cb->clr.sides[0], cb->clr.sides[1]);
	}
	else {
		if (cb->numSides == 3)
			Swap(cb->clr.sides[0], cb->clr.sides[2]);
		else if (cb->numSides == 2)
			Swap(cb->clr.sides[0], cb->clr.sides[1]);
	}
}

void AppWindow::AnimationStep()
{
	static float delta = 0;
	delta += 30.0f;

	curPlane.angle += curPlane.dir*(delta / 20.0f);
	if (curPlane.angle > 90.0f || curPlane.angle < -90)
	{
		delta = 0;
		curPlane.angle = 0.0f;
		curPlane.anim = false;
		curPlane.mRot.LoadIdentity();
		TransformColors();
			
		//RotatePlane((Axis)(rand() % 3), rand()%3, (bool)(rand() % 2));
	}
	else {
		Vector3f axis;
		axis.data[curPlane.normal] = 1.0f;
		Quaternion qRot(axis, curPlane.angle);
		qRot.ToMatrix(curPlane.mRot);
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
		glMultMatrixf(curPlane.mRot.data);
		int x, y, z;
		for (int a = 0; a < 3; a++)
			for (int b = 0; b < 3; b++)
			{
				switch (curPlane.normal) {
					case AXIS_X: x = curPlane.index; y = a; z = b; break;
					case AXIS_Y: x = a; y = curPlane.index; z = b; break;
					case AXIS_Z: x = a; y = b; z = curPlane.index; break;
				}

				CubeBlock *block = blocks[x][y][z];
				block->Render();
			}
	glPopMatrix();

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++)
			{
				if (curPlane.normal == AXIS_X && x == curPlane.index ||
					curPlane.normal == AXIS_Y && y == curPlane.index ||
					curPlane.normal == AXIS_Z && z == curPlane.index)
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
	if (curPlane.anim) {
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

void AppWindow::OnMouseDown(MouseButton button, int x, int y)
{
	if (button == MouseButton::LBUTTON) {
		viewer.BeginRotate(x, y);
	}
	else if (button == MouseButton::RBUTTON)
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
			gluPickMatrix(x, viewport[3] - y, 2, 2, viewport);
			viewer.SetPerspective(60.0f, 1.0f, 400.0f, Point3f(0.0f, 0.0f, -200.0f), viewport[2], viewport[3]);

			glDisable(GL_CULL_FACE);
			RenderScene();
			glEnable(GL_CULL_FACE);

			hitCount = glRenderMode(GL_RENDER);
			if (hitCount > 0)
			{
				UINT minZ = UINT_MAX, id = 0;
				for (int i = 0; i < hitCount; i++) {
					UINT z = selectBuffer[i*4+1];
					if (z <= minZ) {
						id = selectBuffer[i*4+3];
						minZ = z;
					}
				}

				int blockIndex = id & 0x1f;
				int x = blockIndex / 9;
				int y = (blockIndex / 3) % 3;
				int z = blockIndex % 3;
				int side = -1;
				if (id & 0x20) side = 0;
				else if (id & 0x40) side = 1;
				else if (id & 0x80) side = 2;

				//char s[100] = "";
				std::ostringstream s;
				s << '(' << x << ',' << y << ',' << z << ") " << side;

				//itoa(id, s, 10);
				SetWindowTextA(m_hwnd, s.str().c_str());
			}
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
}

void AppWindow::OnMouseMove(UINT keysPressed, int x, int y)
{
	if (keysPressed & KeyModifiers::KM_LBUTTON) {
		viewer.Rotate(x, y);
		RedrawWindow();
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