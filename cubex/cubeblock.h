#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "quaternion.h"

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
	Color3b sides[3];
};

class CubeBlock : public Actor
{
public:
	float size;
	UINT pickId;
	int numSides;
	bool swapSides;
	BlockColor clr;

	int coloredSides[3];

	UINT GetSideId(int i) { return pickId | (1 << (i+5)); }

	CubeBlock(float size, Color3b color = Color3b())
		: size(size), numSides(0), swapSides(false)
	{
		memset(coloredSides, 0, sizeof(int)*3);
	}
	void Render();
};

#endif // _ACTOR_H_