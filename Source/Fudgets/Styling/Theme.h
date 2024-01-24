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

	template<typename T>
	T* CreateStyle(String token_name)
	{
		FudgetToken token = RegisterToken(token_name);
		if (token == FudgetToken::Invalid)
			return nullptr;

		T *style = New<T>(this, token);
		style_map[token] = style;

		return style;
	}

	FudgetToken GetToken(String token_name) const;
	FudgetToken RegisterToken(String token_name, bool duplicate_is_error = true);

	//FudgetStyleBase* GetStyle(String token_name) const;
	//FudgetStyleBase* GetStyle(FudgetToken token) const;

	template<typename T>
	T* GetElementPainter(String token_name) const
	{
		FudgetToken token = GetToken(token_name);
		return GetElementPainter<T>(token);
	}

	template<typename T>
	T* GetElementPainter(FudgetToken token) const
	{
		if (token == FudgetToken::Invalid)
			return nullptr;
		auto it = element_map.find(token);
		if (it == element_map.end())
			return nullptr;
		return dynamic_cast<T*>(it->second);
	}
private:
	// Adds some hard-coded values for now.
	void Init();

	std::map<String, FudgetToken> token_map;
	int highest_token;

	//std::map<FudgetToken, FudgetStyleBase*> style_map;
	std::map<FudgetToken, FudgetElementPainter*> element_map;
};
