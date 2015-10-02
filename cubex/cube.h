#ifndef _CUBE_H_
#define _CUBE_H_

#include "cubeblock.h"

enum Axis {
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2
};

class Cube
{
public:
	Cube();
	~Cube();

	const float blockSize;
	const float blockSpace;
	const float rotateSpeed;

	CubeBlock *blocks[3][3][3];

	void Reset();
	bool CheckIsSolved() const;
	void Shuffle(int count = 15);
	void BeginRotateFace(Axis faceNormal, int index, bool clockWise);
	bool AnimationStep();
	bool IsAnim() const { return curFace.anim; }
	void Render() const;
private:
	static int rot[2][3][3][2];
	bool reseting;

	struct {
		int curCount;
		int maxCount;
		Axis prevNormal;
		int prevIndex;
	} shuffle;

	struct {
		bool anim;
		float angle;
		Axis normal;
		int index;
		int dir;
		Matrix44f mRot;
	} curFace;

	void DoReset();
	void ShuffleStep();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y, int z);
};

#endif // _CUBE_H_