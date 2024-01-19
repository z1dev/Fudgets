#pragma once

#include "Engine/Scripting/ScriptingObject.h"

API_STRUCT()
struct FUDGETS_API FudgetSlotPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSlotPadding)

	FudgetSlotPadding() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f)
	{
	}

	/// <summary>
	/// Padding to the left
	/// </summary>
	API_FIELD() float left;
	/// <summary>
	/// Padding to the right
	/// </summary>
	API_FIELD() float right;
	/// <summary>
	/// Padding to the top
	/// </summary>
	API_FIELD() float top;
	/// <summary>
	/// Padding to the bottom
	/// </summary>
	API_FIELD() float bottom;
};