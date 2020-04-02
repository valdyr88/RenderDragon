#ifndef THREADS_H
#define THREADS_H

#include <mutex>
#include <thread>

class CMutex{
	std::mutex mtx;
	bool locked = false;
public:
	void lock(){
		mtx.lock(); locked = true; }
	void unlock(){
		locked = false; mtx.unlock(); }

	bool isLocked(){ return locked; }
};

class CSafeMutex : public CMutex{
public:
	void lock(){
		if(isLocked() == true) return;
		CMutex::lock();
	}
	void unlock(){
		if(isLocked() == false) return;
		CMutex::unlock();
	}
};

class CScopedUnlock{
	CMutex& mtx;
	CScopedUnlock() = delete;
public:
	CScopedUnlock(CMutex& m) : mtx(m) {}
	~CScopedUnlock(){
		if(mtx.isLocked() == true)
			mtx.unlock();
	}
};

#endif