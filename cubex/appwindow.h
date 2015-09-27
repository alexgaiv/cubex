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
	bool mouseFlag, rotateScene;

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
		Axis face1, face2;
		int index1, index2;
		Vector3d dir1, dir2, mouseDelta;
	} drag;

	void RotateFace(Axis faceNormal, int index, bool clockWise);
	bool CheckIsSolved() const;

	void InitBlocks();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y, int z);
	void AnimationStep();
	void RenderScene();

	bool GetBlockUnderMouse(int winX, int winY, int &x, int &y, int &z, int &side);
	void InitDrag(int x, int y, int z, int side, Point3<int> &a, Point3<int> &b);

	void OnCreate();
	void OnDisplay();
	void OnTimer();
	void OnSize(int w, int h);
	void OnMouseDown(MouseButton button, int x, int y);
	void OnMouseUp(MouseButton button, int x, int y);
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnKeyDown(UINT keyCode) {
		if (keyCode >= 0x31 && keyCode <= 0x39) {
			keyCode -= 0x31;
			Axis axis = (Axis)((keyCode) / 3);
			RotateFace(axis, keyCode % 3, GetAsyncKeyState(VK_SHIFT) ? true : false);
		}
	}
	void OnDestroy();
};

#endif // _APP_WINDOW_H_