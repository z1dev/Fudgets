#pragma once

#include <memory>
#include "Engine/Core/Object.h"

template<class T, class MemoryAllocator = Allocator>
struct FlaxObjectDeleter
{
	void operator()(T*obj) { Delete<T, MemoryAllocator>(obj); }
};

template<class T, class MemoryAllocator = Allocator>
class UniquePtr : public std::unique_ptr<T, FlaxObjectDeleter<T, MemoryAllocator>>
{
public:
	operator T*() { return get(); }
};


template<class T, class MemoryAllocator = Allocator>
class SharedPtr : public std::shared_ptr<T>
{
	using Base = std::shared_ptr<T>;
public:
	operator T*() { return get(); }
	
	template<class Y>
	void reset(Y *ptr)
	{
		Base::reset(ptr, FlaxObjectDeleter<T, MemoryAllocator>);
	}
};
