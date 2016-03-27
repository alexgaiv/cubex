#ifndef _COMMON_H_
#define _COMMON_H_

#define _USE_MATH_DEFINES

#include <Windows.h>
#include <math.h>
#include "gl\glew.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#include "gl\glext.h"
#include "gl\wglext.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

#define M_PIf 3.141592653589f
#define DEG_TO_RAD(a) ((a) * M_PIf / 180.0f)
#define RAD_TO_DEG(a) ((a) / M_PIf * 180.0f)

inline BYTE *GetBinaryResource(LPCTSTR name)
{
	HRSRC hRes = FindResource(NULL, name, RT_RCDATA);
	return (BYTE *)LockResource(LoadResource(NULL, hRes));
}

inline void GetTextResource(LPCTSTR name, char *&str, int &length)
{
	HRSRC hRes = FindResource(NULL, name, RT_RCDATA);
	str = (char *)LockResource(LoadResource(NULL, hRes));
	length = SizeofResource(NULL, hRes);
}

#endif // _COMMON_H_