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

struct FudgetStylePainterResource
{
	// Don't use. This is the "closest" resource in a parent style that has valid values. It's a cached value
	// when the resource of a style doesn't have overrides.
	FudgetStylePainterResource *_inherited_resource = nullptr;

	// The painter id to look for in a theme's PainterIds dictionary.
	FudgetToken _resource_id = FudgetToken::Invalid;

	// The painter id referencing the painter directly in FudgetThemes.
	FudgetToken _value_override = FudgetToken::Invalid;
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

	/// <summary>
	/// Mainly for internal use.
	/// Retrieves the painter resource associated with a token. If the resource in this style is not overriden, the one
	/// in a parent style is returned. If no resource is found in any of the styles, the result is null.
	/// Don't hold on to this resource because the value can be deleted or reallocated any time.
	/// </summary>
	/// <param name="token">Token associated with the painter resource</param>
	/// <returns>The resource if found or null</returns>
	API_FUNCTION() FudgetStylePainterResource* GetPainterResource(FudgetToken token);

	/// <summary>
	/// Tries to find an element painter from a theme or from the global theme database. If this or an ancestor style has an
	/// override for the painter token, it'll be used to get the painter. Otherwise the token is searched in the theme for
	/// an associated token in the global theme database. In case the token is not found, the theme database is searched
	/// directly.
	/// </summary>
	/// <param name="theme">Theme to search for an associated painter token</param>
	/// <param name="token">Token used for the painter either to override in the style or to store in the theme</param>
	/// <returns>An element painter if found, or null</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(FudgetTheme *theme, FudgetToken token);

	/// <summary>
	/// Tries to get the element painter for one of the tokens in class_tokens. This is usually called by the
	/// FudgetControl with a list of inherited class name tokens, but can be any token.
	/// The function repeats the same steps for each token in class_tokens, until one produces a result or nothing
	/// was found.
	/// If the style has a value override for the token, it'll be returned without looking further in the list.
	/// In case there is a resource override, it is checked in the theme for the painter id. With no
	/// overrides, the token is checked in the theme.
	/// Finally, if there was no result, the theme database is searched for each token in the list one by one.
	/// </summary>
	/// <param name="theme">Theme to search for an associated painter token</param>
	/// <param name="class_tokens">Array of tokens, usually generated from class names</param>
	/// <returns>An element painter if found or null</returns>
	API_FUNCTION() FudgetElementPainter* GetControlPainter(FudgetTheme *theme, Array<FudgetToken> class_tokens);

	/// <summary>
	/// Sets a resource token that will be used to retrieve a painter token from a theme. Calling GetElementPainter
	/// with the override will fetch the painter token from the theme, and then the painter from the theme database.
	/// </summary>
	/// <param name="token">Token to override</param>
	/// <param name="painter_token">Painter token to look for by the token</param>
	API_FUNCTION() void SetPainterResourceOverride(FudgetToken token, FudgetToken painter_token);

	/// <summary>
	/// Sets a token override that will be used to look up an element painter directly. Calling GetElementPainter with
	/// the token will retrieve the painter associated with painter token directly from the themes database.
	/// </summary>
	/// <param name="token">Token to override</param>
	/// <param name="painter_token">Painter token to look for in the themes database</param>
	API_FUNCTION() void SetPainterOverride(FudgetToken token, FudgetToken painter_token);

	/// <summary>
	/// Removes the painter override in this style for a token. It doesn't affect styles that this style inherits from.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	API_FUNCTION() void ResetPainterOverride(FudgetToken token);
private:
	// Called from a parent style when a resource override was changed, but not when it was set to null.
	void ParentResourceChanged(FudgetToken token, FudgetStyleResource *resource);

	// Called from a parent style when a resource override was reset or set to null.
	void ParentResourceWasReset(FudgetToken token, FudgetStyleResource *resource);

	// Called from a parent style when an element painter was changed, but not when it was set to null.
	void ParentPainterChanged(FudgetToken token, FudgetStylePainterResource *resource);

	// Called from a parent style when an element painter was reset or set to null.
	void ParentPainterWasReset(FudgetToken token, FudgetStylePainterResource *resource);

	FudgetStyle *_parent;
	Array<FudgetStyle*> _derived;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStyleResource> _resources;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStylePainterResource> _painters;

	friend class FudgetThemes;
};
