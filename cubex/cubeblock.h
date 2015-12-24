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
	static Color3f colors[7];
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
	static Mesh *face, *edges, *face_pickMode;
	static Matrix44f face_transform[6];

	Color3f *GetSideColor(int idx) {
		for (int i = 0; i < numSides; i++) {
			if (coloredSides[i] == idx)
				return &colors[clr.colorIndices[i]];
		}
		return &colors[6];
	}
};

#endif // _ACTOR_H_