#ifndef _GL_WINDOW_H_
#define _GL_WINDOW_H_

#include <string>
#include "resources.h"
#include "glwindow.h"
#include "shader.h"
#include "viewer3d.h"
#include "qslerp.h"
#include "text2d.h"
#include "cube.h"
#include "cubecontext.h"

#define WM_ROTATEFACE (WM_USER + 1)
#define WM_CUBESOLVED (WM_USER + 2)

class GLFrame : public GLWindow
{
public:
	GLFrame();
	~GLFrame();

	int GetCubeSize() const { return ctx->cube->size; }
	bool CanCancelMove() { return ctx->CanCancelMove(); }
	bool ChangeCubeSize(int size);
	void ResetCube();
	void ScrambleCube();
	void CancelMove();
	void SetCubeStyle(bool whiteBorders);
private:
	Viewer3D *viewer;
	ProgramObject *program;
	CubeContext *cubes[6];
	CubeContext *ctx;

	QSlerp resetAnim;
	Point2i mousePos;
	bool faceDrag, sceneDrag;
	bool needRedraw;

	Font2D *font;
	Text2D *timeMsg;
	Text2D *movesMsg;

	struct {
		Axis face[2];
		int index[2];
		Vector3f dir[2];
		bool neg[2];
	} drag;

	void SaveConfig();
	void LoadConfig();

	void ConstructText();
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

	void OnCubeSolved();

	bool GetBlockUnderMouse(int winX, int winY, BlockDesc &block);
	void GetNeighbors(const BlockDesc &block, Point3i &neighbor1, Point3i &neighbor2);
	void CalcRotDirections(const BlockDesc &block);
	void MouseRot(const Vector3f &mouseDir);

	Vector3f CalcBlockWinPos(const Point3i &block,
		const Matrix44f &modelview,
		const Matrix44f &projection, int viewport[4]);
};

#endif // _GL_WINDOW_H_