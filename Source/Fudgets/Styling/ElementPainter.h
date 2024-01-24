#pragma once

#include "Engine/Scripting/ScriptingObject.h"

class FudgetControl;

/// <summary>
/// Base type for classes that can draw elements or a full control. Derived classes can draw something simple
/// like a line of text or the background of a button, or can draw every part of a complex control.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetElementPainter : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetElementPainter);
public:
	FudgetElementPainter();
};

