#ifndef _VERTEX_BUFFER_H
#define _VERTEX_BUFFER_H

#include "common.h"

class VertexBuffer
{
public:
	VertexBuffer(GLenum target = GL_ARRAY_BUFFER);
	~VertexBuffer();

	GLuint GetId() const { return id; }
	GLenum GetTarget() const { return target; }
	void SetTarget(GLenum target) { this->target = target; }

	void AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized = GL_FALSE, GLsizei stride = 0, GLubyte offset = 0) const;

	void VertexPointer(GLint size, GLenum type, GLsizei stride) const;
	void NormalPointer(GLenum type, GLsizei stride) const;
	void TexCoordPointer(GLint size, GLenum type, GLsizei stride) const;

	void DrawElements(GLenum mode, GLsizei count, GLenum type, int first = 0);

	void Bind() const { glBindBuffer(target, id); }
	void Unbind() const { glBindBuffer(target, 0); }

	void SetData(GLsizeiptr size, const void *data, GLenum usage);
	void SetSubData(GLintptr offset, GLsizeiptr size, const void *data);
	void GetSubData(GLintptr offset, GLsizeiptr size, void *data) const;
	int GetSize() const;
	GLenum GetUsage() const;

	void *Map(GLenum access) const;
	bool Unmap() const;

	void CloneTo(VertexBuffer &vb) const;
private:
	GLuint id;
	GLenum target;

	BYTE *data;
	int size;
};

#endif // _VERTEX_BUFFER_H