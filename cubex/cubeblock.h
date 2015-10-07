#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "lib/quaternion.h"
#include "lib/vertexbuffer.h"

class Actor
{
public:
	Vector3f location;
	Quaternion rotation;
	Matrix44f transform;

	Actor() {
		rotation.LoadIdentity();
		transform.LoadIdentity();
	}

	virtual void Render() = 0;

	void ApplyTransform() {
		rotation.ToMatrix(transform);
		transform.translate = location;
		glMultMatrixf(transform.data);
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
	~CubeBlock();

	void Render();
	bool IsSideColored(int side);
private:
	static VertexBuffer *edges, *faces, *faces_pickMode;
	int sideId[6];

	static int InitVertices();

	Color3b *GetSideColor(int idx) {
		for (int i = 0; i < numSides; i++) {
			if (coloredSides[i] == idx)
				return &colors[clr.colorIndices[i]];
		}
		return NULL;
	}
};

#endif // _ACTOR_H_