#include "vertexbuffer.h"

VertexBuffer::VertexBuffer(GLenum target) : target(target) {
	glGenBuffers(1, &id);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &id);
}

void VertexBuffer::AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized, GLsizei stride, GLubyte offset) const
{
	Bind();
	glVertexAttribPointer(index, size, type, normalized, stride, (void *)offset);
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage) {
	Bind();
	glBufferData(target, size, data, usage);
}

void VertexBuffer::SetSubData(GLintptr offset, GLsizeiptr size, const void *data) {
	Bind();
	glBufferSubData(target, offset, size, data);
}

void VertexBuffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) const {
	Bind();
	glGetBufferSubData(target, offset, size, data);
}

int VertexBuffer::GetSize() const {
	int size = 0;
	Bind();
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
	return size;
}

GLenum VertexBuffer::GetUsage() const {
	Bind();
	int usage = 0;
	glGetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);
	return (GLenum)usage;
}

void *VertexBuffer::Map(GLenum access) const {
	Bind();
	return glMapBuffer(target, access);
}

bool VertexBuffer::Unmap() const {
	Bind();
	return glUnmapBuffer(target) == GL_TRUE;
}

void VertexBuffer::CloneTo(VertexBuffer &vb) const
{
	BYTE *data = (BYTE *)Map(GL_READ_ONLY);
	if (data) {
		vb.SetData(GetSize(), data, GetUsage());
		Unmap();
	}
}