#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include <map>

#include "Token.h"

#include "ElementPainter.h"
#include "Style.h"


/// <summary>
/// Base class for themes. A theme is a collection of unlimited number of styles that can draw a
/// UI element. Themes can be queried for styles. They also contain a dictionary of colors and
/// textures and other resources needed for UI drawing. These resources are used by styles to draw
/// the UI, but all of them can be overriden.
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetTheme : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetTheme);
public:
	FudgetTheme();
	~FudgetTheme();

	// Standard tokens used by controls. New styles can register new tokens if they need them, and store them
	// in any way. The same token can be retrieved from the FudgetTheme with GetToken, if the same string is
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
	/// Token for the "ButtonHoverAnimationTime" string.
	/// </summary>
	static const FudgetToken ButtonHoverAnimationTimeToken;

	// Returns a token from the string that was used to create it. The same string always returns the same
	// or equivalent token.
	static FudgetToken GetToken(String token_name);
	// Registers a new token with a string. The string can be anything, but it's case sensitive. If duplicate_is_error
	// is true, returns the FudgetToken::Invalid token.
	static FudgetToken RegisterToken(String token_name, bool duplicate_is_error = false);

	/// <summary>
	/// Tries to retrieve an element painter object for a token string.
	/// </summary>
	/// <param name="token_name">String of the token</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(String token_name) const;

	/// <summary>
	/// Tries to retrieve an element painter object for a token
	/// </summary>
	/// <param name="token">The token associated with the element painter</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(FudgetToken token) const;

	/// <summary>
	/// Tries to retrieve a style for the token. The token can be any string, but if it's a control class name
	/// the control specific token might be returned.
	/// </summary>
	/// <param name="token">Token associated with the style</param>
	/// <returns>The style if it's found or nullptr</returns>
	API_FUNCTION() FudgetStyle* GetStyle(FudgetToken token) const;

	/// <summary>
	/// Returns a style for the first matching token in the array. The tokens are usually coming from a control
	/// when it wants to get its class appropriate style. If no style is found for the tokens, the default style
	/// of the theme is returned.
	/// </summary>
	/// <param name="class_tokens">A chain of tokens that are looked up one by one until one matches a style</param>
	/// <returns>The style that matches one of the tokens or the default style of the theme</returns>
	API_FUNCTION() FudgetStyle* GetControlStyleOrDefault(const Array<FudgetToken> &class_tokens) const;

	/// <summary>
	/// The default style that is the base for all standard FudgetControl styles.
	/// </summary>
	API_PROPERTY() FudgetStyle* GetDefaultStyle() const { return _default_style; }
private:
	// Adds some hard-coded values for now.
	void Init();

	static std::map<String, FudgetToken> _token_map;
	static int _highest_token;

	std::map<FudgetToken, FudgetElementPainter*> _element_map;
	std::map<FudgetToken, FudgetStyle*> _style_map;

	// The style that is used as the base for every other style. It's possible to create styles independent
	// of the default, but the built-in controls all have styles derived from this one.
	FudgetStyle *_default_style;
};
