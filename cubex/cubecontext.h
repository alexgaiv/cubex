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

class TimeCounter
{
public:
	TimeCounter() {
		Reset();
	}
	int GetTime() {
		bool f = started && !suspended;
		return (int)(f ? totalTime + time(NULL) - lastTime : totalTime);
	}
	bool IsStarted() const {
		return started;
	}
	void Reset() {
		totalTime = lastTime = 0;
		started = suspended = false;
	}
	void Start() {
		totalTime = 0;
		lastTime = time(NULL);
		started = true;
		suspended = false;
	}
	void Stop() {
		if (!suspended)
			totalTime += time(NULL) - lastTime;
		started = false;
	}
	void Suspend() {
		if (!started) return;
		totalTime += time(NULL) - lastTime;
		suspended = true;
	}
	void Resume() {
		if (!started || !suspended) return;
		lastTime = time(NULL);
		suspended = false;
	}

	void Serialize(ofstream &os) {
		os << totalTime << ' ' << lastTime << ' ' << started << ' ' << suspended << ' ';
	}
	void Deserialize(ifstream &is) {
		is >> totalTime >> lastTime >> started >> suspended;
	}
private:
	time_t totalTime;
	time_t lastTime;
	bool started;
	bool suspended;
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
	TimeCounter timer;
	float rotAngle;
private:
	CircularStack<MoveDesc, 100> history;
	bool isSolved;
	int numMoves;
	int numActualMoves;
	bool wasScrambled;
	bool isFaceRotating;
public:
	CubeContext(GLRenderingContext *m_rc, int cubeSize);
	~CubeContext();

	void Serialize(ofstream &os);
	void Deserialize(ifstream &is);

	bool IsSolved() const { return isSolved; }
	bool CanCancelMove() const { return history.GetSize() != 0; }
	void GetScoreStr(WCHAR *buf, int bufLen);
	void Reset();
	void Scramble();
	void CancelMove();
	void OnTimer(bool &needRedraw, bool &isSolved);
	void BeginRotateFace(const MoveDesc &m);
private:
	void reset();
	bool checkSolved();
};

#endif // _CUBE_CONTEXT_H_