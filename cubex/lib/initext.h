#ifndef _INITEXT_H
#define _INITEXT_H

#include "common.h"

struct ExtSupported {
	static bool VBO;
};

extern void (APIENTRYP glBindBuffer)(GLenum target, GLuint buffer);
extern void (APIENTRYP glDeleteBuffers)(GLsizei n, const GLuint *buffers);
extern void (APIENTRYP glGenBuffers)(GLsizei n, GLuint *buffers);
extern GLboolean (APIENTRYP glIsBuffer)(GLuint buffer);
extern void (APIENTRYP glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
extern void (APIENTRYP glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
extern void (APIENTRYP glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
extern void *(APIENTRYP glMapBuffer)(GLenum target, GLenum access);
extern GLboolean (APIENTRYP glUnmapBuffer)(GLenum target);
extern void (APIENTRYP glGetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
extern void (APIENTRYP glGetBufferPointerv)(GLenum target, GLenum pname, GLvoid **params);

namespace initext
{
	static class _Ext_init_helper
	{
	public:
		_Ext_init_helper();
		void InitVBO();
	} notused;
}

#endif // _INITEXT_H