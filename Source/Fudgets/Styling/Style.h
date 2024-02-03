#pragma once

#include <map>
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Types/Variant.h"

#include "Token.h"

class FudgetThemes;
class FudgetTheme;
class FudgetElementPainter;


struct FudgetStyleResource
{
	// Don't use. This is the "closest" resource in a parent style that has valid values. It's a cached value
	// when the resource of a style doesn't have overrides.
	FudgetStyleResource *_inherited_resource = nullptr;

	// The resource id to look for in a theme's Resources dictionary.
	FudgetToken _resource_id = FudgetToken::Invalid;

	// The direct value of the resource set for this style, overriding inherited values or ids.
	Variant _value_override = Variant::Null;
};


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
	/// <summary>
	/// Initializes a new style. Don't call directly as it won't be registered with the themes system. Use FudgetThemes::CreateStyle instead.
	/// </summary>
	FudgetStyle();
	/// <summary>
	/// Don't destroy styles directly. The theme will clean them up when the UI is uninitialized.
	/// </summary>
	~FudgetStyle();

	/// <summary>
	/// Creates a new style that inherits all its values from this one, or null if the token is already taken.
	/// </summary>
	/// <returns></returns>
	API_FUNCTION() FudgetStyle* CreateInheritedStyle(FudgetToken name_token);

	/// <summary>
	/// Mainly for internal use.
	/// Retrieves the resource associated with a token. If the resource in this style is not overriden, the one
	/// in a parent style is returned. If no resource is found in any of the styles, the result is null.
	/// Don't hold on to this resource because the value can be deleted or reallocated any time.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	/// <returns>The resource if found or null</returns>
	API_FUNCTION() FudgetStyleResource* GetResource(FudgetToken token);

	/// <summary>
	/// Retrieves the resource's value associated with a token in this style. If this style has no override for the token,
	/// the parent style is checked, up to the topmost ancestor. The result is either a value directly set with SetValueOverride,
	/// or a value looked up in the theme with a token set with SetResourceOverride. The result can also be null if no overrides
	/// were set.
	/// </summary>
	/// <param name="theme">The theme to check for values if no direct value override was set</param>
	/// <param name="token">The token to look up in this style for overrides</param>
	/// <param name="result">The retrieved value associated with the token</param>
	/// <returns>Whether a value with the token was found and stored in result</returns>
	API_FUNCTION() bool GetResourceValue(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Variant &result);

	/// <summary>
	/// Sets value as the override for the passed token. Calling GetResourceValue will return this value directly, unless
	/// an inherited style is queried which has a different override for it.
	/// </summary>
	/// <param name="token">Token to be used to retrieve the value later. Usually this is the token of a resource in a theme</param>
	/// <param name="value">Value to set for the token</param>
	API_FUNCTION() void SetValueOverride(FudgetToken token, const Variant &value);

	/// <summary>
	/// Sets a resource token that will be used to retrieve a value from a theme. Calling GetResourceValue with the token will
	/// fetch the resource token, and will use that to look for the value in the theme. 
	/// </summary>
	/// <param name="token">Token to be used to retrieve a value later. Usually this is the token of a resource in a theme</param>
	/// <param name="resource_token">The new resource token</param>
	API_FUNCTION() void SetResourceOverride(FudgetToken token, FudgetToken resource_token);

	/// <summary>
	/// Removes the override in this style for a token. It doesn't affect styles that this style inherits from.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	API_FUNCTION() void ResetResourceOverride(FudgetToken token);

private:
	// Called from a parent style when a resource override was changed, but not when it was set to null.
	void ParentResourceChanged(FudgetToken token, FudgetStyleResource *resource);

	// Called from a parent style when a resource override was reset or set to null.
	void ParentResourceWasReset(FudgetToken token, FudgetStyleResource *resource);

	FudgetStyle *_parent;
	Array<FudgetStyle*> _derived;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStyleResource> _resources;

	friend class FudgetThemes;
};
