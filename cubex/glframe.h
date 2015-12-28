#ifndef _APP_WINDOW_H_
#define _APP_WINDOW_H_

#include <string>
#include <time.h>
#include "glwindow.h"
#include "shader.h"
#include "viewer3d.h"
#include "qslerp.h"
#include "cube.h"

#define WM_ROTATEFACE (WM_USER+1)
#define WM_CUBESOLVED (WM_USER+2)

template<class T, int capacity>
class CircularStack
{
public:
	CircularStack() : size(0), cur(0) { }
	int GetSize() { return size; }
	void Clear() { size = cur = 0; }

	void Push(T val) {
		if (cur == capacity) cur = 0;
		if (size != capacity) size++;
		data[cur++] = val;
	}
	T *Pop() {
		if (size == 0) return NULL;
		if (cur > 0) cur--;
		else cur = capacity - 1;
		size--;
		return &data[cur];
	}
private:
	T data[capacity];
	int size, cur;
};

class GLFrame : public GLWindow
{
public:
	GLFrame();
	~GLFrame();

	bool ChangeCubeSize(int size);
	void ResetCube();
	void MixUpCube();
	void SetCubeStyle(bool whiteBorders);
	void CancelMove();
	bool CanCancelMove() { return history.GetSize() != 0; }
private:
	ProgramObject *program;
	Cube *cube;
	Viewer3D viewer;

	time_t solveTime;
	bool wasMixed;
	int numMoves, numActualMoves;
	bool fSolvedAnim;
	float rotAngle;
	static Quaternion qResetView;
	QSlerp resetAnim;
	Point2i mousePos;
	bool faceDrag, sceneDrag;
	bool isFaceRotating, isMixing;
	bool needRedraw;

	std::string timeMsg;
	std::string movesMsg;

	struct {
		Axis face[2];
		int index[2];
		Vector3d dir[2];
		bool neg[2];
	} drag;

	struct MoveDesc {
		Axis normal;
		int index;
		bool clockWise;
	};

	CircularStack<MoveDesc, 100> history;

	void SetPerspective(int w, int h);
	void RenderScene();

	void OnCreate();
	void OnDisplay() { RenderScene(); }
	void OnSize(int w, int h);
	void OnMouseDown(MouseButton button, int x, int y);
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnMouseUp(MouseButton button, int x, int y);
	void OnTimer();
	void OnDestroy();

	void OnFaceRotated();
	void OnMixed();
	void OnCubeSolved();

	bool GetBlockUnderMouse(int winX, int winY, BlockDesc &block);
	void GetNeighbors(const BlockDesc &block, Point3i &neighbor1, Point3i &neighbor2);
	void CalcRotDirections(const BlockDesc &block);
	void MouseRot(const Vector3d &mouseDir);

	Vector3d CalcBlockWinPos(const Point3i &block,
		const Matrix44d &modelview,
		const Matrix44d &projection, int viewport[4]);
};

#endif // _APP_WINDOW_H_