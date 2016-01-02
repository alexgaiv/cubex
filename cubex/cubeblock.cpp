#include "cubeblock.h"
#include "resources.h"
#include "global.h"

Color3f CubeBlock::colors[6] =
{
	Color3f(22,125,68)/255, // green
	Color3f(208,95,3)/255, // orange
	Color3f(11,90,200)/255, // blue
	Color3f(168,30,15)/255, // red
	Color3f(209,205,201)/255, // white
	Color3f(194,181,25)/255, // yellow
};

Color3f CubeBlock::borderDiffuse;
Color3f CubeBlock::borderAmbient;

const float CubeBlock::size = 15.0f;
bool CubeBlock::fRenderPickMode = false;
bool CubeBlock::fUseReducedModel = false;

Mesh *CubeBlock::face = NULL;
Mesh *CubeBlock::border = NULL;
Mesh *CubeBlock::border_reduced = NULL;
Mesh *CubeBlock::face_pickMode = NULL;
Matrix44f CubeBlock::face_transform[6];

CubeBlock::CubeBlock(UINT pickId)
	: pickId(pickId), numSides(0)
{
	memset(&clr, 0, sizeof(clr));
	memset(coloredSides, 0, sizeof(coloredSides));
}

void CubeBlock::DrawWhiteBorders(bool whiteBorders)
{
	if (whiteBorders) {
		borderAmbient = Color3f(0.8f);
		borderDiffuse = Color3f(0.85f);
	}
	else {
		borderAmbient = Color3f(0.0f);
		borderDiffuse = Color3f(0.35f);
	}
	ProgramObject *p = Global::GetCurProgram();
	p->Uniform("BorderMaterial.ambient", 1, borderAmbient.data);
	p->Uniform("BorderMaterial.diffuse", 1, borderDiffuse.data);
}

void CubeBlock::InitStatic()
{
	Quaternion qs[6] =
	{
		Quaternion(Vector3f(0,1,0), -90),
		Quaternion(Vector3f(0,1,0), 180),
		Quaternion(Vector3f(0,1,0), 90),
		Quaternion::Identity(),
		Quaternion(Vector3f(1,0,0), -90),
		Quaternion(Vector3f(1,0,0), 90),
	};

	for (int i = 0; i < 6; i++)
		qs[i].ToMatrix(face_transform[i]);

	face = new Mesh;
	border = new Mesh;
	border_reduced = new Mesh;
	face_pickMode = new Mesh;

	face->LoadRaw("face.raw");
	border->LoadRaw("border.raw");
	border_reduced->LoadRaw("border_reduced.raw");
	face_pickMode->LoadRaw("face_pick.raw");

	if (!GLEW_ARB_shader_objects) return;

	Texture2D face_mask("face_mask.tga", GL_TEXTURE0);
	Texture2D face_normal("face_normal.tga", GL_TEXTURE1);
	Texture2D border_normal("border_normal.tga", GL_TEXTURE1);
	Texture2D border_reduced_normal("border_reduced_normal.tga", GL_TEXTURE1);
	Texture2D border_specular("border_specular.tga", GL_TEXTURE2);

	face_mask.SetFilters(GL_LINEAR);
	face_normal.SetFilters(GL_LINEAR);
	border_normal.SetFilters(GL_LINEAR);
	border_reduced_normal.SetFilters(GL_LINEAR);
	border_specular.SetFilters(GL_LINEAR);

	face->BindTexture(face_mask);
	face->BindNormalMap(face_normal);
	border->BindNormalMap(border_normal);
	border->BindSpecularMap(border_specular);
	border_reduced->BindNormalMap(border_reduced_normal);

	DrawWhiteBorders(false);
}	

void CubeBlock::FreeStatic()
{
	delete face;
	delete face_pickMode;
	delete border;
	delete border_reduced;
}

bool CubeBlock::IsSideColored(int side)
{
	bool ret = false;
	for (int i = 0; i < numSides; i++)
		if (coloredSides[i] == side)
			return true;
	return false;
}

void CubeBlock::RenderFixed()
{
	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3f *c = GetSideColor(i);
			if (c == &borderDiffuse)
				glColor3f(0, 0, 0);
			else glColor3fv((*c * 1.2f).data);

			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face->DrawFixed();
			Global::PopModelView();
		}
		else {
			int id = pickId | (1 << (i+10));
			GLubyte r = id & 0xff;
			GLubyte g = id >> 8;

			glColor3ub(r, g, 1);
			
			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face_pickMode->DrawFixed();
			Global::PopModelView();
		}
	}

	if (!fRenderPickMode) {
		glColor3f(0, 0, 0);
		(fUseReducedModel ? border_reduced : border)->DrawFixed();
	}
}

void CubeBlock::Render()
{
	Global::PushModelView();
	this->ApplyTransform();

	if (!GLEW_ARB_shader_objects) {
		RenderFixed();
		Global::PopModelView();
		return;
	}

	static ProgramObject *program = Global::GetCurProgram();

	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3f *c = GetSideColor(i);
			if (c == &borderDiffuse)
				program->Uniform("RenderBackSide", 1);

			program->Uniform("Mode", 0);
			program->Uniform("FrontMaterial.ambient", 1, Color3f(1).data);
			program->Uniform("FrontMaterial.diffuse", 1, c->data);
			program->Uniform("FrontMaterial.shininess", 70);

			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face->Draw();
			Global::PopModelView();

			if (c == &borderDiffuse)
				program->Uniform("RenderBackSide", 0);
		}
		else {
			int id = pickId | (1 << (i+10));
			GLubyte r = id & 0xff;
			GLubyte g = id >> 8;

			program->Uniform("Mode", 3);
			program->Uniform("FrontMaterial.diffuse", r/255.0f, g/255.0f, 1/255.0f);
			
			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face_pickMode->Draw();
			Global::PopModelView();
		}
	}

	if (!fRenderPickMode)
	{
		program->Uniform("Mode", 1);
		program->Uniform("FrontMaterial.ambient", 1, borderAmbient.data);
		program->Uniform("FrontMaterial.diffuse", 1, borderDiffuse.data);

		if (fUseReducedModel) {
			program->Uniform("UseSpecularMap", 0);
			program->Uniform("FrontMaterial.shininess", 70);
			border_reduced->Draw();
		}
		else {
			program->Uniform("UseSpecularMap", 1);
			program->Uniform("FrontMaterial.shininess", 200);
			border->Draw();
		}
	}

	Global::PopModelView();
}