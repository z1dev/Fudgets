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
struct FudgetBorder;
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
    /// Retrieves the resource value by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetResourceValue(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Variant &result);

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
    /// Retrieves the style resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetStyleResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetStyle* &result);


    /// <summary>
    /// Retrieves the painter mapping resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetPainterMappingResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetPartPainterMapping &result);


    /// <summary>
    /// Retrieves the string resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetStringResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) String &result);


    /// <summary>
    /// Retrieves the color resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetColorResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Color &result);


    /// <summary>
    /// Retrieves the bool resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetBoolResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) bool &result);


    /// <summary>
    /// Retrieves the float resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFloatResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) float &result);

    /// <summary>
    /// Retrieves the Float2 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFloat2Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float2 &result);

    /// <summary>
    /// Retrieves the Float3 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFloat3Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float3 &result);

    /// <summary>
    /// Retrieves the Float4 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFloat4Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Float4 &result);

    /// <summary>
    /// Retrieves the Int resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetIntResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) int &result);

    /// <summary>
    /// Retrieves the Int2 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetInt2Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int2 &result);

    /// <summary>
    /// Retrieves the Int3 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetInt3Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int3 &result);

    /// <summary>
    /// Retrieves the Int4 resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetInt4Resource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) Int4 &result);

    /// <summary>
    /// Retrieves the enum resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <typeparam name="T">The type of the enum to retreive</typeparam>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    template<typename T>
    static bool GetEnumResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) T &result)
    {
        Variant var;
        if (GetResourceValue(style, theme, id, check_theme, var))
        {
            if (EnumFromVariant<T>(var, result))
                return true;
        }

        result = T();
        return false;
    }

    /// <summary>
    /// Retrieves the FudgetPadding resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetPaddingResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetPadding &result);

    /// <summary>
    /// Retrieves the FudgetBorder resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetBorderResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetBorder &result);

    /// <summary>
    /// Retrieves the FudgetTextDrawSettings resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetTextDrawResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetTextDrawSettings &result);

    /// <summary>
    /// Retrieves the FudgetFontSettings resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFontSettingsResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetFontSettings &result);

    /// <summary>
    /// Retrieves the FudgetFont resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetFontResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetFont &result);

    /// <summary>
    /// Retrieves the FudgetDrawArea resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetDrawAreaResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetDrawArea &result);

    /// <summary>
    /// Retrieves the FudgetDrawable resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetDrawableResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetDrawable* &result);

    /// <summary>
    /// Retrieves the TextureBase resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetTextureResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) TextureBase* &result);

    /// <summary>
    /// Retrieves the FudgetTextDrawSettings resource by an id either from the style, one of its parent styles or the theme, checked in this order.
    /// The id might refer to a resource override, in which case the style attempts to retreive the value from the theme.
    /// The value is stored in result, unless the id wasn't found.
    /// </summary>
    /// <param name="style">The starting point to look up a value for the id. The parent styles are checked as well if nothing is found.</param>
    /// <param name="theme">The theme to get the resource from for resource overrides or when the id wasn't found in the style.</param>
    /// <param name="id">The id to look up for a resource value or resource override</param>
    /// <param name="check_theme">Whether the theme is checked directly for the id if it was not found in any of the styles.</param>
    /// <param name="result">Receives retrieved value associated with the id</param>
    /// <returns>Whether a resource with the id was found and stored in result</returns>
    API_FUNCTION() static bool GetTextDrawSettingsResource(FudgetStyle *style, FudgetTheme *theme, int id, bool check_theme, API_PARAM(Out) FudgetTextDrawSettings &result);

    static bool PainterMappingFromVariant(const Variant &var, FudgetPartPainterMapping &result);
    static bool StringFromVariant(const Variant &var, String &result);
    static bool DrawableFromVariant(FudgetStyle *style, FudgetTheme *theme, const Variant &var, FudgetDrawable* &result);
    static bool AreaFromVariant(const Variant &var, FudgetDrawArea &result);
    static bool TextureFromVariant(const Variant &var, TextureBase* &result);
    static bool TextDrawSettingsFromVariant(const Variant &var, FudgetTextDrawSettings &result);
    static bool FontSettingsFromVariant(const Variant &var, FudgetFontSettings &result);
    static bool FontFromVariant(const Variant &var, FudgetFont &result);
    static bool PaddingFromVariant(const Variant &var, FudgetPadding &result);
    static bool BorderFromVariant(const Variant &var, FudgetBorder &result);
    static bool BoolFromVariant(const Variant &var, bool &result);
    static bool FloatFromVariant(const Variant &var, float &result);
    static bool Float2FromVariant(const Variant &var, Float2 &result);
    static bool Float3FromVariant(const Variant &var, Float3 &result);
    static bool Float4FromVariant(const Variant &var, Float4 &result);
    static bool IntFromVariant(const Variant &var, int &result);
    static bool Int2FromVariant(const Variant &var, Int2 &result);
    static bool Int3FromVariant(const Variant &var, Int3 &result);
    static bool Int4FromVariant(const Variant &var, Int4 &result);
    static bool ColorFromVariant(const Variant &var, Color &result);

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

    template<typename T>
    static bool EnumFromVariant(const Variant &var, T &result)
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

