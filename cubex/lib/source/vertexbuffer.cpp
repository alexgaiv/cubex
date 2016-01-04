#include "vertexbuffer.h"

VertexBuffer::VertexBuffer(GLenum target) : target(target), data(NULL), size(0)
{
	if (GLEW_ARB_vertex_buffer_object)
		glGenBuffers(1, &id);
}

VertexBuffer::~VertexBuffer()
{
	if (GLEW_ARB_vertex_buffer_object)
		glDeleteBuffers(1, &id);
	else if (data) delete [] data;
}

void VertexBuffer::AttribPointer(GLuint index, GLint size, GLenum type,
		GLboolean normalized, GLsizei stride, GLubyte offset) const
{
	Bind();
	glVertexAttribPointer(index, size, type, normalized, stride, (void *)offset);
}

void VertexBuffer::VertexPointer(GLint size, GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glVertexPointer(size, type, stride, 0);
	}
	else {
		glVertexPointer(size, type, stride, data);
	}
}

void VertexBuffer::NormalPointer(GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glNormalPointer(type, stride, 0);
	}
	else {
		glNormalPointer(type, stride, data);
	}
}

void VertexBuffer::TexCoordPointer(GLint size, GLenum type, GLsizei stride) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glTexCoordPointer(size, type, stride, 0);
	}
	else {
		glTexCoordPointer(size, type, stride, data);
	}
}

void VertexBuffer::DrawElements(GLenum mode, GLsizei count, GLenum type, int first)
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glDrawElements(mode, count, type, (void *)first);
	}
	else {
		glDrawElements(mode, count, type, data);
	}
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage)
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glBufferData(target, size, data, usage);
	}
	else {
		this->size = size;
		if (this->data) delete [] this->data;
		this->data = new BYTE[size];
		memcpy(this->data, data, size);
	}
}

void VertexBuffer::SetSubData(GLintptr offset, GLsizeiptr size, const void *data)
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glBufferSubData(target, offset, size, data);
	}
	else if (offset < this->size) {
		memcpy(this->data + offset, data, max(size, this->size - offset));
	}
}

void VertexBuffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		glGetBufferSubData(target, offset, size, data);
	}
	else if (offset < this->size) {
		memcpy(data, this->data + offset, max(size, this->size - offset));
	}
}

int VertexBuffer::GetSize() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		int size = 0;
		Bind();
		glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
		return size;
	}
	return this->size;
}

GLenum VertexBuffer::GetUsage() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		int usage = 0;
		glGetBufferParameteriv(target, GL_BUFFER_USAGE, &usage);
		return (GLenum)usage;
	}
	else return GL_STATIC_DRAW;
}

void *VertexBuffer::Map(GLenum access) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		return glMapBuffer(target, access);
	}
	else return data;
}

bool VertexBuffer::Unmap() const
{
	if (GLEW_ARB_vertex_buffer_object) {
		Bind();
		return glUnmapBuffer(target) == GL_TRUE;
	}
	return true;
}

void VertexBuffer::CloneTo(VertexBuffer &vb) const
{
	if (GLEW_ARB_vertex_buffer_object) {
		BYTE *data = (BYTE *)Map(GL_READ_ONLY);
		if (data) {
			vb.SetData(GetSize(), data, GetUsage());
			Unmap();
		}
	}
	else {
		if (vb.data) delete [] vb.data;
		vb.size = size;
		vb.data = new BYTE[size];
		memcpy(vb.data, data, size);
	}
}