#ifndef _CUBE_H_
#define _CUBE_H_

#include "cubeblock.h"

enum Axis {
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2
};

struct BlockDesc
{
	Point3<int> pos;
	int side;
};

class Cube
{
public:
	Cube();
	~Cube();

	const float blockSize;
	const float rotateSpeed;

	CubeBlock *blocks[3][3][3];

	void GetBlockById(int id, BlockDesc &block);
	void Reset();
	void MixUp(int steps = 15);
	bool IsSolved() const;
	void BeginRotateFace(Axis faceNormal, int index, bool clockWise);
	bool AnimationStep();
	bool IsAnim() const { return curFace.anim; }
	void Render() const;
private:
	static int rot[2][3][3][2];
	bool reseting;

	struct {
		int curSteps;
		int maxSteps;
		Axis prevNormal;
		int prevIndex;
	} mixup;

	struct {
		bool anim;
		float angle;
		Axis normal;
		int index;
		int dir;
		Matrix44f mRot;
	} curFace;

	void DoReset();
	void MixupStep();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y, int z);
};

#endif // _CUBE_H_