#pragma once

#include "Engine/Scripting/ScriptingObject.h"

API_STRUCT()
struct FUDGETS_API FudgetSlotPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSlotPadding)

	FudgetSlotPadding() : left(0.0f), right(0.0f), top(0.0f), bottom(0.0f)
	{
	}


	float left;
	float right;
	float top;
	float bottom;
};