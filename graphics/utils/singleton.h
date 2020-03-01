#ifndef SINGLETON_H
#define SINGLETON_H

#include "pointers.h"

template<typename T> class CSingleton{
	static UniquePtr<T> singleton;
	T object;

	CSingleton() = default;
public:
	static T* get(){
		if(CSingleton<T>::singleton == nullptr)
			CSingleton<T>::singleton = NewUnique<T>(T());
		return CSingleton<T>::singleton.get();
	}
	static void release(){
		if(CSingleton<T>::singleton != nullptr)
			CSingleton<T>::singleton.reset();
		CSingleton<T>::singleton = nullptr;
	}

	T* operator->(){ return &object; }
};

template<typename T>
UniquePtr<T> CSingleton<T>::singleton = nullptr;

#endif //SINGLETON_H