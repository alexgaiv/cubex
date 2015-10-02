#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "lib/quaternion.h"

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

	float size;
	UINT pickId;
	int numSides;
	BlockColor clr;
	int coloredSides[3];

	CubeBlock(float size)
		: size(size), pickId(0), numSides(0)
	{
		memset(&clr, 0, sizeof(clr));
	}
	void Render();
	bool IsSideColored(int side);
private:
	Color3b *GetSideColor(int idx) {
		for (int i = 0; i < numSides; i++) {
			if (coloredSides[i] == idx)
				return &colors[clr.colorIndices[i]];
		}
		return NULL;
	}
	UINT GetSideId(int i) { return pickId | (1 << (i+5)); }
};

#endif // _ACTOR_H_