#ifndef _GL_CONTEXT_H_
#define _GL_CONTEXT_H_

#include <Windows.h>
#include <stack>
#include <list>
#include <vector>
#include "datatypes.h"
#include "shader.h"

using namespace std;

class ProgramObject;

enum GLRenderingContextFlags
{
	GLRC_DEBUG = 1,
	GLRC_FORWARD_COMPATIBLE = 2,
	GLRC_CORE_PROFILE = 4,
	GLRC_COMPATIBILITY_PROFILE = 8,
	GLRC_REQUEST_API_VERSION = 16,
	GLRC_MSAA = 32
};

struct GLRenderingContextParams
{
	DWORD glrcFlags;
	int majorVersion;
	int minorVersion;
	int layerPlane;
	int msaaNumberOfSamples;
	PIXELFORMATDESCRIPTOR *pixelFormat; // can be NULL
};

class GLRenderingContext
{
public:
	HGLRC hrc;

	GLRenderingContext(HDC hdc, const GLRenderingContextParams *params = NULL);
	~GLRenderingContext();

	void MakeCurrent() { wglMakeCurrent(_hdc, hrc); }

	ProgramObject *GetCurProgram();

	void SetModelView(const Matrix44f &mat);
	void SetProjection(const Matrix44f &mat);
	void MultModelView(const Matrix44f &mat);
	void MultProjection(const Matrix44f &mat);
	Matrix44f GetModelView() const { return modelview; }
	Matrix44f GetProjection() const { return projection; }

	void PushModelView()  { mvStack.push(modelview); }
	void PopModelView()   { SetModelView(mvStack.top()); mvStack.pop(); }
	void PushProjection() { projStack.push(projection); }
	void PopProjection()  { SetProjection(projStack.top()); projStack.pop(); }
private:
	friend class ProgramObject;
	friend class Mesh;
	friend class BaseTexture;

	HDC _hdc;
	list<ProgramObject *> shaders;
	Matrix44f modelview, projection;
	stack<Matrix44f> mvStack, projStack;

	GLuint curProgram;
	GLenum curTextureUnit;

	void AttachProgram(ProgramObject *prog) {
		shaders.push_back(prog);
	}
	void DetachProgram(ProgramObject *prog) {
		shaders.remove(prog);
	}
	
	void set_mv(const Matrix44f &mat);
	void set_proj(const Matrix44f &mat);

	HGLRC createContextAttrib(HDC hdc, const GLRenderingContextParams *params);
};

#endif // _GL_CONTEXT_H_