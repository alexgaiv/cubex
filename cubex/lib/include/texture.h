#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "common.h"
#include "datatypes.h"
#include "glcontext.h"

#define TEX_GENERATE_ID GLuint(-1)

class BaseTexture
{
public:
	BaseTexture(GLenum target, GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_GENERATE_ID);
	BaseTexture(const BaseTexture &t);
	~BaseTexture();

	BaseTexture &operator=(const BaseTexture &t);

	void Bind(GLRenderingContext *rc);
	void Delete() { glDeleteTextures(1, &id); }

	GLuint GetId() const { return id; }
	GLenum GetTarget() const { return target; }
	GLenum GetTextureUnit() const { return textureUnit; }
	void SetTextureUnit(GLenum unit) { textureUnit = unit; }

	void SetFilters(GLint minFilter, GLint magFilter);
	void SetWrapMode(GLint wrapS, GLint wrapT, GLint wrapR = -1);
	void SetBorderColor(Color4f color);

	void BuildMipmaps();
protected:
	GLuint id;
	GLenum target, textureUnit;
	int width, height;
	int internalFormat, format;

	void _bind() {
		glActiveTexture(textureUnit);
		glBindTexture(target, id);
	}

	bool loadFromTGA(const char *name, BYTE *&data);
	void texImage2D(GLenum target, BYTE *imageData);
private:
	mutable int *refs;
	bool needDelete;
	void clone(const BaseTexture &t);
	void read(HANDLE hFile, LPVOID lpBuffer, DWORD nNumBytes);
};

class Texture2D : public BaseTexture
{
public:
	Texture2D(GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_GENERATE_ID);
	Texture2D(const char *name, GLenum textureUnit = GL_TEXTURE0, GLuint id = TEX_GENERATE_ID);
	Texture2D(const Texture2D &t);

	Texture2D &operator=(const Texture2D &t);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	bool IsLoaded() const { return loaded; }
	bool LoadFromTGA(const char *name);
	void SetTexImage(GLenum level, GLint internalFormat, GLsizei width, GLsizei height,
		GLint border, GLenum format, GLenum type, const GLvoid *data);
private:
	bool loaded;
};

class CubeTexture : public BaseTexture
{
public:
	CubeTexture(GLenum textureUnit = GL_TEXTURE0);
	CubeTexture(const char **sides, GLenum textureUnit = GL_TEXTURE0);
	CubeTexture(const CubeTexture &t);

	CubeTexture &operator=(const CubeTexture &t);

	bool IsLoaded() const { return loaded; }
	bool LoadFromTGA(const char **sides);
private:
	bool loaded;
};

#endif // _TEXTURE2D_H_