#include "cube.h"
#include "transform.h"
#include <time.h>

#define FORALLBLOCKS(x, y, z) \
	for (int x = 0; x < size; x++) \
	for (int y = 0; y < size; y++) \
	for (int z = 0; z < size; z++) \

#define VISIBLEBLOCKS() \
	if (x > 0 && x < size - 1 && \
		y > 0 && y < size - 1 && \
		z > 0 && z < size - 1) continue

Cube::Cube(int size)
	: size(size), reseting(false),
	blockSize(CubeBlock::size), rotateSpeed(13.0f)
{
	srand((UINT)time(NULL));
	blocks = NULL;
	ZeroMemory(&mixup, sizeof(mixup));
	ZeroMemory(&curFace, sizeof(curFace));
	curFace.mRot.LoadIdentity();

	tmp = new BlockColor*[size];
	for (int i = 0; i < size; i++) {
		tmp[i] = new BlockColor[size];
	}

	UINT pickId = 0;
	float s = (size - 1) * blockSize / 2;

	blocks = new CubeBlock***[size];
	for (int x = 0; x < size; x++) {
		blocks[x] = new CubeBlock**[size];
		for (int y = 0; y < size; y++) {
			blocks[x][y] = new CubeBlock*[size];
			for (int z = 0; z < size; z++)
			{
				CubeBlock *&b = blocks[x][y][z];
				b = new CubeBlock(pickId++);
				b->location = Vector3f(x*blockSize - s, y*blockSize - s, z*blockSize - s);
				InitBlockSides(b, x, y, z);
			}
		}
	}
}

Cube::~Cube()
{	
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			for (int z = 0; z < size; z++)
				delete blocks[x][y][z];
			delete [] blocks[x][y];
		}
		delete [] blocks[x];
	}
	delete blocks;

	for (int i = 0; i < size; i++)
		delete [] tmp[i];
	delete [] tmp;
}

void Cube::GetBlockById(int id, BlockDesc &b) const
{
	int blockIndex = id & 0x3ff;
	b.pos = Point3i(
		blockIndex / (size*size),
		(blockIndex / size) % size,
		blockIndex % size
	);

	b.side = -1;
	for (int i = 0; i < 6; i++)
		if (id & (1 << (i+10))) {
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
	ZeroMemory(&mixup, sizeof(mixup));
	FORALLBLOCKS(x, y, z) {
		VISIBLEBLOCKS();
		CubeBlock *b = blocks[x][y][z];
		for (int i = 0; i < b->numSides; i++) {
			b->clr.colorIndices[i] = b->coloredSides[i];
		}
	}
	reseting = false;
}

void Cube::MixUp(int steps)
{
	if (!curFace.anim) {
		mixup.curSteps = mixup.maxSteps = steps;
		MixupStep();
	}
}

bool Cube::IsSolved() const
{
	int sideColors[6];
	memset(sideColors, -1, sizeof(sideColors));

	FORALLBLOCKS(x, y, z)
	{
		VISIBLEBLOCKS();
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
			if (mixup.curSteps) MixupStep();
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
	Global::PushModelView();
		Global::MultModelView(curFace.mRot.data);
		int x, y, z;
		for (int a = 0; a < size; a++)
			for (int b = 0; b < size; b++)
			{
				switch (curFace.normal) {
					case AXIS_X: x = curFace.index; y = a; z = b; break;
					case AXIS_Y: x = a; y = curFace.index; z = b; break;
					case AXIS_Z: x = a; y = b; z = curFace.index; break;
				}
				VISIBLEBLOCKS();
				blocks[x][y][z]->Render();
			}
	Global::PopModelView();

	FORALLBLOCKS(x, y, z) {
		VISIBLEBLOCKS();
		if (curFace.normal == AXIS_X && x == curFace.index ||
			curFace.normal == AXIS_Y && y == curFace.index ||
			curFace.normal == AXIS_Z && z == curFace.index)
				continue;
			blocks[x][y][z]->Render();
	}
}

void Cube::MixupStep()
{
	Axis normal;
	int index;
	bool dir = (rand() % 2) ? true : false;

	do {
		normal = (Axis)(rand() % 3);
		index = rand() % size;
	} while (mixup.curSteps != mixup.maxSteps &&
		normal == mixup.prevNormal && index == mixup.prevIndex);

	BeginRotateFace(normal, index, dir);

	mixup.prevNormal = normal;
	mixup.prevIndex = index;
	mixup.curSteps--;
}

void Cube::InitBlockSides(CubeBlock *cb, int x, int y, int z)
{
	int i = 0;

	if (x == 0)
		cb->coloredSides[i++] = 0;
	else if (x == size - 1)
		cb->coloredSides[i++] = 2;

	if (y == 0)
		cb->coloredSides[i++] = 5;
	else if (y == size - 1)
		cb->coloredSides[i++] = 4;

	if (z == 0)
		cb->coloredSides[i++] = 1;
	else if (z == size - 1)
		cb->coloredSides[i++] = 3;

	cb->numSides = i;
	for (i = 0; i < cb->numSides; i++) {
		cb->clr.colorIndices[i] = cb->coloredSides[i];
	}
}

void Cube::TransformColors()
{
	for (int i = 0; i < size; i++)
		memset(tmp[i], -1, size*sizeof(BlockColor));

	int x, y, z;
	for (int a = 0; a < size; a++)
		for (int b = 0; b < size; b++)
		{
			switch (curFace.normal) {
				case AXIS_X: x = curFace.index; y = a; z = b; break;
				case AXIS_Y: x = a; y = curFace.index; z = b; break;
				case AXIS_Z: x = a; y = b; z = curFace.index; break;
			}
			VISIBLEBLOCKS();

			int r1, r2;
			int s = curFace.normal % 2 ? -1 : 1;
			if (curFace.dir*s > 0) {
				r1 = b;
				r2 = size - a - 1;
			}
			else {
				r1 = size - b - 1;
				r2 = a;
			}

			CubeBlock *cb = blocks[x][y][z];

			tmp[a][b] = cb->clr;
			BlockColor &tmpColor = tmp[r1][r2];
			
			if (tmpColor.colorIndices[0] != -1)
				cb->clr = tmpColor;
			else
			{
				CubeBlock *block = NULL;
				switch (curFace.normal) {
					case AXIS_X: block = blocks[x][r1][r2]; break;
					case AXIS_Y: block = blocks[r1][y][r2]; break;
					case AXIS_Z: block = blocks[r1][r2][z]; break;
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
		if (x != 0 && x != size - 1 && cb->numSides == 2)
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
		if (cb->numSides == 3 || z != 0 && z != size - 1 && cb->numSides == 2)
			Swap(c.colorIndices[0], c.colorIndices[1]);
	}
}

#undef FORALLBLOCKS
#undef VISIBLEBLOCKS