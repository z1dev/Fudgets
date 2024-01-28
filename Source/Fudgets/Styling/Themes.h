#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Core/Collections/Dictionary.h"

#include "Token.h"

#include "ElementPainter.h"
#include "Style.h"

#include <map>


/// <summary>
/// A collection of values used by styles to provide them to element painters for drawing. This collection
/// represents one theme.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetTheme : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetTheme)
public:
	FudgetTheme();

	/// <summary>
	/// The values in this theme, that can be referenced by styles. Although it's not read-only, the values
	/// shouldn't be changed directly, because styles cache the values and they won't be refreshed.
	/// </summary>
	API_FIELD() Dictionary<FudgetToken, Variant> _resources;

	/// <summary>
	/// Element painter tokens that can be set for controls based on their name. The tokens are used to retrieve
	/// the painter from FudgetThemes::GetElementPainter
	/// </summary>
	API_FIELD() Dictionary<FudgetToken, FudgetToken> _painter_ids;
};


/// <summary>
/// Base class for themes. A theme is a collection of unlimited number of styles that can draw a
/// UI element. Themes can be queried for styles. They also contain a dictionary of colors and
/// textures and other resources needed for UI drawing. These resources are used by styles to draw
/// the UI, but all of them can be overriden.
/// </summary>
API_CLASS(Static)
class FUDGETS_API FudgetThemes
{
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetThemes);
public:
	//FudgetThemes();
	//~FudgetThemes();

	API_FUNCTION() static void Initialize();
	API_FUNCTION() static void Uninitialize();

	// Token for the base theme that's guaranteed to be present.
	static const FudgetToken MainThemeToken;

	// Token to get the default style that holds basic values.
	static const FudgetToken DefaultStyleToken;

	// The static tokens below are probably all temporary here and will be removed:

	// Standard tokens used by controls. New styles can register new tokens if they need them, and store them
	// in any way. The same token can be retrieved from the FudgetThemes with GetToken, if the same string is
	// passed to it.

	// Tokens for getting paint properties:

	/// <summary>
	/// Token for the "ButtonDownToken" string.
	/// </summary>
	static const FudgetToken ButtonDownToken;
	/// <summary>
	/// Token for the "ButtonPressedToken" string.
	/// </summary>
	static const FudgetToken LeftButtonPressedToken;
	/// <summary>
	/// Token for the "ButtonMouseOverToken" string.
	/// </summary>
	static const FudgetToken MouseHoverToken;

	// Tokens of standard values that can be used to find style settings

	/// <summary>
	/// Token for the "ColorDark" string.
	/// </summary>
	static const FudgetToken ColorDarkToken;
	/// <summary>
	/// Token for the "ColorLight" string.
	/// </summary>
	static const FudgetToken ColorLightToken;
	/// <summary>
	/// Token for the "ColorNormal" string.
	/// </summary>
	static const FudgetToken ColorNormalToken;
	/// <summary>
	/// Token for the "HoverAnimationTime" string.
	/// </summary>
	static const FudgetToken HoverAnimationTimeToken;
	/// <summary>
	/// Token for "ColorAccent" string.
	/// </summary>
	static const FudgetToken ColorAccentToken;
	/// <summary>
	/// Token for "FocusRectangleWidth" string.
	/// </summary>
	static const FudgetToken FocusRectangleWidthToken;


	static const FudgetToken ButtonBackgroundNormalToken;
	static const FudgetToken ButtonBackgroundPressedToken;
	static const FudgetToken ButtonBackgroundHoverToken;
	static const FudgetToken ButtonHoverAnimationTimeToken;
	static const FudgetToken ButtonFocusRectangleColorToken;
	static const FudgetToken ButtonFocusRectangleWidthToken;

	static const FudgetToken ButtonBackgroundPainterToken;



	// Returns a token from the string that was used to create it. The same string always returns the same
	// or equivalent token.
	API_FUNCTION() static FudgetToken GetToken(String token_name);
	// Registers a new token with a string. The string can be anything, but it's case sensitive. If duplicate_is_error
	// is true, returns the FudgetToken::Invalid token.
	API_FUNCTION() static FudgetToken RegisterToken(String token_name, bool duplicate_is_error = false);

	/// <summary>
	/// Tries to retrieve an element painter object for a token string.
	/// </summary>
	/// <param name="token_name">String of the token</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() static FudgetElementPainter* GetElementPainter(String token_name);

	/// <summary>
	/// Tries to retrieve an element painter object for a token
	/// </summary>
	/// <param name="token">Token associated with the element painter</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() static FudgetElementPainter* GetElementPainter(FudgetToken token);

	/// <summary>
	/// Tries to retrieve a theme for the token or null if token is not found.
	/// </summary>
	/// <param name="token">Token associated with the theme</param>
	/// <returns>The theme if found, or null</returns>
	API_FUNCTION() static FudgetTheme* GetTheme(FudgetToken token);

	/// <summary>
	/// Tries to retrieve a style for the token. The token can correspond to any string, but if it's a control
	/// class name, the control specific token might be returned.
	/// </summary>
	/// <param name="token">Token associated with the style</param>
	/// <returns>The style if found or null</returns>
	API_FUNCTION() static FudgetStyle* GetStyle(FudgetToken token);

	/// <summary>
	/// Returns a style for the first matching token in the array. The tokens are usually coming from a control
	/// when it wants to get its class appropriate style. If no style is found for the tokens, the default style
	/// of the theme is returned.
	/// </summary>
	/// <param name="class_tokens">A chain of tokens that are looked up one by one until one matches a style</param>
	/// <returns>The style that matches one of the tokens or the default style of the theme</returns>
	API_FUNCTION() static FudgetStyle* GetControlStyleOrDefault(const Array<FudgetToken> &class_tokens);

	/// <summary>
	/// Looks up a value from the theme and sets the result to it.
	/// </summary>
	/// <param name="theme_token">Theme that should have the value</param>
	/// <param name="value_token">Token associated with the value</param>
	/// <param name="result">The result will be stored here</param>
	/// <returns>True if the value was found in the theme, false if not</returns>
	API_FUNCTION() static bool GetValueFromTheme(FudgetToken theme_token, FudgetToken value_token, API_PARAM(Out) Variant &result);

private:
	static void RegisterStyle(FudgetToken token, FudgetStyle *style);
	static void UnregisterStyle(FudgetToken token, FudgetStyle *style);

	static std::map<String, FudgetToken> _token_map;
	static int _highest_token;

	// A collection of painters that can be used to draw a part of a control or a full control.
	static std::map<FudgetToken, FudgetElementPainter*> _element_map;

	// A collection of styles that provide values, like colors or floats to an element painter.
	static std::map<FudgetToken, FudgetStyle*> _style_map;

	// A collection of themes. Each theme is a collection of data in itself to provide values that can be used by
	// styles and element painters.
	static std::map<FudgetToken, FudgetTheme*> _theme_map;

	static bool _themes_initialized;

	friend class FudgetStyle;
};
