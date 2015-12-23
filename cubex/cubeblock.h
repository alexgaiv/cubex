#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "global.h"
#include "quaternion.h"
#include "vertexbuffer.h"

class Actor
{
public:
	Vector3f location;
	Quaternion rotation;
	Matrix44f transform;

	virtual void Render() = 0;

	void ApplyTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		Global::MultModelView(transform.data);
	}
};

class BlockColor
{
public:
	int colorIndices[3];
};

class CubeBlock : public Actor
{
public:
	static Color3b colors[6];
	static bool fRenderPickMode;
	static const float size;

	UINT pickId;
	int numSides;
	BlockColor clr;
	int coloredSides[3];

	CubeBlock(UINT pickId);

	static void InitStatic();
	static void FreeStatic();

	void Render();
	bool IsSideColored(int side);
private:
	static VertexBuffer *faces, *faces_pickMode, *edges;
	static float *verts;
	static float *ReadVertexData(HINSTANCE hInst, DWORD resourceId);

	Color3b *GetSideColor(int idx) {
		for (int i = 0; i < numSides; i++) {
			if (coloredSides[i] == idx)
				return &colors[clr.colorIndices[i]];
		}
		return NULL;
	}
};

#endif // _ACTOR_H_