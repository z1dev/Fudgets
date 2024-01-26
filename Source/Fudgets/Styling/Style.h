#pragma once

#include "Engine/Scripting/ScriptingObject.h"

#include "Token.h"

class FudgetThemeBase;

/// <summary>
/// Class for style elements. A style element contains the settings used for drawing controls and control elements,
/// like colors, paddings, line width, fonts. Unless this is the main style for the theme, styles inherit every
/// setting from their parent style. They can also add new settings and override those in the parents.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetStyle : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetStyle);
public:
	FudgetStyle(FudgetThemeBase *theme, FudgetToken token);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	API_PROPERTY() String GetName() const { return _name; }

	/// <summary>
	/// 
	/// </summary>
	/// <param name="value"></param>
	API_PROPERTY() void SetName(String value) { _name = value; }

private:
	FudgetThemeBase *_theme;
	FudgetToken _token;
	String _name;
};
