#pragma once

#include <map>
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Graphics/Textures/TextureBase.h"

#include "Token.h"

class FudgetThemes;
class FudgetTheme;
class FudgetElementPainter;
struct FudgetDrawArea;
struct FudgetTextDrawSettings;
struct FudgetPadding;
struct Color;
struct FudgetFontSettings;
struct FudgetFont;

struct FudgetStyleResource
{
	// Don't use. This is the "closest" resource in a parent style that has valid values. It's a cached value
	// when the resource of a style doesn't have overrides.
	FudgetStyleResource *_inherited_resource = nullptr;

	// The resource id to look for in a theme's resources.
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

	template<typename T>
	T* CreatePainter(FudgetTheme *theme, FudgetToken painter_token)
	{
		if (!painter_token.IsValid())
			return nullptr;

		FudgetToken token;
		if (!GetTokenResource(theme, painter_token, token))
			return nullptr;
		if (!token.IsValid())
			return nullptr;

		FudgetPartPainter *painter = FudgetThemes::CreatePainter(token);
		T *result = dynamic_cast<T*>(painter);
		if (result == nullptr)
		{
			Delete(result);
			return nullptr;
		}
		return result;
	}


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
	/// Retrieves the resource's value associated with a token in this style. If this style has no override for the token,
	/// the parent style is checked, up to the topmost ancestor. The result is either a value directly set with SetValueOverride,
	/// or a value looked up in the theme with a token set with SetResourceOverride. The result can also be null if no overrides
	/// were set.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">The theme to check for values if no direct value override was set</param>
	/// <param name="token">The token to look up in this style for overrides</param>
	/// <param name="result">The retrieved value associated with the token</param>
	/// <returns>Whether a value with the token was found and stored in result</returns>
	API_FUNCTION() bool GetResourceValue(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) Variant &result);

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
	/// Looks up a token resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTokenResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetToken &result);
	/// <summary>
	/// Looks up a token resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTokenResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetToken &result);

	/// <summary>
	/// Looks up a color resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetColorResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Color &result);

	/// <summary>
	/// Looks up a color resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetColorResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Color &result);

	/// <summary>
	/// Looks up a bool resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetBoolResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) bool &result);

	/// <summary>
	/// Looks up a bool resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetBoolResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) bool &result);

	/// <summary>
	/// Looks up a float resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloatResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) float &result);

	/// <summary>
	/// Looks up a float resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloatResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) float &result);

	/// <summary>
	/// Looks up a Float2 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat2Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float2 &result);

	/// <summary>
	/// Looks up a Float2 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat2Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Float2 &result); 

	/// <summary>
	/// Looks up a Float3 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat3Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float3 &result);

	/// <summary>
	/// Looks up a Float3 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat3Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Float3 &result); 

	/// <summary>
	/// Looks up a Float4 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat4Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Float4 &result);

	/// <summary>
	/// Looks up a Float4 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFloat4Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Float4 &result);

	/// <summary>
	/// Looks up a int resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetIntResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) int &result);

	/// <summary>
	/// Looks up a int resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetIntResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) int &result);

	/// <summary>
	/// Looks up a Int2 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt2Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int2 &result);

	/// <summary>
	/// Looks up a Int2 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt2Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Int2 &result);

	/// <summary>
	/// Looks up a Int3 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt3Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int3 &result);

	/// <summary>
	/// Looks up a Int3 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt3Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Int3 &result);

	/// <summary>
	/// Looks up a Int4 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt4Resource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) Int4 &result);

	/// <summary>
	/// Looks up a Int4 resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetInt4Resource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) Int4 &result);

	/// <summary>
	/// Looks up a FudgetPadding resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetPaddingResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetPadding &result);

	/// <summary>
	/// Looks up a FudgetPadding resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetPaddingResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetPadding &result);

	/// <summary>
	/// Looks up a FudgetTextDrawSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextDrawResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetTextDrawSettings &result);

	/// <summary>
	/// Looks up a FudgetTextDrawSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextDrawResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetTextDrawSettings &result);

	/// <summary>
	/// Looks up a FudgetFontSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFontSettingsResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetFontSettings &result);

	/// <summary>
	/// Looks up a FudgetFontSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFontSettingsResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetFontSettings &result);

	/// <summary>
	/// Looks up a FudgetFont resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFontResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetFont &result);

	/// <summary>
	/// Looks up a FudgetFont resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetFontResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetFont &result);

	/// <summary>
	/// Looks up a FudgetDrawArea resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetDrawAreaResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetDrawArea &result);

	/// <summary>
	/// Looks up a FudgetDrawArea resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetDrawAreaResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetDrawArea &result);

	/// <summary>
	/// Looks up a texture resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextureResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) TextureBase* &result);

	/// <summary>
	/// Looks up a texture resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextureResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) TextureBase* &result);

	/// <summary>
	/// Looks up a FudgetTextDrawSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextDrawSettingsResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) FudgetTextDrawSettings &result);

	/// <summary>
	/// Looks up a FudgetTextDrawSettings resource associated with a token in this style or a parent style using the theme, and sets the result
	/// value to it on success.
	/// This version of the function checks multiple tokens until one matches.
	/// </summary>
	/// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
	/// <param name="token">Token that might be associated with the value in this style.</param>
	/// <param name="result">Receives the resource's value if it is found.</param>
	/// <returns>Whether the token was associated with a value of the requested type</returns>
	API_FUNCTION() bool GetTextDrawSettingsResource(FudgetTheme *theme, const Span<FudgetToken> &token, API_PARAM(Out) FudgetTextDrawSettings &result);

	template<typename T>
	bool GetEnumResource(FudgetTheme *theme, FudgetToken token, API_PARAM(Out) T &result)
	{
		Variant var;
		if (GetResourceValue(theme, token, var))
		{
			if (EnumFromVariant<T>(var, result))
				return true;
		}
		result = T();
		return false;
	}

	template<typename T>
	bool GetEnumResource(FudgetTheme *theme, const Span<FudgetToken> &tokens, API_PARAM(Out) T &result)
	{
		for (auto t : tokens)
		{
			if (GetEnumResource(theme, t, result))
				return true;
		}
		result = T();
		return false;
	}

private:
	// Called from a parent style when a resource override was changed, but not when it was set to null.
	void ParentResourceChanged(FudgetToken token, FudgetStyleResource *resource);

	// Called from a parent style when a resource override was reset or set to null.
	void ParentResourceWasReset(FudgetToken token, FudgetStyleResource *resource);

	bool TokenFromVariant(const Variant &var, FudgetToken &result) const;
	bool AreaFromVariant(const Variant &var, FudgetDrawArea &result) const;
	bool TextureFromVariant(const Variant &var, TextureBase* &result) const;
	bool TextDrawSettingsFromVariant(const Variant &var, FudgetTextDrawSettings &result) const;
	bool FontSettingsFromVariant(const Variant &var, FudgetFontSettings &result) const;
	bool FontFromVariant(const Variant &var, FudgetFont &result) const;
	bool PaddingFromVariant(const Variant &var, FudgetPadding &result) const;
	bool BoolFromVariant(const Variant &var, bool &result) const;
	bool FloatFromVariant(const Variant &var, float &result) const;
	bool Float2FromVariant(const Variant &var, Float2 &result) const;
	bool Float3FromVariant(const Variant &var, Float3 &result) const;
	bool Float4FromVariant(const Variant &var, Float4 &result) const;
	bool IntFromVariant(const Variant &var, int &result) const;
	bool Int2FromVariant(const Variant &var, Int2 &result) const;
	bool Int3FromVariant(const Variant &var, Int3 &result) const;
	bool Int4FromVariant(const Variant &var, Int4 &result) const;
	bool ColorFromVariant(const Variant &var, Color &result) const;

	template<typename T>
	bool EnumFromVariant(const Variant &var, T &result) const
	{
		if (var.Type.Type == VariantType::Enum)
		{
			result = (T)var.AsInt;
			return true;
		}
		return false;
	}

	FudgetStyle *_parent;
	Array<FudgetStyle*> _derived;

	// Cached values and overrides. The style only holds resources that were set directly in the style or were
	// once queried with GetResource.
	std::map<FudgetToken, FudgetStyleResource> _resources;

	friend class FudgetThemes;
};

