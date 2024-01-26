#pragma once

#include <map>
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Collections/Array.h"

#include "Token.h"

class FudgetTheme;

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
	FudgetStyle(FudgetTheme *theme);
	~FudgetStyle();

	/// <summary>
	/// Creates a new style that inherits all its values from this one
	/// </summary>
	/// <returns></returns>
	API_FUNCTION() FudgetStyle* CreateInheritedStyle();

	/// <summary>
	/// Returns a color set in this style either directly or by inheriting from a parent style.
	/// </summary>
	/// <param name="token">Token associated with the color</param>
	/// <returns>Color for the token</returns>
	API_FUNCTION() Color GetColor(FudgetToken token) const;

	/// <summary>
	/// Changes or sets the color for a token. Inherited styles are notified of the change, and if they don't have an
	/// override for the color, they will use this one.
	/// </summary>
	/// <param name="token">The token associated with the color</param>
	/// <param name="value">The new color value</param>
	API_FUNCTION() void SetColorOverride(FudgetToken token, Color value);

	/// <summary>
	/// Resets the color from the override value to the original. It does nothing if the value was not overriden. Removes
	/// this color if it didn't override anything.
	/// </summary>
	/// <param name="token">The token associated with the color</param>
	API_FUNCTION() void ResetColor(FudgetToken token);

	/// <summary>
	/// Returns a float value set in this style either directly or by inheriting from a parent style.
	/// </summary>
	/// <param name="token">Token associated with the float</param>
	/// <returns>Float for the token</returns>
	API_FUNCTION() float GetFloat(FudgetToken token) const;

	/// <summary>
	/// Changes or sets the float value for a token. Inherited styles are notified of the change, and if they don't have an
	/// override for the value, they will use this one.
	/// </summary>
	/// <param name="token">The token associated with the float</param>
	/// <param name="value">The new float value</param>
	API_FUNCTION() void SetFloatOverride(FudgetToken token, float value);

	/// <summary>
	/// Resets the float from the override value to the original. It does nothing if the value was not overriden. Removes
	/// this float value if it didn't override anything.
	/// </summary>
	/// <param name="token">The token associated with the color</param>
	API_FUNCTION() void ResetFloat(FudgetToken token);

protected:
	void ParentColorChanged(FudgetToken token);
	void ParentFloatChanged(FudgetToken token);
private:
	void NotifyDerivedColorChanged(FudgetToken token);
	void NotifyDerivedFloatChanged(FudgetToken token);

	FudgetTheme* _theme;

	FudgetStyle *_parent;
	Array<FudgetStyle*> _derived;

	std::map<FudgetToken, Color> _color_map;
	std::map<FudgetToken, float> _float_map;

	std::map<FudgetToken, Color> _color_map_overrides;
	std::map<FudgetToken, float> _float_map_overrides;
};
