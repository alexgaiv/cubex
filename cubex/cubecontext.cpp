#include "cubecontext.h"
#include <strsafe.h>

Quaternion CubeContext::qResetView =
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 30.0f) *
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), -45.0f);

CubeContext::CubeContext(GLRenderingContext *m_rc, int cubeSize)
{
	cube = new Cube(m_rc, cubeSize);
	qRotation = qResetView;
	reset();
}

CubeContext::~CubeContext() {
	delete cube;
}

void CubeContext::reset()
{
	timer.Reset();
	rotAngle = 0.0f;
	isSolved = false;
	numMoves = numActualMoves = 0;
	wasScrambled = false;
	isFaceRotating = false;
}

void CubeContext::Serialize(ofstream &os)
{
	os << qRotation.x << ' ' <<
		qRotation.y << ' ' <<
		qRotation.z << ' ' <<
		qRotation.w << ' ' <<
		isSolved << ' ' <<
		numMoves << ' ' <<
		numActualMoves << ' ' <<
		wasScrambled << ' ' <<
		isFaceRotating << ' ';

	cube->Serialize(os);
	timer.Serialize(os);
	history.Serialize(os);
}

void CubeContext::Deserialize(ifstream &is)
{
	is >> qRotation.x >>
		qRotation.y >>
		qRotation.z >>
		qRotation.w >>
		isSolved >>
		numMoves >>
		numActualMoves >>
		wasScrambled >>
		isFaceRotating;

	cube->Deserialize(is);
	timer.Deserialize(is);
	history.Deserialize(is);
}

void CubeContext::GetScoreStr(WCHAR *buf, int bufLen)
{
	WCHAR format[50] = { };
	LoadStringW(NULL, IDS_SCORE, format, 50);

	int t = timer.GetTime();
	StringCchPrintfW(buf, bufLen, format, numMoves, t / 60, t % 60);
}

void CubeContext::Reset() {
	reset();
	cube->Reset();
	history.Clear();
}

void CubeContext::Scramble() {
	reset();
	wasScrambled = true;
	cube->Scramble(cube->size < 7 ? 15 : 20);
	history.Clear();
}

void CubeContext::CancelMove()
{
	if (!cube->IsAnim()) {
		MoveDesc *m = history.Pop();
		if (m) {
			cube->BeginRotateFace(m->normal, m->index, !m->clockWise);
			numMoves--;
			if (numActualMoves > 0) numActualMoves--;
		}
	}
}

void CubeContext::OnTimer(bool &needRedraw, bool &solved)
{
	needRedraw = solved = false;

	if (isSolved) {
		rotAngle += 3.0f;
		if (rotAngle > 360.0f) rotAngle -= 360.0f;
		needRedraw = true;
	}
	else {
		bool fCubeAnim = cube->AnimationStep();
		if (isFaceRotating && !fCubeAnim) {
			if (checkSolved()) {
				timer.Stop();
				isSolved = true;
				solved = true;
			}
			isFaceRotating = false;
		}
		needRedraw = fCubeAnim;
	}
}

void CubeContext::BeginRotateFace(const MoveDesc &m)
{
	if (!timer.IsStarted()) timer.Start();

	cube->BeginRotateFace(m.normal, m.index, m.clockWise);
	history.Push(m);
	isFaceRotating = true;
	numMoves++;
	numActualMoves++;
}

bool CubeContext::checkSolved()
{
	if (cube->IsSolved()) {
		bool ret = wasScrambled || numActualMoves >= cube->GOD_NUMBER;
		wasScrambled = false;
		numActualMoves = 0;
		return ret;
	}
	return false;
}