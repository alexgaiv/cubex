#include "cube.h"
#include <time.h>

#define FORALLBLOCKS(x, y, z) \
	for (int x = 0; x < 3; x++) \
	for (int y = 0; y < 3; y++) \
	for (int z = 0; z < 3; z++) 

int Cube::rot[2][3][3][2] =
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

Cube::Cube()
	: reseting(false),
	blockSize(30.0f), blockSpace(0.0f), rotateSpeed(12.0f)
{
	srand((UINT)time(NULL));
	ZeroMemory(blocks, sizeof(blocks));
	ZeroMemory(&shuffle, sizeof(shuffle));
	ZeroMemory(&curFace, sizeof(curFace));
	curFace.mRot.LoadIdentity();

	const float d = blockSize + blockSpace;
	UINT pickId = 0;
	FORALLBLOCKS(x, y, z) {
		CubeBlock *&b = blocks[x][y][z];
		b = new CubeBlock(blockSize, pickId++);
		b->location = Vector3f((x-1)*d, (y-1)*d, (z-1)*d);
				
		InitBlockSides(b, x, y, z);
	}
}

Cube::~Cube()
{
	FORALLBLOCKS(x, y, z)
		delete blocks[x][y][z];
}

void Cube::GetBlockById(int id, BlockDesc &b)
{
	int blockIndex = id & 0x1f;
	b.pos = Point3<int>(
		blockIndex / 9,
		(blockIndex / 3) % 3,
		blockIndex % 3
	);

	b.side = -1;
	for (int i = 0; i < 6; i++)
		if (id & (1 << (i+5))) {
			b.side = i;
			break;
		}
}

void Cube::Reset()
{
	if (curFace.anim) reseting = true;
	else DoReset();
}

void Cube::DoReset()
{
	ZeroMemory(&shuffle, sizeof(shuffle));
	FORALLBLOCKS(x, y, z) {
		CubeBlock *b = blocks[x][y][z];
		for (int i = 0; i < b->numSides; i++) {
			b->clr.colorIndices[i] = b->coloredSides[i];
		}
	}
	reseting = false;
}

void Cube::Shuffle(int count)
{
	if (!curFace.anim) {
		shuffle.curCount = shuffle.maxCount = count;
		ShuffleStep();
	}
}

bool Cube::IsSolved() const
{
	int sideColors[6];
	memset(sideColors, -1, sizeof(sideColors));

	FORALLBLOCKS(x, y, z)
	{
		CubeBlock *b = blocks[x][y][z];
		for (int i = 0; i < b->numSides; i++)
		{
			int s = b->coloredSides[i];
			int c = b->clr.colorIndices[i];

			if (sideColors[s] == -1) {
				sideColors[s] = c;
			}
			else if (sideColors[s] != c)
				return false;
		}
	}
	return true;
}

void Cube::BeginRotateFace(Axis faceNormal, int index, bool clockWise)
{
	if (curFace.anim) return;
	
	curFace.angle = 0.0;
	curFace.normal = faceNormal;
	curFace.index = index;
	curFace.dir = clockWise ? 1 : -1;
	curFace.mRot.LoadIdentity();
	curFace.anim = true;
}

bool Cube::AnimationStep()
{
	if (!curFace.anim) return false;

	curFace.angle += curFace.dir*rotateSpeed;
	if (curFace.angle > 90.0f || curFace.angle < -90)
	{
		curFace.angle = 0.0f;
		curFace.anim = false;
		curFace.mRot.LoadIdentity();

		if (!reseting) {
			TransformColors();
			if (shuffle.curCount) ShuffleStep();
		}
		else DoReset();
	}
	else {
		Vector3f axis;
		axis.data[curFace.normal] = 1.0f;
		Quaternion qRot(axis, curFace.angle);
		qRot.ToMatrix(curFace.mRot);
	}
	return true;
}

void Cube::Render() const
{
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
				blocks[x][y][z]->Render();
			}
	glPopMatrix();

	FORALLBLOCKS(x, y, z) {
		if (curFace.normal == AXIS_X && x == curFace.index ||
			curFace.normal == AXIS_Y && y == curFace.index ||
			curFace.normal == AXIS_Z && z == curFace.index)
				continue;
			blocks[x][y][z]->Render();
	}
}

void Cube::ShuffleStep()
{
	Axis normal;
	int index;
	bool dir = (rand() % 2) ? true : false;

	do {
		normal = (Axis)(rand() % 3);
		index = rand() % 3;
	} while (shuffle.curCount != shuffle.maxCount &&
		normal == shuffle.prevNormal && index == shuffle.prevIndex);

	BeginRotateFace(normal, index, dir);		

	shuffle.prevNormal = normal;
	shuffle.prevIndex = index;
	shuffle.curCount--;
}

void Cube::InitBlockSides(CubeBlock *cb, int x, int y, int z)
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

void Cube::TransformColors()
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

void Cube::OrientateColors(CubeBlock *cb, int x, int y, int z)
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