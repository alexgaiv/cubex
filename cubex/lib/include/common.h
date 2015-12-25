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
//#include "initext.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")

#define M_PIf 3.141592653589f
#define DEG_TO_RAD(a) ((a) * M_PIf / 180.0f)
#define RAD_TO_DEG(a) ((a) / M_PIf * 180.0f)

template<class T>
void Swap(T &a, T &b) {
	T tmp = a;
	a = b;
	b = tmp;
}

#endif // _COMMON_H_