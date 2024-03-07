#pragma once

#include <map>
#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Graphics/Textures/TextureBase.h"


class FudgetThemes;
class FudgetTheme;
class FudgetPartPainter;
struct FudgetDrawArea;
struct FudgetTextDrawSettings;
struct FudgetPadding;
struct Color;
struct FudgetFontSettings;
struct FudgetFont;
class FudgetDrawable;
class FudgetStyle;
struct FudgetPartPainterMapping;

struct FudgetStyleResource
{
    FudgetStyleResource(FudgetStyle *owner) : _resource_owner(owner) {}
    ~FudgetStyleResource() {}

    // Owner style of this resource.
    FudgetStyle *_resource_owner;

    // Don't use. This is the "closest" resource in a parent style that has valid values. It's a cached value
    // when the resource of a style doesn't have overrides.
    FudgetStyleResource *_inherited_resource = nullptr;

    // The resource id to look for in a theme's resources.
    int _resource_id = -1;

    // The direct value of the resource set for this style, overriding inherited values or ids.
    Variant _value_override = Variant();
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
    /// Creates a new style that inherits all its values from this one, or null if the name is empty or is already taken.
    /// If this style is destroyed, the inherited style will be destroyed as well.
    /// </summary>
    /// <param name="name">Name of the style to create.</param>
    /// <returns>The created style or null on wrong name</returns>
    API_FUNCTION() FudgetStyle* CreateOwnedStyle(const String &name);

    /// <summary>
    /// Returns a style inherited from this style by name.
    /// </summary>
    /// <param name="name">Name of the style to return</param>
    /// <returns>The owned style with the name or null</returns>
    API_FUNCTION() FudgetStyle* GetOwnedStyle(const String &name) const;

    API_PROPERTY() bool IsOwnedStyle() const { return _owned_style; }

    /// <summary>
    /// Creates a new style that inherits all its values from this one, named as the template argument class' full name, or
    /// null if the name is already taken. The style will be registered in the main theme database and will be associated
    /// with the template argument's type.
    /// </summary>
    /// <returns>The created style or null if style with a clashing name exists</returns>
    template<typename T>
    FudgetStyle* CreateInheritedStyle()
    {
        return InheritStyleInternal(T::TypeInitializer.GetType().Fullname.ToString(), _owned_style);
    }

    /// <summary>
    /// The name of the style that was used to create it.
    /// </summary>
    API_PROPERTY() const String& GetName() const { return _name; }

    /// <summary>
    /// Retrieves the resource's value associated with an id in this style. If this style has no override for the id,
    /// the parent style is checked, up to the topmost ancestor. The result is either a value set with SetValueOverride,
    /// or a value looked up in the theme with an id set with SetResourceOverride. The result can also be null if no overrides
    /// were set. If a FudgetResourceId value override is set, the function acts like it was called with its id instead,
    /// starting from the style where the override was set. Cyclic dependency is detected in the same style only. Not when
    /// the next FudgetResourceId value override was found in a parent style.
    /// If check_theme is true, and no style in the chain of styles contains a resource or override to a resource for the id,
    /// the theme is checked for a resource for the id.
    /// </summary>
    /// <param name="theme">The theme to check for values if no direct value override was set</param>
    /// <param name="id">The id to look up in this style for overrides</param>
    /// <param name="check_theme">Whether to try to get a resource from the theme directly, if neither the style nor a parent style has an override</param>
    /// <param name="result">The retrieved value associated with the id</param>
    /// <returns>Whether a value with the id was found and stored in result</returns>
    API_FUNCTION() bool GetResourceValue(FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Variant &result);

    /// <summary>
    /// Retrieves the resource's value associated with an id in this style. If this style has no override for the id,
    /// the parent style is checked, up to the topmost ancestor. The result is either a value directly set with SetValueOverride,
    /// or a value looked up in the theme with an id set with SetResourceOverride. The result can also be null if no overrides
    /// were set.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">The theme to check for values if no direct value override was set</param>
    /// <param name="ids">An array of ids to look up in this style for overrides, until one id is found</param>
    /// <param name="check_theme">Whether to try to get a resource from the theme directly, if neither the style nor a parent style has an override</param>
    /// <param name="result">The retrieved value associated with the id</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetResourceValue(FudgetTheme *theme, const Span<int> &ids, bool check_theme, API_PARAM(Out) Variant &result);

    /// <summary>
    /// Sets value as the override for the passed id. Calling GetResourceValue will return this value directly, unless
    /// an inherited style is queried which has a different override for it.
    /// </summary>
    /// <param name="id">Id to be used to retrieve the value later. Usually this is the id of a resource in a theme</param>
    /// <param name="value">Value to set for the id</param>
    API_FUNCTION() void SetValueOverride(int id, const Variant &value);

    /// <summary>
    /// Sets a resource id override that will be used to retrieve a value from a theme. Calling GetResourceValue with the
    /// id will fetch the resource id override, and will use that to look for the value in the theme. 
    /// </summary>
    /// <param name="id">Id to be used to retrieve a value later. Usually this is the id of a resource in a theme</param>
    /// <param name="resource_id">The resource id that overrides the original</param>
    API_FUNCTION() void SetResourceOverride(int id, int resource_id);

    /// <summary>
    /// Removes the override in this style for an id. It doesn't affect styles that this style inherits from.
    /// </summary>
    /// <param name="id">Id associated with the resource</param>
    API_FUNCTION() void ResetResourceOverride(int id);

    /// <summary>
    /// Looks up a string resource associated with an id in this style or a parent style using the theme. If the string
    /// is the name of a style, sets the result to the style. Returns whether the style was found.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the style if it was found.</param>
    /// <returns>Whether the id was associated with a string representing a style or not</returns>
    API_FUNCTION() bool GetStyleResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetStyle* &result);
    /// <summary>
    /// Looks up a string resource associated with an id in this style or a parent style using the theme. If the string
    /// is the name of a style, sets the result to the style. Returns whether the style was found.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the style if it was found.</param>
    /// <returns>Whether one of the ids was associated with a string representing a style or not</returns>
    API_FUNCTION() bool GetStyleResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetStyle* &result);

    /// <summary>
    /// Looks up a painter mapping resource associated with an id in this style or a parent style using the theme.
    /// Returns whether the mapping was found.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the style if it was found.</param>
    /// <returns>Whether the id was associated with a painter mapping or not</returns>
    API_FUNCTION() bool GetPainterMappingResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetPartPainterMapping &result);

    /// <summary>
    /// Looks up a painter mapping resource associated with an id in this style or a parent style using the theme.
    /// Returns whether the mapping was found.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the painter mapping if it was found.</param>
    /// <returns>Whether one of the ids was associated with a painter mapping or not</returns>
    API_FUNCTION() bool GetPainterMappingResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetPartPainterMapping &result);

    /// <summary>
    /// Looks up a string resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetStringResource(FudgetTheme *theme, int id, API_PARAM(Out) String &result);
    /// <summary>
    /// Looks up a string resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetStringResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) String &result);

    /// <summary>
    /// Looks up a color resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetColorResource(FudgetTheme *theme, int id, API_PARAM(Out) Color &result);

    /// <summary>
    /// Looks up a color resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetColorResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Color &result);

    /// <summary>
    /// Looks up a bool resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetBoolResource(FudgetTheme *theme, int id, API_PARAM(Out) bool &result);

    /// <summary>
    /// Looks up a bool resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetBoolResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) bool &result);

    /// <summary>
    /// Looks up a float resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFloatResource(FudgetTheme *theme, int id, API_PARAM(Out) float &result);

    /// <summary>
    /// Looks up a float resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFloatResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) float &result);

    /// <summary>
    /// Looks up a Float2 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFloat2Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float2 &result);

    /// <summary>
    /// Looks up a Float2 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFloat2Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float2 &result); 

    /// <summary>
    /// Looks up a Float3 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFloat3Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float3 &result);

    /// <summary>
    /// Looks up a Float3 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFloat3Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float3 &result); 

    /// <summary>
    /// Looks up a Float4 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFloat4Resource(FudgetTheme *theme, int id, API_PARAM(Out) Float4 &result);

    /// <summary>
    /// Looks up a Float4 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFloat4Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Float4 &result);

    /// <summary>
    /// Looks up a int resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetIntResource(FudgetTheme *theme, int id, API_PARAM(Out) int &result);

    /// <summary>
    /// Looks up a int resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetIntResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) int &result);

    /// <summary>
    /// Looks up a Int2 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetInt2Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int2 &result);

    /// <summary>
    /// Looks up a Int2 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetInt2Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int2 &result);

    /// <summary>
    /// Looks up a Int3 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetInt3Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int3 &result);

    /// <summary>
    /// Looks up a Int3 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetInt3Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int3 &result);

    /// <summary>
    /// Looks up a Int4 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetInt4Resource(FudgetTheme *theme, int id, API_PARAM(Out) Int4 &result);

    /// <summary>
    /// Looks up a Int4 resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetInt4Resource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) Int4 &result);

    /// <summary>
    /// Looks up an enum resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <typeparam name="T">Type of the enum</typeparam>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    template<typename T>
    bool GetEnumResource(FudgetTheme *theme, int id, API_PARAM(Out) T &result)
    {
        Variant var;
        if (GetResourceValue(theme, id, true, var))
        {
            if (EnumFromVariant<T>(var, result))
                return true;
        }
        result = T();
        return false;
    }

    /// <summary>
    /// Looks up an enum resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <typeparam name="T">Type of the enum</typeparam>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    template<typename T>
    bool GetEnumResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) T &result)
    {
        for (auto id : ids)
        {
            if (GetEnumResource(theme, id, result))
                return true;
        }
        result = T();
        return false;
    }

    /// <summary>
    /// Looks up a FudgetPadding resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetPaddingResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetPadding &result);

    /// <summary>
    /// Looks up a FudgetPadding resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetPaddingResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetPadding &result);

    /// <summary>
    /// Looks up a FudgetTextDrawSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetTextDrawResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetTextDrawSettings &result);

    /// <summary>
    /// Looks up a FudgetTextDrawSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetTextDrawResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetTextDrawSettings &result);

    /// <summary>
    /// Looks up a FudgetFontSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFontSettingsResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetFontSettings &result);

    /// <summary>
    /// Looks up a FudgetFontSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFontSettingsResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetFontSettings &result);

    /// <summary>
    /// Looks up a FudgetFont resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetFontResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetFont &result);

    /// <summary>
    /// Looks up a FudgetFont resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetFontResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetFont &result);

    /// <summary>
    /// Looks up a FudgetDrawArea resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetDrawAreaResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetDrawArea &result);

    /// <summary>
    /// Looks up a FudgetDrawArea resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetDrawAreaResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetDrawArea &result);

    /// <summary>
    /// Looks up a FudgetDrawInstructionList associated with an id in this style or a parent style using the theme, and sets the result
    /// to a FudgetDrawable created from it.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetDrawableResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetDrawable* &result);

    /// <summary>
    /// Looks up a FudgetDrawInstructionList associated with an id in this style or a parent style using the theme, and sets the result
    /// to a FudgetDrawable created from it.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetDrawableResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetDrawable* &result);

    /// <summary>
    /// Looks up a texture resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetTextureResource(FudgetTheme *theme, int id, API_PARAM(Out) TextureBase* &result);

    /// <summary>
    /// Looks up a texture resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetTextureResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) TextureBase* &result);

    /// <summary>
    /// Looks up a FudgetTextDrawSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="id">Id that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether the id was associated with a value of the requested type</returns>
    API_FUNCTION() bool GetTextDrawSettingsResource(FudgetTheme *theme, int id, API_PARAM(Out) FudgetTextDrawSettings &result);

    /// <summary>
    /// Looks up a FudgetTextDrawSettings resource associated with an id in this style or a parent style using the theme, and sets the result
    /// to it on success.
    /// This version of the function checks multiple ids until one matches.
    /// </summary>
    /// <param name="theme">Theme that is checked for the value, unless a direct value override was set.</param>
    /// <param name="ids">Array of ids that might be associated with the value in this style.</param>
    /// <param name="result">Receives the resource's value if it is found.</param>
    /// <returns>Whether a value with an id was found that was stored in result</returns>
    API_FUNCTION() bool GetTextDrawSettingsResource(FudgetTheme *theme, const Span<int> &ids, API_PARAM(Out) FudgetTextDrawSettings &result);
protected:
    /// <summary>
    /// Creates a new style that inherits all its values from this one, or null if the name is empty or is already taken.
    /// If named is true, the new style will be owned by this one and destroyed when this style is destroyed. The name only needs
    /// to be unique in its parent or in the theme for non owned styles.
    /// </summary>
    /// <param name="name">Name of the new style..</param>
    /// <param name="named">Whether the new style is a named style owned by this style and is not a style for a control class.</param>
    /// <returns>The created style or null if the name was empty or taken</returns>
    API_FUNCTION() FudgetStyle* InheritStyleInternal(const String &name, bool owned);
private:
    /// <summary>
    /// Retrieves the resource associated with an id. If the resource in this style is not overriden, the one
    /// in a parent style is returned. If no resource is found in any of the styles, the result is null.
    /// Don't hold on to this resource because the value can be deleted or reallocated any time.
    /// </summary>
    /// <param name="id">Id associated with the resource</param>
    /// <returns>The resource if found or null</returns>
    FudgetStyleResource* GetResource(int id);

    // Called from a parent style when a resource override was changed, but not when it was set to null.
    void ParentResourceChanged(int id, FudgetStyleResource *resource);

    // Called from a parent style when a resource override was reset or set to null.
    void ParentResourceWasReset(int id, FudgetStyleResource *resource);

    bool PainterMappingFromVariant(const Variant &var, FudgetPartPainterMapping &result) const;
    bool StringFromVariant(const Variant &var, String &result) const;
    bool DrawableFromVariant(FudgetTheme *theme, const Variant &var, FudgetDrawable* &result);
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
        auto name = StaticType<T>().GetType().Fullname;
        if (var.Type.Type == VariantType::Enum && name == var.Type.GetTypeName())
        {
            result = (T)var.AsInt;
            return true;
        }
        return false;
    }

    /// <summary>
    /// Style that this style directly inherited.
    /// </summary>
    FudgetStyle *_parent;
    /// <summary>
    /// Styles created for classes inherited from this style. These styles are registered in the main theme database and
    /// can be retrieved by name from it.
    /// </summary>
    Array<FudgetStyle*> _inherited;
    /// <summary>
    /// Styles created with names, owned by this style. They don't get registered with the main theme database. These styles
    /// can be fetched with GetInheritedNamed.
    /// </summary>
    Array<FudgetStyle*> _owned;

    /// <summary>
    /// The name used as the name for this style when the style was created.
    /// </summary>
    String _name;

    /// <summary>
    /// True only for styles created by a name instead of by class. Owned styles are owned by the style they inherit, and
    /// get destroyed when that style is destroyed.
    /// </summary>
    bool _owned_style;

    // Cached values and overrides. The style only holds resources that were set directly in the style or were
    // once queried with GetResource.
    std::map<int, FudgetStyleResource*> _resources;

    friend class FudgetThemes;
};

