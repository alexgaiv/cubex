#include "initext.h"

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

namespace initext
{
	_Ext_init_helper::_Ext_init_helper()
	{
		if (glBindBuffer) return;

		PIXELFORMATDESCRIPTOR pfd = { };
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 24;

		HDC hdc = GetDC(NULL);
		int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, iPixelFormat, &pfd);

		HGLRC hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);

		InitVBO();

		wglMakeCurrent(hdc, NULL);
		wglDeleteContext(hrc);
		ReleaseDC(NULL, hdc);
	}

	void _Ext_init_helper::InitVBO()
	{
		(PROC &)glBindBuffer           = wglGetProcAddress("glBindBuffer");
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
}