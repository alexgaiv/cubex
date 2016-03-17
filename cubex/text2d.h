#ifndef _TEXT_2D_
#define _TEXT_2D_

#include "vertexbuffer.h"
#include "texture.h"
#include "glcontext.h"

class Text2D;

class Font2D
{
public:
	Font2D(const char *name);
	Font2D(const Font2D &f);
	~Font2D();
	
	Font2D &operator=(const Font2D &f);
private:
	friend class Text2D;

	struct Charset {
		int base;
		int startChar;
		int endChar;
	};

	int numChars, numCharsets;
	Charset *charsets;
	int *charWidth;

	Texture2D fontTexture;
	int numCellsX, numCellsY;
	float fontHeight;

	float dty;
	float numCellsX_inv, numCellsY_inv;
	float texWidth_inv;

	void load(const char *name);
	void clone(const Font2D &f);
};

class Text2D
{
public:
	Text2D(GLRenderingContext *rc, Font2D *font);
	Text2D(const Text2D &t);

	void SetFont(Font2D &font);
	void SetText(const wchar_t *text);
	void Draw(int x, int y);

	Text2D &operator=(const Text2D &t);
private:
	GLRenderingContext *rc;
	Font2D *font;
	int numVerts;
	VertexBuffer vertices, texCoords;

	static const char *shaderSource[2];

	void clone(const Text2D &t);
};

#endif // _TEXT_2D_