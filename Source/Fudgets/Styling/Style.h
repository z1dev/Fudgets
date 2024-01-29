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
	// The "closest" resource in a parent style that has valid values.
	FudgetStyleResource *_inherited_resource = nullptr;

	// The resource id set for this style, overriding the inherited values or ids
	FudgetToken _resource_id = FudgetToken::Invalid;

	// The value of the resource set for this style, overriding inherited values or ids.
	Variant _value_override = Variant::Null;
};

struct FudgetStylePainterResource
{
	// The "closest" resource in a parent style that has valid values.
	FudgetStylePainterResource *_inherited_resource = nullptr;

	// The value of the resource set for this style, overriding inherited values or ids.
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
	FudgetStyle(String name);
	FudgetStyle(FudgetToken name_token);
	virtual ~FudgetStyle();

	/// <summary>
	/// Creates a new style that inherits all its values from this one, or null if the token is already taken.
	/// </summary>
	/// <returns></returns>
	API_FUNCTION() FudgetStyle* CreateInheritedStyle(FudgetToken name_token);

	/// <summary>
	/// Retrieves the resource associated with a token. If the resource in this style is not overriden, the one
	/// in a parent style is returned. If no resource is found in any of the styles, the result is null.
	/// Don't hold onto this resource because the value can be deleted or reallocated any time.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	/// <returns>The resource if found or null</returns>
	API_FUNCTION() FudgetStyleResource* GetResource(FudgetToken token);

	/// <summary>
	/// Retrieves the resource's value associated with a token. If the resource in this style is not overriden, the
	/// one in a parent style is returned. If no resource is found in any of the styles, the result is false.
	/// </summary>
	/// <param name="theme"></param>
	/// <param name="token"></param>
	/// <param name="result"></param>
	/// <returns></returns>
	API_FUNCTION() bool GetResourceValue(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Variant &result);

	/// <summary>
	/// Sets an override for the resource of the passed token. The override is a direct value that will be used for the
	/// token, instead of looking up a value in the theme.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	/// <param name="value">Value to set for the token</param>
	API_FUNCTION() void SetValueOverride(FudgetToken token, const Variant &value);

	/// <summary>
	/// Sets an override for the resource of the passed token. The override is a resource token that cam be associated
	/// with a theme. It is possible to set a token that no theme has a value for.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	/// <param name="resource_token">Resource token to use to look up a value in themes</param>
	API_FUNCTION() void SetResourceOverride(FudgetToken token, FudgetToken resource_token);

	/// <summary>
	/// Removes the override of a resource for the given token if it had a value or resource_id overriden.
	/// </summary>
	/// <param name="token">Token associated with the resource</param>
	API_FUNCTION() void ResetResourceOverride(FudgetToken token);

	/// <summary>
	/// Tries to get an element painter for a class represented by the token. Painters might be set in this style
	/// or in a parent style. If none are set, null is returned.
	/// </summary>
	/// <param name="token">Token associated with the painter resource</param>
	/// <returns>The painter resource if found or null</returns>
	API_FUNCTION() FudgetStylePainterResource* GetPainterResource(FudgetToken token);

	/// <summary>
	/// Tries to get the element painter for the token from the style or the nearest parent style having the token.
	/// Returns null if no painter was found.
	/// </summary>
	/// <param name="theme">The theme that can supply the painter for the class</param>
	/// <param name="token">Array of the class' tokens</param>
	/// <returns>An element painter if found or null</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter(FudgetTheme *theme, FudgetToken token);

	/// <summary>
	/// Tries to get the element painter for a class or an ancestor of the class from the theme. Painters might
	/// be overriden in this style for class names, and then those will be returned instead.
	/// The first class to check is the one at the front of the tokens array. If it has an override, that one is
	/// returned, otherwise the theme is checked for a painter too. This is repeated for each class token in the
	/// array, until the last one was tried.
	/// Returns null if no painter was found.
	/// </summary>
	/// <param name="theme">The theme that can supply the painter for the class</param>
	/// <param name="class_tokens">Array of the class' tokens</param>
	/// <returns>An element painter if found or null</returns>
	API_FUNCTION() FudgetElementPainter* GetControlPainter(FudgetTheme *theme, Array<FudgetToken> class_tokens);

	/// <summary>
	/// Sets an override for the element painter for the given token. The override is a token used to retrieve the
	/// element painter from FudgetThemes.
	/// </summary>
	/// <param name="token">Token associated with the class that uses the element painter</param>
	/// <param name="painter_token">Token of the painter from FudgetThemes to use for the class</param>
	API_FUNCTION() void SetPainterOverride(FudgetToken token, FudgetToken painter_token);

	/// <summary>
	/// Removes the override of an element painter for the given token if it was overriden in the style.
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

	FudgetToken _style_name;

	FudgetStyle *_parent;
	Array<FudgetStyle*> _derived;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStyleResource> _resources;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStylePainterResource> _painters;
};
