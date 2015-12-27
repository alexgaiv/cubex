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
	static bool fRenderPickMode;
	static bool fUseReducedModel;
	static bool fWhiteBorders;
	static const float size;

	static void DrawWhiteBorders(bool whiteBorders);
	
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

#endif // _ACTOR_H_