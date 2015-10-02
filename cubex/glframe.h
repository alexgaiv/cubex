#ifndef _APP_WINDOW_H_
#define _APP_WINDOW_H_

#include "lib/glwindow.h"
#include "lib/viewer3d.h"
#include "cube.h"

#define WM_ROTATEFACE (WM_USER+1)

template<class T, int capacity>
class CircularArray
{
public:
	CircularArray() : size(0), cur(0) { }
	int GetSize() { return size; }
	void Clear() { size = cur = 0; }

	void Add(T val) {
		if (cur == capacity) cur = 0;
		if (size != capacity) size++;
		data[cur++] = val;
	}
	T *Delete() {
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

	void ResetCube() {
		cube->Reset();
		history.Clear();
		RedrawWindow();
	}

	void ShuffleCube() {
		cube->Shuffle();
		history.Clear();
	}

	void CancelMove() {
		if (!cube->IsAnim()) {
			MoveDesc *m = history.Delete();
			if (m) cube->BeginRotateFace(m->normal, m->index, !m->clockWise);	
		}
	}
	bool CanCancelMove() { return history.GetSize() != 0; }
private:
	Cube *cube;
	Viewer3D viewer;

	Point2i mousePos;
	bool faceRot, sceneRot;

	struct MoveDesc {
		Axis normal;
		int index;
		bool clockWise;
	};

	CircularArray<MoveDesc, 100> history;

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
	void OnMouseMove(UINT keysPressed, int x, int y);
	void OnMouseUp(MouseButton button, int x, int y);
	void OnDestroy();
};

#endif // _APP_WINDOW_H_