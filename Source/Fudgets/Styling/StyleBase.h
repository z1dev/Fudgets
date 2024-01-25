#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include "Token.h"

class FudgetThemeBase;

/// <summary>
/// Base class for style elements. A style element can draw a specific part of the UI, like a button text, background
/// for a textbox or other part. Styles are specialized. For example a button would draw itself by calling to draw a
/// background from a background style, a border around itself from a border drawing style and text or icon from
/// the corresponding styles.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetStyleBase : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetStyleBase);
public:
	FudgetStyleBase(FudgetThemeBase *theme, FudgetToken token);
private:
	FudgetThemeBase *_theme;
	FudgetToken _token;
};
