#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include <map>

#include "Token.h"


//class FudgetStyleBase;
class FudgetElementPainter;

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

	// Standard tokens used by controls. New styles can register new tokens if they need them, and
	// store them in any way. The same token can be retreived from the FudgetTheme with GetToken, if
	// the same string is passed to it.

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



	//template<typename T>
	//T* CreateStyle(String token_name)
	//{
	//	FudgetToken token = RegisterToken(token_name);
	//	if (token == FudgetToken::Invalid)
	//		return nullptr;

	//	T *style = New<T>(this, token);
	//	style_map[token] = style;

	//	return style;
	//}

	static FudgetToken GetToken(String token_name);
	static FudgetToken RegisterToken(String token_name, bool duplicate_is_error = false);

	/// <summary>
	/// Tries to retreive an element painter object for a token string.
	/// </summary>
	/// <param name="token_name">String of the token</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(String token_name) const
	{
		FudgetToken token = GetToken(token_name);
		return GetElementPainter(token);
	}

	/// <summary>
	/// Tries to retreive an element painter object for a token
	/// </summary>
	/// <param name="token">The token associated with the element painter</param>
	/// <returns>The element painter, or null if a painter with the token was not found</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(FudgetToken token) const
	{
		if (token == FudgetToken::Invalid)
			return nullptr;
		auto it = element_map.find(token);
		if (it == element_map.end())
			return nullptr;
		return it->second;
	}

private:
	// Adds some hard-coded values for now.
	void Init();

	static std::map<String, FudgetToken> token_map;
	static int highest_token;

	//std::map<FudgetToken, FudgetStyleBase*> style_map;
	std::map<FudgetToken, FudgetElementPainter*> element_map;
};
