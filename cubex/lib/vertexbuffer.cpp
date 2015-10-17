#include "vertexbuffer.h"

VertexBuffer::VertexBuffer() : target(0)
{
	glGenBuffers(1, &id);
	int a = glGetError();
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &id);
}

void VertexBuffer::Bind(GLenum target) {
	this->target = target;
	glBindBuffer(target, id);
}

void VertexBuffer::Unbind(GLenum target) {
	glBindBuffer(target, 0);
}

void VertexBuffer::SetData(GLsizeiptr size, const void *data, GLenum usage) {
	glBufferData(target, size, data, usage);
}

void VertexBuffer::SetSubData(GLintptr offset, GLsizeiptr size, const void *data) {
	glBufferSubData(target, offset, size, data);
}

void VertexBuffer::GetSubData(GLintptr offset, GLsizeiptr size, void *data) {
	glGetBufferSubData(target, offset, size, data);
}

void *VertexBuffer::Map(GLenum access) {
	return glMapBuffer(target, access);
}

bool VertexBuffer::Unmap() {
	return glUnmapBuffer(target) == GL_TRUE;
}