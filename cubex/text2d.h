#ifndef _TEXT_2D_
#define _TEXT_2D_

#include "vertexbuffer.h"
#include "texture.h"
#include "glcontext.h"

class Text2D
{
public:
	Text2D(GLRenderingContext *rc, const char *name);
	Text2D(const Text2D &t);
	~Text2D();
	void SetText(const wchar_t *text);
	void Draw(int x, int y);

	Text2D &operator=(const Text2D &t);
private:
	struct Charset {
		int base;
		int startChar;
		int endChar;
	};

	GLRenderingContext *rc;
	int numChars, numCharsets;
	Charset *charsets;
	int *charWidth;

	Texture2D fontTexture;
	int cellWidth, cellHeight;
	int fontHeight; 

	int numVerts;
	VertexBuffer vertices, texCoords;

	void clone(const Text2D &t);
};

#endif // _TEXT_2D_