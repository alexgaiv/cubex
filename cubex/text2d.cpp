#include "text2d.h"
#include "datatypes.h"
#include "transform.h"
#include "glcontext.h"

Font2D::Font2D(const char *name)
{
	numChars = numCharsets = 0;
	charsets = NULL;
	charWidth = NULL;
	numCellsX = numCellsY = 0;
	fontHeight = 0;
	load(name);
}

Font2D::Font2D(const Font2D &f) {
	clone(f);
}

Font2D::~Font2D()
{
	if (charsets) delete [] charsets;
	if (charWidth) delete [] charWidth;
	fontTexture.Delete();
}

Font2D &Font2D::operator=(const Font2D &f)
{
	if (charsets) delete [] charsets;
	if (charWidth) delete [] charWidth;
	clone(f);
	return *this;
}

void Font2D::clone(const Font2D &f)
{
	numChars = f.numChars;
	numCharsets = f.numCharsets;

	if (f.charsets) {
		charsets = new Charset[numCharsets];
		memcpy(charsets, f.charsets, numCharsets * sizeof(Charset));
	}
	else charsets = NULL;

	if (f.charWidth) {
		charWidth = new int[numChars];
		memcpy(charWidth, f.charWidth, numChars * sizeof(int));
	}
	else charWidth = NULL;

	fontTexture = f.fontTexture;
	numCellsX = f.numCellsX;
	numCellsY = f.numCellsY;
	fontHeight = f.fontHeight;

	dty = f.dty;
	numCellsX_inv = f.numCellsX_inv;
	numCellsY_inv = f.numCellsY_inv;
	texWidth_inv = f.texWidth_inv;
}

void Font2D::load(const char *name)
{
	char *data = NULL;
	int len = 0;
	GetTextResource(name, data, len);
	if (!data) return;

	int curCharset = 0;
	int cellWidth = 0, cellHeight = 0;

	while (*data)
	{
		char ch = *data;
		char prefix[3] = { };
		for (int i = 0; i < 2 && *data; i++)
			prefix[i] = *data++;

		if (!strcmp(prefix, "nc"))
			sscanf_s(data, "%d", &numChars);
		else if (!strcmp(prefix, "ns")) {
			sscanf_s(data, "%d", &numCharsets);
			charsets = new Charset[numCharsets];
			ZeroMemory(charsets, numCharsets*sizeof(Charset));
		}
		else if (!strcmp(prefix, "f "))
		{
			char path[MAX_PATH] = { };
			for (int i = 0; *data && *data != '\n' && *data != '\r'; i++, data++)
				path[i] = *data;

			fontTexture.LoadFromTGA(path);
			fontTexture.SetFilters(GL_LINEAR, GL_LINEAR);
		}
		else if (*data++ != ' ') continue;
		else if (!strcmp(prefix, "cw")) {
			sscanf_s(data, "%d", &cellWidth);
		}
		else if (!strcmp(prefix, "ch")) {
			sscanf_s(data, "%d", &cellHeight);
		}
		else if (!strcmp(prefix, "fh")) {
			int fh = 0;
			sscanf_s(data, "%d", &fh);
			fontHeight = (float)fh;
		}
		else if (!strcmp(prefix, "fw"))
		{
			charWidth = new int[numChars];
			for (int i = 0; i < numChars; i++) {
				sscanf_s(data, "%d", &charWidth[i]);
				while (*data && *data++ != ' ' && *data != '\n');
			}
		}
		else if (!strcmp(prefix, "cs")) {
			if (curCharset < numCharsets) {
				Charset charset = { };
				sscanf_s(data, "%d %d %d", &charset.base, &charset.startChar, &charset.endChar);
				charsets[curCharset++] = charset;
			}
		}
			
		while (*data && *data++ != '\n');
		while (*data == '\r' || *data == '\n') data++;
	}

	if (!fontTexture.IsLoaded()) return;
	dty = fontHeight / fontTexture.GetHeight();
	texWidth_inv = 1.0f / fontTexture.GetWidth();

	if (cellWidth != 0) {
		numCellsX = fontTexture.GetWidth() / cellWidth;
		numCellsX_inv = 1.0f / numCellsX;
	}
	if (cellHeight != 0) {
		numCellsY = fontTexture.GetHeight() / cellHeight;
		numCellsY_inv = 1.0f / numCellsY;
	}
}

const char *Text2D::shaderSource[2] = 
{
	"#version 130\n"
	"in vec3 Vertex;"
	"in vec2 TexCoord;"
	"out vec2 fTexCoord;"
	"uniform mat4 ModelViewProjection;"
	"void main() {"
		"fTexCoord = TexCoord;"
		"gl_Position = ModelViewProjection * vec4(Vertex, 1.0);"
	"}"
	,
	"#version 130\n"
	"in vec2 fTexCoord;"
	"uniform sampler2D ColorMap;"
	"uniform vec3 Color;"
	"void main() {"
		"gl_FragColor = vec4(Color, texture(ColorMap, fTexCoord).a)"
	"}"
};

Text2D::Text2D(GLRenderingContext *rc, Font2D *font)
	: rc(rc), font(font), numVerts(0)
{

}

Text2D::Text2D(const Text2D &t) {
	clone(t);
}

Text2D &Text2D::operator=(const Text2D &t)
{
	clone(t);
	return *this;
}

void Text2D::clone(const Text2D &t)
{
	rc = t.rc;
	font = t.font;
	if (t.numVerts != 0) {
		t.vertices.CloneTo(vertices);
		t.texCoords.CloneTo(texCoords);
	}
}

void Text2D::SetText(const wchar_t *text)
{
	numVerts = 6*wcslen(text);
	Vector3f *verts = new Vector3f[numVerts];
	Vector2f *texs = new Vector2f[numVerts];

	float w = 0;
	for (int i = 0; *text; i++, text++)
	{
		wchar_t ch = *text;
		int index = 0;
		int x = 0, y = 0;

		for (int j = 0; j < font->numCharsets; j++)
		{
			Font2D::Charset &c = font->charsets[j];
			if (ch >= c.startChar && ch <= c.endChar)
			{
				index = ch - c.startChar + c.base;
				x = index % font->numCellsX;
				y = index / font->numCellsY;
				break;
			}
		}

		float dw = (float)font->charWidth[index];
		int k = 6*i;
		verts[k]   = Vector3f(w, 0, 0);
		verts[k+1] = Vector3f(w, font->fontHeight, 0);
		verts[k+2] = Vector3f(w + dw, 0, 0);
		verts[k+3] = verts[k+2];
		verts[k+4] = verts[k+1];
		verts[k+5] = Vector3f(w + dw, font->fontHeight, 0);

		float tx = (float)x * font->numCellsX_inv;
		float ty = (float)y * font->numCellsY_inv;
		float dtx = dw * font->texWidth_inv;
		
		texs[k]   = Vector2f(tx, ty);
		texs[k+1] = Vector2f(tx, ty + font->dty);
		texs[k+2] = Vector2f(tx + dtx, ty);
		texs[k+3] = texs[k+2];
		texs[k+4] = texs[k+1];
		texs[k+5] = Vector2f(tx + dtx, ty + font->dty);

		w += dw;
	}
	
	vertices.SetData(numVerts*sizeof(Vector3f), verts, GL_STATIC_DRAW);
	texCoords.SetData(numVerts*sizeof(Vector2f), texs, GL_STATIC_DRAW);

	delete [] verts;
	delete [] texs;
}

void Text2D::Draw(int x, int y)
{
	float viewport[4] = { };
	glGetFloatv(GL_VIEWPORT, viewport);

	rc->GetCurProgram()->Uniform("Mode", 4);
	rc->PushProjection();
	rc->PushModelView();

	rc->SetProjection(Ortho2D(0, viewport[2], viewport[3], 0));
	rc->SetModelView(Translate((float)x, (float)y, 0));

	font->fontTexture.Bind(rc);

	glEnableVertexAttribArray(AttribsLocations.Vertex);
	vertices.AttribPointer(AttribsLocations.Vertex, 3, GL_FLOAT);

	glEnableVertexAttribArray(AttribsLocations.TexCoord);
	texCoords.AttribPointer(AttribsLocations.TexCoord, 2, GL_FLOAT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, numVerts);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(AttribsLocations.Vertex);
	glDisableVertexAttribArray(AttribsLocations.TexCoord);
	rc->PopModelView();
	rc->PopProjection();
}