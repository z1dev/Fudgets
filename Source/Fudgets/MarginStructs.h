#pragma once

#include "Engine/Scripting/ScriptingObject.h"

/// <summary>
/// Padding around a control, that is considered as an extension of the control when calculating layouts.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetSlotPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetSlotPadding)

	FudgetSlotPadding() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f)
	{
	}

	/// <summary>
	/// Padding to the left
	/// </summary>
	API_FIELD() float Left;
	/// <summary>
	/// Padding to the right
	/// </summary>
	API_FIELD() float Right;
	/// <summary>
	/// Padding to the top
	/// </summary>
	API_FIELD() float Top;
	/// <summary>
	/// Padding to the bottom
	/// </summary>
	API_FIELD() float Bottom;
};