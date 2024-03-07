#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Render2D/FontAsset.h"

#include "Style.h"
#include "StyleStructs.h"
#include "DrawableBuilder.h"

#include <map>
#include <vector>

class FudgetPartPainter;


/// <summary>
/// Simple struct used as a theme resource that acts like a pointer to a different resource. If a requested resource
/// in a  FudgetTheme holds this value, the theme will try to get the other resource instead. It shouldn't be set
/// as a value override in a style.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetResourceId
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetResourceId);

    /// <summary>
    /// The associated id.
    /// </summary>
    API_FIELD() int Id;
};

template<>
struct TIsPODType<FudgetResourceId>
{
    enum { Value = true };
};


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
    /// <summary>
    /// Don't use this. It's only public because it's needed to be available for the engine
    /// </summary>
    FudgetTheme();

    /// <summary>
    /// Don't use this. It's only public because it's needed to be available for the engine
    /// </summary>
    FudgetTheme(const FudgetTheme &ori);

    /// <summary>
    /// Gets a resource by an integer id or nothing if the resource is not found. The resource can be of any type.
    /// Its use only depends on the styles and controls that access it.
    /// </summary>
    /// <param name="res_id">Unique id of the resource</param>
    /// <param name="result">The value of the resource</param>
    /// <returns>Whether a resource was found for the id or not</returns>
    API_FUNCTION() bool GetResource(int res_id, API_PARAM(Out) Variant &result) const;

    /// <summary>
    /// Sets or creates a resource for an integer id. The resource can be of any type. Its use only depends on the
    /// styles and controls that can use it.
    /// </summary>
    /// <param name="res_id">Unique id of the resource</param>
    /// <param name="value">The value of the resource</param>
    API_FUNCTION() void SetResource(int res_id, Variant value);
private:
    /// <summary>
    /// Creates a theme that is a duplicate of this theme. All values will be matching. This also means that if an object
    /// is stored as a pointer, changes on its properties will change the same object in other theme.
    /// </summary>
    /// <returns></returns>
    FudgetTheme* Duplicate() const;


    /// <summary>
    /// The values in this theme, that can be referenced by styles. Although it's not read-only, the values
    /// shouldn't be changed directly, because styles cache the values and they won't be refreshed.
    /// </summary>
    std::map<int, Variant> _resources;

    friend class FudgetThemes;
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
    /// <summary>
    /// Initializes main themes and styles if they haven't been initializeds
    /// </summary>
    /// <param name="in_game">Whether called by the game plugin instead of the editor plugin</param>
    API_FUNCTION() static void Initialize(bool in_game);
    /// <summary>
    /// Uninitializes main themes and styles if they were initialized by a call to Initialize. Depending on
    /// whether this was called from the game plugin or not, might remove all objects that were created during
    /// gameplay.
    /// </summary>
    /// <param name="in_game">Whether called by the game plugin instead of the editor plugin</param>
    API_FUNCTION() static void Uninitialize(bool in_game);

    // Name for the base theme that's guaranteed to be present. "MainTheme"
    API_FIELD(ReadOnly) static const String MainThemeName;

    /// <summary>
    /// Stores a font asset with an integer id that can be retrieved with GetFontAsset using the same id. The id must be positive
    /// and unique among font ids.
    /// </summary>
    /// <param name="font_id">Id that can be used to access the font asset</param>
    /// <param name="asset">The font asset to store</param>
    /// <returns>Whether the asset was successfully stored. False if the id was already taken</returns>
    API_FUNCTION() static bool RegisterFontAsset(int font_id, FontAsset *asset);

    /// <summary>
    /// Returns a font asset for the id that was used to register it, or null if the id wasn't found.
    /// </summary>
    API_FUNCTION() static FontAsset* GetFontAsset(int font_id);

    /// <summary>
    /// Tries to retrieve a theme by name or null if the name wasn't found. The name is case sensitive and can't be empty.
    /// </summary>
    /// <param name="theme_name">Name of the theme</param>
    /// <returns>The theme if found, or null</returns>
    API_FUNCTION() static FudgetTheme* GetTheme(const String &theme_name);

    /// <summary>
    /// Creates a copy of a theme with source name as a new theme called by the destination name, unless the destination name
    /// is already taken. The names are case sensitive and can't be empty.
    /// All themes must be created this way to be registered and used.
    /// </summary>
    /// <param name="source_name">Name of the theme to duplicate. It must be for an existing theme.</param>
    /// <param name="dest_name">Name of the new theme. This name must be unique among themes.</param>
    /// <returns>The new theme, or null if one of the names were invalid</returns>
    API_FUNCTION() static FudgetTheme* DuplicateTheme(const String &source_name, const String &dest_name);

    /// <summary>
    /// Creates and registers a new style object with a unique name. The name is case sensitive and can't be empty. If the
    /// name is already taken, the result will be null. The style can be accessed with GetStyle later with the same name.
    /// Styles with a name matching a control's or painter's full class name will be used for that control or painter by
    /// default.
    /// </summary>
    /// <param name="name">Name of the new style</param>
    /// <returns>The created style, or null if the name was a duplicate or empty</returns>
    API_FUNCTION(NoProxy) static FudgetStyle* CreateStyle(const String &name);

    /// <summary>
    /// Returns a style with the given name, creating it if it doesn't exist. The name is case sensitive and can't be empty.
    /// The style can be accessed with GetStyle later with the same name. Styles with a name matching a control's or painter's
    /// full class name will be used for that control or painter by default. Use the template version of CreateOrGetStyle
    /// to create or get a style based on a class name.
    /// </summary>
    /// <param name="name">Name of the new style</param>
    /// <returns>The created style, or null if the name was a duplicate or empty</returns>
    API_FUNCTION(NoProxy) static FudgetStyle* CreateOrGetStyle(const String &name);

    template<typename T>
    static FudgetStyle* CreateOrGetStyle()
    {
        return CreateOrGetStyle(T::TypeInitializer.GetType().Fullname);
    }

    /// <summary>
    /// Tries to retrieve a style created with the given name. The name is case sensitive and can't be empty.
    /// </summary>
    /// <param name="name">Name of the style</param>
    /// <returns>The style if found or null</returns>
    API_FUNCTION() static FudgetStyle* GetStyle(const String &name);

    /// <summary>
    /// Returns a style for the first matching name in the array. The names usually come from a control when it wants to get
    /// its class appropriate style. If no style is found for the names, null is returned.
    /// </summary>
    /// <param name="class_names">An array of names that are looked up one by one until one matches a style</param>
    /// <returns>The style that matches one of the names or null</returns>
    API_FUNCTION() static FudgetStyle* FindMatchingStyle(const Array<String> &class_names, const String &styling_name);

    /// <summary>
    /// Creates a new painter object if the name represents a painter.
    /// </summary>
    API_FUNCTION() static FudgetPartPainter* CreatePainter(const StringAnsi &painter_name);

    /// <summary>
    /// Checks if there's a class by the passed name that was inherited from the template argument type. Both classes must
    /// have been declared with API_CLASS and have their type initializer set up in generated code.
    /// </summary>
    /// <typeparam name="BASE">Base type</typeparam>
    /// <param name="class_name">Possible name of a derived class</param>
    /// <returns>Whether the class with the class name was derived from the base type</returns>
    template<typename BASE>
    static bool IsDerivedType(const StringAnsi &class_name)
    {
        if (class_name.IsEmpty())
            return false;

        const ScriptingTypeHandle type = Scripting::FindScriptingType(class_name);
        if (!type)
            return false;

        return BASE::TypeInitializer.IsAssignableFrom(type);
    }

    /// <summary>
    /// Used by FudgetDrawableBuilder to store the draw instruction list it created which can be used to make a FudgetDrawable.
    /// </summary>
    /// <param name="drawlist">Draw instruction list to register.</param>
    /// <returns>Index of the registered draw instructions list, or -1 on error.</returns>
    static int RegisterDrawInstructionList(FudgetDrawInstructionList *drawlist);
    /// <summary>
    /// Used by FudgetDrawableBuilder to check if a draw instruction list has been already created with the given name.
    /// </summary>
    static bool IsDrawInstructionListRegistered(FudgetDrawInstructionList *drawlist);
    /// <summary>
    /// Returns the draw instruction list registered with RegisterDrawInstructionList by its index
    /// </summary>
    static FudgetDrawInstructionList* GetDrawInstructionList(int drawlist_index);

#ifdef USE_EDITOR
    /// <summary>
    /// True when the game is running inside the editor, and the theme functions refer to the data created for the game.
    /// </summary>
    API_PROPERTY() static bool GetRuntimeUse() { return _runtime_use; }
    /// <summary>
    /// Set or unset during the game's runtime inside the base classes when they need to access the themes. The use of
    /// this function is discouraged in any other situation.
    /// </summary>
    /// <param name="value">Whether the access to the themes data is from a fudget object running in a game or not</param>
    API_PROPERTY() static void SetRuntimeUse(bool value);
#endif

private:
    struct Data
    {
        // A collection of styles that provide values, like colors or floats to an element painter.
        Dictionary<String, FudgetStyle*> _style_map;

        // A collection of themes. Each theme is a collection of data in itself to provide values that can be used by
        // styles and element painters.
        Dictionary<String, FudgetTheme*> _theme_map;

        Dictionary<int, FontAsset*> _font_asset_map;

        std::vector<FudgetDrawInstructionList*> _style_area_list;
    };

#if USE_EDITOR
    // Whether the themes is currently using the runtime data.
    static bool _runtime_use;

    // Theme data used while the game is not running, both in the scene and in the style editor
    static Data* _edittime_data;

    // Theme data used by UI objects that are in the root gui hierarchy under a Fudget while the game is running.
    static Data* _runtime_data;

    static bool _edittime_initialized;
#endif

    static bool _initialized;
    static Data *_data;
};
