#pragma once

#include "Engine/Core/Collections/Array.h"

// Moves an item at index from to index to in the array arr. Only use for fast copiable items, like pointers.
template<typename T>
void MoveInArray(Array<T> arr, int from, int to)
{
	auto item = arr[from];
	if (from > to)
	{
		for (int ix = from; ix > to; --ix)
		{
			arr[from] = arr[from - 1];
		}
		arr[to] = item;
	}
	else
	{
		for (int ix = from; ix < to; ++ix)
		{
			arr[from] = arr[from + 1];
		}
		arr[to] = item;
	}
}

extern const float MaximumFloatLimit;

float AddBigFloats(float a, float b);
