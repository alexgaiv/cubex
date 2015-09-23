#ifndef _APP_WINDOW_H_
#define _APP_WINDOW_H_

#include "glwindow.h"
#include "viewer3d.h"
#include "cubeblock.h"

class AppWindow : public GLWindow
{
public:
	AppWindow();
private:
	Viewer3D viewer;
	CubeBlock *blocks[3][3][3];

	static Color3b colors[6];
	static int colorMap[3][3][3][3];
	static int orientation[3][3][3][4];
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
	} curPlane;

	void RotatePlane(Axis planeNormal, int index, bool clockWise);
	bool CheckIsSolved() const;

	void InitBlocks();
	void InitBlockSides(CubeBlock *cb, int x, int y, int z);
	void TransformColors();
	void OrientateColors(CubeBlock *cb, int x, int y);
	void AnimationStep();
	void RenderScene();

	void OnCreate();
	void OnDisplay();
	void OnTimer();
	void OnSize(int w, int h);
	void OnMouseDown(MouseButton button, int x, int y);
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnKeyDown(UINT keyCode) {
		if (keyCode >= 0x31 && keyCode <= 0x39) {
			keyCode -= 0x31;
			Axis axis = (Axis)((keyCode) / 3);
			RotatePlane(axis, keyCode % 3, (bool)GetAsyncKeyState(VK_SHIFT));
		}
	}
	void OnDestroy();
};

#endif // _APP_WINDOW_H_