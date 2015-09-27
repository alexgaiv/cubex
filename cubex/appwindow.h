#ifndef _APP_WINDOW_H_
#define _APP_WINDOW_H_

#include "lib/glwindow.h"
#include "lib/viewer3d.h"
#include "cubeblock.h"

class AppWindow : public GLWindow
{
public:
	AppWindow();
private:
	const float blockSize;
	const float blockSpace;
	const float rotateSpeed;

	Viewer3D viewer;
	CubeBlock *blocks[3][3][3];
	Point2i mousePos;
	bool faceRot, sceneRot;

	static int rot[2][3][3][2];

	enum Axis {
		AXIS_X = 0,
		AXIS_Y = 1,
		AXIS_Z = 2
	};

	struct {
		bool anim;
		float angle;
		Axis normal;
		int index;
		int dir;
		Matrix44f mRot;
	} curFace;

	struct {
		Axis face[2];
		int index[2];
		Vector3d dir[2];
		bool neg[2];
	} drag;

	struct BlockDesc
	{
		Point3<int> pos;
		int side;
	};

	void RotateFace(Axis faceNormal, int index, bool clockWise);
	bool CheckIsSolved() const;

	void InitBlocks();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y, int z);
	void AnimationStep();
	void RenderScene();

	void GetBlockById(int id, BlockDesc &block);
	bool GetBlockUnderMouse(int winX, int winY, BlockDesc &block);
	void GetNeighbors(const BlockDesc &block, Point3<int> &neighbor1, Point3<int> &neighbor2);
	void CalcRotDirections(const BlockDesc &block);
	void MouseRot(const Vector3d &mouseDir);

	Vector3d CalcBlockWinPos(const Point3<int> &block,
		const Matrix44d &modelview,
		const Matrix44d &projection, int viewport[4]);

	void SetPerspective(int w, int h);

	void OnCreate();
	void OnDisplay();
	void OnTimer();
	void OnSize(int w, int h);
	void OnMouseDown(MouseButton button, int x, int y);
	void OnMouseUp(MouseButton button, int x, int y);
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnDestroy();
};

#endif // _APP_WINDOW_H_