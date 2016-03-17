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
	Point3i pos;
	int side;
};

class Cube
{
public:
	Cube(GLRenderingContext *rc, int size);
	~Cube();

	void Serialize(ofstream &os);
	void Deserialize(ifstream &is);

	const int size;
	const float blockSize;
	const float rotateSpeed;
	static const int GOD_NUMBER = 2;

	CubeBlock ****blocks;

	void GetBlockById(int id, BlockDesc &block) const;
	void Reset();
	void Scramble(int steps = 15);
	bool IsSolved() const;
	void BeginRotateFace(Axis faceNormal, int index, bool clockWise);
	bool AnimationStep();
	bool IsAnim() const { return curFace.anim; }
	void Render() const;
private:
	GLRenderingContext *rc;
	BlockColor **tmp;
	bool reseting;

	struct {
		int curSteps;
		int maxSteps;
		Axis prevNormal;
		int prevIndex;
	} scramble;

	struct {
		bool anim;
		float angle;
		Axis normal;
		int index;
		int dir;
		Matrix44f mRot;
	} curFace;

	void DoReset();
	void ScrambleStep();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y, int z);
};


#endif // _CUBE_H_