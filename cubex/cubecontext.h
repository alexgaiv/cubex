#ifndef _CUBE_CONTEXT_H_
#define _CUBE_CONTEXT_H_

#include <time.h>
#include "common.h"
#include "resources.h"
#include "glcontext.h"
#include "quaternion.h"
#include "cube.h"

template<class T, int capacity>
class CircularStack
{
public:
	CircularStack() : size(0), cur(0) { }
	int GetSize() const { return size; }
	const T *GetData() const { return data; }
	void Clear() { size = cur = 0; }

	void Serialize(ofstream &os)
	{
		os << size << ' ' << cur << ' ';
		for (int i = 0; i < size; i++)
			os << data[i].normal << ' ' << data[i].index << ' ' << data[i].clockWise << ' ';
	}

	void Deserialize(ifstream &is)
	{
		is >> size >> cur;
		for (int i = 0; i < size && i < capacity; i++)
			is >> (int &)data[i].normal >> data[i].index >> data[i].clockWise;
	}

	void Push(T val) {
		if (cur == capacity) cur = 0;
		if (size != capacity) size++;
		data[cur++] = val;
	}
	T *Pop() {
		if (size == 0) return NULL;
		if (cur > 0) cur--;
		else cur = capacity - 1;
		size--;
		return &data[cur];
	}
private:
	T data[capacity];
	int size, cur;
};

struct MoveDesc {
		Axis normal;
		int index;
		bool clockWise;
	};

class CubeContext
{
public:
	static Quaternion qResetView;
	Cube *cube;
	Quaternion qRotation;
	bool fSolvedAnim;
	float rotAngle;
private:
	CircularStack<MoveDesc, 100> history;
	time_t solveTime;
	time_t solveTimeLast;
	bool wasScrambled;
	int numMoves;
	int numActualMoves;
	bool isFaceRotating;
	bool isScrambling;
public:
	CubeContext(GLRenderingContext *m_rc, int cubeSize);
	~CubeContext();

	void Serialize(ofstream &os);
	void Deserialize(ifstream &is);

	bool CanCancelMove() const { return history.GetSize() != 0; }
	void StartTimeCounter();
	void SuspendTimeCounter();
	void GetTimeStr(WCHAR *buf, int bufLen);
	void GetMovesStr(WCHAR *buf, int bufLen);
	void Reset();
	void Scramble();
	void CancelMove();
	void OnTimer(bool &needRedraw, bool &isSolved);
	void BeginRotateFace(const MoveDesc &m);
private:
	bool CheckSolved();
};

#endif // _CUBE_CONTEXT_H_