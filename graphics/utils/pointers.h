#ifndef POINTERS_H
#define POINTERS_H

#include <memory>
#define SharedPtr std::shared_ptr
#define WeakPtr std::weak_ptr
#define UniquePtr std::unique_ptr

#define NewUnique std::make_unique
#define NewShared std::make_shared

#define __new new
#define __release_ptr(x) if(x != 0){ delete x; x = nullptr; }
#define __release_array(x) if(x != 0){ delete[] x; x = nullptr; }

#include "types/types.h"

struct SRawData{
	byte* data = nullptr;
	uint32 size = 0;
};

#define fill_memory(memory,size,value) { for(uint64 i = 0; i < size; ++i){ ((byte*)((void*)memory))[i] = value; } }
#define zero_memory(memory,size) fill_memory(memory,size,0)
#define copy_memory(dest,size,src){ for(uint64 i = 0; i < size; ++i){ ((byte*)((void*)dest))[i] = ((byte*)((void*)src))[i]; }  }
#define copy_memory_type(dest,size,src,type){ uint64 sz = (uint64)(size/sizeof(type)); for(uint64 i = 0; i < sz; ++i){ ((type*)((void*)dest))[i] = ((type*)((void*)src))[i]; } }

#define sizeof_array(Array, Type) (sizeof(Array) / sizeof(Type))

#endif POINTERS_H