#ifndef POINTERS_H
#define POINTERS_H

#include <memory>
#define SharedPtr std::shared_ptr
#define WeakPtr std::weak_ptr
#define UniquePtr std::unique_ptr

#define NewUnique std::make_unique
#define NewShared std::make_shared

#include "types/types.h"

struct SRawData{
	byte* data = nullptr;
	uint32 size = 0;
};

#endif POINTERS_H