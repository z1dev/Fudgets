#pragma once

#include "Engine/Scripting/ScriptingObject.h"

/// <summary>
/// Padding inside any element that contains others, like a slot in a layout or a drawn border in a style.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetPadding
{
	DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetPadding)

	FudgetPadding() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f)
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