#include "cubeblock.h"
#include "resources.h"
#include "global.h"

Color3f CubeBlock::colors[7] =
{
	Color3f(22,125,68)/255, // green
	Color3f(208,95,3)/255, // orange
	Color3f(11,90,200)/255, // blue
	Color3f(168,30,15)/255, // red
	Color3f(209,205,201)/255, // white
	Color3f(194,181,25)/255, // yellow
	Color3f(0.09f)
};

const float CubeBlock::size = 15.0f;
bool CubeBlock::fRenderPickMode = false;

Mesh *CubeBlock::face = NULL;
Mesh *CubeBlock::edges = NULL;
Mesh *CubeBlock::face_pickMode = NULL;
Matrix44f CubeBlock::face_transform[6];

CubeBlock::CubeBlock(UINT pickId)
	: pickId(pickId), numSides(0)
{
	memset(&clr, 0, sizeof(clr));
	memset(coloredSides, 0, sizeof(coloredSides));
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
	edges = new Mesh;
	face_pickMode = new Mesh;

	face->LoadObj("models/face.obj");
	edges->LoadObj("models/edges.obj");
	face_pickMode->LoadObj("models/face_pick.obj");

	Texture2D bake_edges("textures/bake_edges.tga", GL_TEXTURE1);
	Texture2D bake_face("textures/bake_face.tga", GL_TEXTURE1);
	Texture2D tex_face("textures/face.tga", GL_TEXTURE0);
	bake_edges.SetFilters(GL_LINEAR);
	bake_face.SetFilters(GL_LINEAR);
	tex_face.SetFilters(GL_LINEAR);

	edges->BindNormalMap(bake_edges);
	face->BindNormalMap(bake_face);
	face->BindTexture(tex_face);
}	

void CubeBlock::FreeStatic()
{
	delete face;
	delete edges;
	delete face_pickMode;
}

bool CubeBlock::IsSideColored(int side)
{
	bool ret = false;
	for (int i = 0; i < numSides; i++)
		if (coloredSides[i] == side)
			return true;
	return false;
}

void CubeBlock::Render()
{
	Global::PushModelView();
	this->ApplyTransform();

	static ProgramObject *program = Global::GetCurProgram();

	/*glEnableClientState(GL_VERTEX_ARRAY);
	if (GLEW_ARB_vertex_buffer_object) {
		//(fRenderPickMode ? faces_pickMode : faces)->Bind();
		glVertexPointer(3, GL_FLOAT, 0, 0);
	}
	//else glVertexPointer(3, GL_FLOAT, 0, verts + (fRenderPickMode ? 72 : 0));
	
	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3b *c = GetSideColor(i);
			if (c) {
				glColor3ubv(c->data);
			} else glColor3f(0.0f, 0.0f, 0.0f);
		}
		else {
			int id = pickId | (1 << (i+10));
			GLubyte r = id & 0xff;
			GLubyte g = id >> 8;
			glColor3ub(r, g, 1);
		}

		glDrawArrays(GL_QUADS, i*4, 4);

		for (int i = 0; i < 6; i++)
		{
			if (!fRenderPickMode) {
				program->Uniform("Color", 1, GetSideColor(i)->data);
			}
			else {
				//int id = pickId | (1 << (i+10));
				//GLubyte r = id & 0xff;
				//GLubyte g = id >> 8;
				//glColor3ub(r, g, 1);
			}

		}
	}*/

	for (int i = 0; i < 6; i++)
	{
		if (!fRenderPickMode) {
			Color3f *c = GetSideColor(i);
			if (c == &colors[6])
				program->Uniform("UseNormalMap", 0);
			program->Uniform("NoSpecular", 1);

			program->Uniform("Color", 1, c->data);
			program->Uniform("UseTexture", 1);
			program->Uniform("lightMode", 1);
			program->Uniform("FrontMaterial.ambient", 1, Color4f(1).data);
			program->Uniform("FrontMaterial.specular", 1, Color4f(0.5f).data);
			program->Uniform("FrontMaterial.shininess", 70);

			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face->Draw();
			Global::PopModelView();
			program->Uniform("UseNormalMap", 1);
			program->Uniform("NoSpecular", 0);
		}
		else {
			int id = pickId | (1 << (i+10));
			GLubyte r = id & 0xff;
			GLubyte g = id >> 8;
			program->Uniform("Color", r/255.0f, g/255.0f, 1/255.0f);

			program->Uniform("NoLighting", 1);
			Global::PushModelView();
				Global::MultModelView(face_transform[i]);
				face_pickMode->Draw();
			Global::PopModelView();
			program->Uniform("NoLighting", 0);
		}
	}

	if (!fRenderPickMode) {
		program->Uniform("Color", 1, Color3f(0.35f).data);
		program->Uniform("UseTexture", 0);
		program->Uniform("lightMode", 0);
		program->Uniform("FrontMaterial.ambient", 1, Color4f(0.0f).data);
		program->Uniform("FrontMaterial.specular", 1, Color4f(0.8f).data);
		program->Uniform("FrontMaterial.shininess", 200);
		edges->Draw();
	}

	Global::PopModelView();
}