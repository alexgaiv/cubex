#include "cubecontext.h"
#include <strsafe.h>

Quaternion CubeContext::qResetView =
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 30.0f) *
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), -45.0f);

CubeContext::CubeContext(GLRenderingContext *m_rc, int cubeSize)
{
	cube = new Cube(m_rc, cubeSize);
	solveTime = solveTimeLast = 0;
	fSolvedAnim = false;
	rotAngle = 0.0f;
	wasMixed = false;
	numMoves = numActualMoves = 0;
	isFaceRotating = isMixing = false;
	qRotation = qResetView;
}

CubeContext::~CubeContext() {
	delete cube;
}

void CubeContext::Serialize(ofstream &os)
{
	os.write((char *)&qRotation, sizeof(qRotation));
	os << qRotation.x << ' ' <<
		qRotation.y << ' ' <<
		qRotation.z << ' ' <<
		qRotation.w << ' ' <<
		fSolvedAnim << ' ' <<
		rotAngle << ' ' <<
		solveTime << ' ' <<
		solveTimeLast << ' ' <<
		wasMixed << ' ' <<
		numMoves << ' ' <<
		numActualMoves << ' ' <<
		isFaceRotating << ' ' <<
		isMixing << ' ';

	history.Serialize(os);
	cube->Serialize(os);
}

void CubeContext::Deserialize(ifstream &is)
{
	is >> qRotation.x >>
		qRotation.y >>
		qRotation.z >>
		qRotation.w >>
		fSolvedAnim >>
		rotAngle >>
		solveTime >>
		solveTimeLast >>
		wasMixed >>
		numMoves >>
		numActualMoves >>
		isFaceRotating >>
		isMixing;

	history.Deserialize(is);
	cube->Deserialize(is);
}

void CubeContext::StartTimeCounter()
{
	if (!fSolvedAnim)
		solveTimeLast = time(NULL);
}

void CubeContext::SuspendTimeCounter()
{
	if (!fSolvedAnim)
		solveTime += time(NULL) - solveTimeLast;
}

void CubeContext::GetTimeStr(WCHAR *buf, int bufLen)
{
	WCHAR time_str[30] = { };
	LoadStringW(NULL, IDS_TIME, time_str, 30);

	int mins = (int)solveTime / 60;
	int secs = (int)(mins ? solveTime % mins : solveTime);

	StringCchPrintfW(buf, bufLen, time_str, mins, secs);
}

void CubeContext::GetMovesStr(WCHAR *buf, int bufLen)
{
	WCHAR moves_str[30] = { };
	LoadStringW(NULL, IDS_MOVES, moves_str, 30);
	StringCchPrintfW(buf, bufLen, moves_str, numMoves);
}

void CubeContext::Reset()
{
	solveTime = 0;
	solveTimeLast = time(NULL);
	fSolvedAnim = false;
	wasMixed = false;
	numMoves = numActualMoves = 0;
	isMixing = false;
	cube->Reset();
	history.Clear();
}

void CubeContext::MixUp()
{
	fSolvedAnim = false;
	wasMixed = true;
	numMoves = numActualMoves = 0;
	isMixing = true;
	cube->MixUp(cube->size < 7 ? 15 : 20);
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
	

void CubeContext::OnTimer(bool &needRedraw, bool &isSolved)
{
	needRedraw = isSolved = false;

	if (fSolvedAnim) {
		rotAngle += 3.0f;
		if (rotAngle > 360.0f) rotAngle -= 360.0f;
		needRedraw = true;
		return;
	}

	bool fCubeAnim = cube->AnimationStep();

	if (isFaceRotating && !fCubeAnim) {
		if (CheckSolved())
		{
			solveTime += time(NULL) - solveTimeLast;
			fSolvedAnim = true;
			rotAngle = 0.0f;
			isSolved = true;
		}
		isFaceRotating = false;
	}
	if (isMixing && !fCubeAnim) {
		solveTime = 0;
		solveTimeLast = time(NULL);
		isMixing = false;
	}

	needRedraw = fCubeAnim;
}

void CubeContext::BeginRotateFace(const MoveDesc &m)
{
	cube->BeginRotateFace(m.normal, m.index, m.clockWise);
	history.Push(m);
	isFaceRotating = true;
	numMoves++;
	numActualMoves++;
}

bool CubeContext::CheckSolved()
{
	if (cube->IsSolved()) {
		bool ret = wasMixed || numActualMoves >= cube->GOD_NUMBER;
		wasMixed = false;
		numActualMoves = 0;
		return ret;
	}
	return false;
}