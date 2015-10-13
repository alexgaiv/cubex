#include "initext.h"

bool ExtSupported::VBO = false;

void (APIENTRYP glBindBuffer)(GLenum target, GLuint buffer);
void (APIENTRYP glDeleteBuffers)(GLsizei n, const GLuint *buffers);
void (APIENTRYP glGenBuffers)(GLsizei n, GLuint *buffers);
GLboolean (APIENTRYP glIsBuffer)(GLuint buffer);
void (APIENTRYP glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
void (APIENTRYP glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void (APIENTRYP glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
void *(APIENTRYP glMapBuffer)(GLenum target, GLenum access);
GLboolean (APIENTRYP glUnmapBuffer)(GLenum target);
void (APIENTRYP glGetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
void (APIENTRYP glGetBufferPointerv)(GLenum target, GLenum pname, GLvoid **params);

void InitGlExtensios()
{
	static bool init = false;
	if (init) return;
	init = true;

	(PROC &)glBindBuffer = wglGetProcAddress("glBindBuffer");

	if (glBindBuffer)
	{
		(PROC &)glDeleteBuffers        = wglGetProcAddress("glDeleteBuffers");
		(PROC &)glGenBuffers           = wglGetProcAddress("glGenBuffers");
		(PROC &)glIsBuffer             = wglGetProcAddress("glIsBuffer");
		(PROC &)glBufferData           = wglGetProcAddress("glBufferData");
		(PROC &)glBufferSubData        = wglGetProcAddress("glBufferSubData");
		(PROC &)glGetBufferSubData     = wglGetProcAddress("glGetBufferSubData");
		(PROC &)glMapBuffer            = wglGetProcAddress("glMapBuffer");
		(PROC &)glUnmapBuffer          = wglGetProcAddress("glUnmapBuffer");
		(PROC &)glGetBufferParameteriv = wglGetProcAddress("glGetBufferParameteriv");
		(PROC &)glGetBufferPointerv    = wglGetProcAddress("glGetBufferPointerv");
	}
	else
	{
		(PROC &)glBindBuffer           = wglGetProcAddress("glBindBufferARB");
		(PROC &)glDeleteBuffers        = wglGetProcAddress("glDeleteBuffersARB");
		(PROC &)glGenBuffers           = wglGetProcAddress("glGenBuffersARB");
		(PROC &)glIsBuffer             = wglGetProcAddress("glIsBufferARB");
		(PROC &)glBufferData           = wglGetProcAddress("glBufferDataARB");
		(PROC &)glBufferSubData        = wglGetProcAddress("glBufferSubDataARB");
		(PROC &)glGetBufferSubData     = wglGetProcAddress("glGetBufferSubDataARB");
		(PROC &)glMapBuffer            = wglGetProcAddress("glMapBufferARB");
		(PROC &)glUnmapBuffer          = wglGetProcAddress("glUnmapBufferARB");
		(PROC &)glGetBufferParameteriv = wglGetProcAddress("glGetBufferParameterivARB");
		(PROC &)glGetBufferPointerv    = wglGetProcAddress("glGetBufferPointervARB");
	}

	ExtSupported::VBO = glBindBuffer ? true : false;
};