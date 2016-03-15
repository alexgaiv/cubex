#include "text2d.h"
#include "datatypes.h"
#include "transform.h"
#include "glcontext.h"

Text2D::Text2D(GLRenderingContext *rc, const char *name)
	: rc(rc), charsets(NULL), charWidth(NULL)
{
	numVerts = numCharsets = 0;
	cellWidth = cellHeight = fontHeight = 0;

	char *data = NULL;
	int len = 0;
	GetTextResource(name, data, len);
	if (!data) return;

	int curCharset = 0;

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
		else if (!strcmp(prefix, "cw"))
			sscanf_s(data, "%d", &cellWidth);
		else if (!strcmp(prefix, "ch"))
			sscanf_s(data, "%d", &cellHeight);
		else if (!strcmp(prefix, "fh"))
			sscanf_s(data, "%d", &fontHeight);
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
}

Text2D::Text2D(const Text2D &t) {
	clone(t);
}

Text2D &Text2D::operator=(const Text2D &t)
{
	if (charsets) delete [] charsets;
	if (charWidth) delete [] charWidth;
	clone(t);
	return *this;
}

void Text2D::clone(const Text2D &t)
{
	rc = t.rc;
	numChars = t.numChars;
	numCharsets = t.numCharsets;
	fontTexture = t.fontTexture;
	cellWidth = t.cellWidth;
	cellHeight = t.cellHeight;
	fontHeight = t.fontHeight;
	numVerts = t.numVerts;

	charsets = new Charset[numCharsets];
	for (int i = 0; i < numCharsets; i++)
		charsets[i] = t.charsets[i];

	charWidth = new int[numChars];
	for (int i = 0; i < numChars; i++)
		charWidth[i] = t.charWidth[i];

	if (numVerts != 0) {
		t.vertices.CloneTo(vertices);
		t.texCoords.CloneTo(texCoords);
	}
}

Text2D::~Text2D()
{
	if (charsets) delete [] charsets;
	if (charWidth) delete [] charWidth;
}

void Text2D::SetText(const wchar_t *text)
{
	numVerts = 6*wcslen(text);
	Vector3f *verts = new Vector3f[numVerts];
	Vector2f *texs = new Vector2f[numVerts];

	float dw = 0;
	for (int i = 0; *text; i++, text++)
	{
		wchar_t ch = *text;
		int index = 0;
		int x = 0, y = 0;

		for (int j = 0; j < numCharsets; j++)
		{
			Charset &c = charsets[j];
			if (ch >= c.startChar && ch <= c.endChar)
			{
				index = ch - c.startChar + c.base;
				x = index % (fontTexture.GetWidth() / cellWidth);
				y = index / (fontTexture.GetHeight() / cellHeight);
				break;
			}
		}

		int w = charWidth[index];
		int k = 6*i;
		verts[k]   = Vector3f(dw, 0, 0);
		verts[k+1] = Vector3f(dw, (float)fontHeight, 0);
		verts[k+2] = Vector3f(dw + w, 0, 0);
		verts[k+3] = verts[k+2];
		verts[k+4] = verts[k+1];
		verts[k+5] = Vector3f(dw + w, (float)fontHeight, 0);

		float kw = x * (float)cellWidth / fontTexture.GetWidth();
		float kh = y * (float)cellHeight / fontTexture.GetHeight();
		float kf = (float)fontHeight / fontTexture.GetHeight();
		float kc = (float)w / fontTexture.GetHeight();

		texs[k]   = Vector2f(kw, kh);
		texs[k+1] = Vector2f(kw, kh + kf);
		texs[k+2] = Vector2f(kw + kc, kh);
		texs[k+3] = texs[k+2];
		texs[k+4] = texs[k+1];
		texs[k+5] = Vector2f(kw + kc, kh + kf);

		dw += w;
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

	fontTexture.Bind(rc);

	glEnableVertexAttribArray(AttribsLocations.Vertex);
	vertices.Bind();
	vertices.AttribPointer(AttribsLocations.Vertex, 3, GL_FLOAT);

	glEnableVertexAttribArray(AttribsLocations.TexCoord);
	texCoords.Bind();
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