#ifndef _CUBE_BLOCK_H_
#define _CUBE_BLOCK_H_

#include "quaternion.h"
#include "vertexbuffer.h"
#include "mesh.h"

class Actor
{
public:
	Vector3f location;
	Quaternion rotation;
	float scale;
	Matrix44f transform;
	Mesh mesh;

	Actor(GLRenderingContext *rc) : rc(rc), mesh(rc), scale(1.0f) { }

	void ApplyTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		if (scale != 1.0f) transform.Scale(scale);
		rc->MultModelView(transform);
	}

	void Draw() {
		rc->PushModelView();
		ApplyTransform();
		mesh.Draw();
		rc->PopModelView();
	}
protected:
	GLRenderingContext *rc;
};

class BlockColor
{
public:
	int colorIndices[3];
};

class CubeBlock : public Actor
{
public:
	static bool fRenderPickMode;
	static bool fUseReducedModel;
	static const float size;

	static void DrawWhiteBorders(GLRenderingContext *rc, bool whiteBorders);
	
	UINT pickId;
	int numSides;
	BlockColor clr;
	int coloredSides[3];

	CubeBlock(GLRenderingContext *rc, UINT pickId);

	static void InitStatic(GLRenderingContext *rc);
	static void FreeStatic();

	void Render();
	bool IsSideColored(int side);
private:
	static Color3f colors[6];
	static Color3f borderDiffuse, borderAmbient;
	static Mesh *face, *border, *border_reduced, *face_pickMode;
	static Matrix44f face_transform[6];

	void RenderFixed();

	Color3f *GetSideColor(int idx) {
		for (int i = 0; i < numSides; i++) {
			if (coloredSides[i] == idx)
				return &colors[clr.colorIndices[i]];
		}
		return &borderDiffuse;
	}
};

#endif // _CUBE_BLOCK_H_