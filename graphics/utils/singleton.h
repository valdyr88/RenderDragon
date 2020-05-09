#ifndef SINGLETON_H
#define SINGLETON_H

#include "log.h"
#include "pointers.h"
#include "threads.h"

//#define CSingleton_mutex

template<typename T> class CSingleton{
	#ifdef CSingleton_mutex
		static CMutex mtx;
	#endif
protected:
	static UniquePtr<T> singleton;

	CSingleton() = default;
public:
	static T* get(){
		#ifdef CSingleton_mutex
			if(mtx.isLocked() == true){
				LOG_ERR("singleton ptr not returned before call to get()");
				return nullptr;
			}
			mtx.lock();
		#endif
		if(CSingleton<T>::singleton == nullptr)
			CSingleton<T>::singleton = NewUnique<T>(T());
		return CSingleton<T>::singleton.get();
	}

	//ToDo: napravit variadic temp za parametre, ali razlièiti tipovi parametara
	template<typename paramType>
	static T* get(paramType* p){
		if(CSingleton<T>::singleton == nullptr)
			CSingleton<T>::singleton = NewUnique<T>(T(p));
		return CSingleton<T>::singleton.get();
	}

	static void ret(T** ptr){
		if(*ptr != CSingleton<T>::singleton.get()){
			LOG_ERR("wrong pointer passed into ret()");
			return;
		}
		#ifdef CSingleton_mutex
			mtx.unlock();
		#endif
		(*ptr) = nullptr;
	}

	static void Release(){
		if(CSingleton<T>::singleton != nullptr)
			CSingleton<T>::singleton.reset();
		CSingleton<T>::singleton = nullptr;
	}
};

#ifdef CSingleton_mutex
	template<typename T>
	CMutex CSingleton<T>::mtx;
#endif

template<typename T>
UniquePtr<T> CSingleton<T>::singleton = nullptr;


#endif //SINGLETON_H