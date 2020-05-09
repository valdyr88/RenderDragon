#ifndef POINTERS_H
#define POINTERS_H

#include <memory>
#include <vector>
#define SharedPtr std::shared_ptr
#define WeakPtr std::weak_ptr
#define UniquePtr std::unique_ptr

#define NewUnique std::make_unique
#define NewShared std::make_shared

#define __new new
#define __release_ptr(x) if((x) != nullptr){ delete (x); (x) = nullptr; }
#define __release_array(x) if((x) != nullptr){ delete[] (x); (x) = nullptr; }

#include "types/types.h"

struct SRawData{
	byte* data = nullptr;
	uint32 size = 0;

	SRawData() : data(nullptr), size(0){}
	SRawData(void* ptr, uint32 s) : data((byte*)ptr), size(s){}
	template<typename type>
	SRawData(std::vector<type>& darray){
		data = (byte*)darray.data();
		size = (uint32)(sizeof(type)*darray.size());
	}
};

#define fill_memory(memory,size,value) { for(uint64 i = 0; i < size; ++i){ ((byte*)((void*)memory))[i] = value; } }
#define zero_memory(memory,size) fill_memory(memory,size,0)
#define copy_memory(dest,size,src){ for(uint64 i = 0; i < size; ++i){ ((byte*)((void*)dest))[i] = ((byte*)((void*)src))[i]; }  }
#define copy_memory_type(dest,size,src,type){ uint64 sz = (uint64)(size/sizeof(type)); for(uint64 i = 0; i < sz; ++i){ ((type*)((void*)dest))[i] = ((type*)((void*)src))[i]; } }

#define sizeof_array(Array, Type) (sizeof(Array) / sizeof(Type))

namespace stdex
{
	template <class _Ty>
	class weak_ptrex : public std::weak_ptr<_Ty>{
		
	public:
		//Warning! this function is unsafe in case the object this weak_ptr points to is released at the same time this function is called
		bool contains(_Ty* ptr) const{
			return this->get() == ptr; }
	};
}
#define WeakPtrEx stdex::weak_ptrex

#endif POINTERS_H