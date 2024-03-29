#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Render2D/FontAsset.h"

#include "Style.h"
#include "StyleStructs.h"
#include "../Utils/Utils.h"

#include <map>
#include <vector>

class FudgetPartPainter;
struct FudgetDrawInstructionList;
class FudgetDrawable;

/// <summary>
/// Simple struct used as a theme resource that acts like a pointer to a different resource. If a requested resource
/// in a  FudgetTheme holds this value, the theme will try to get the other resource instead. It shouldn't be set
/// as a value override in a style.
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetResourceId
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetResourceId);

    FudgetResourceId() : Id(0) {}
    FudgetResourceId(int forwarded_id) : Id(forwarded_id) {}
    FudgetResourceId(const FudgetResourceId &other) : Id(other.Id) {}
    FudgetResourceId(FudgetResourceId &&other) noexcept : Id(other.Id) {}
    FudgetResourceId& operator=(const FudgetResourceId &other) { Id = other.Id; return *this; }
    FudgetResourceId& operator=(FudgetResourceId &&other) noexcept  { Id = other.Id; return *this; }

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
    /// Retreives the name of the style associated with a class' full name. It's used to get the style when a control of
    /// this class tries to paint itself.
    /// </summary>
    /// <param name="class_name">The full name of the class to get the style name for</param>
    /// <returns>Name of the style used for the class in this theme. Returns the class name itself if not set.</returns>
    API_FUNCTION() const String& GetClassStyleName(const String &class_name) const;

    /// <summary>
    /// Sets the name of the style to associate with a class' full name. It's used to get the style when a control of
    /// this class tries to paint itself.
    /// </summary>
    /// <param name="class_name">he full name of the class to set the style name for</param>
    /// <param name="style_name">Name of the style to use for the class in this theme.</param>
    API_FUNCTION() void SetClassStyleName(const String &class_name, const String &style_name);

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


    /// <summary>
    /// Sets forwarding for an integer id to another integer id. When GetResource is called with the id, the forwarded
    /// id will be looked up instead and the resource found there will be returned. Chaining multiple forwarded ids
    /// is possible, but circular forwarding results in undefined behavior. (Hopefully just returns false)
    /// </summary>
    /// <param name="res_id">Unique id of the resource</param>
    /// <param name="forward_id">The value of the resource</param>
    API_FUNCTION() void SetForwarding(int res_id, int forward_id);

    template<typename T>
    typename std::enable_if<Fudget_is_class<T>(), void>::type SetResource(int res_id, const T &value)
    {
        SetResource(res_id, StructToVariant(value));
    }


    template<typename R>
    typename std::enable_if<Fudget_is_resource_id<R>(), void>::type SetResource(R res_id, const Variant &value)
    {
        SetResource((int)res_id, value);
    }

    template<typename R, typename T>
    typename std::enable_if<Fudget_is_resource_id<R>() && Fudget_is_class<T>(), void>::type SetResource(R res_id, const T &value)
    {
        SetResource((int)res_id, StructToVariant(value));
    }

    template<typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type SetResource(int res_id, const T &value)
    {
        SetResource(res_id, EnumToVariant(value));
    }

    template<typename R, typename T>
    typename std::enable_if<Fudget_is_resource_id<R>() && std::is_enum<T>::value, void>::type SetResource(R res_id, const T &value)
    {
        SetResource((int)res_id, EnumToVariant(value));
    }


    template<typename T>
    typename std::enable_if<Fudget_is_resource_id<T>(), void>::type SetForwarding(int res_id, T forward_id)
    {
        SetForwarding((int)res_id, (int)forward_id);
    }

    template<typename R>
    typename std::enable_if<Fudget_is_resource_id<R>(), void>::type SetForwarding(R res_id, int forward_id)
    {
        SetForwarding((int)res_id, (int)forward_id);
    }

    template<typename R, typename T>
    typename std::enable_if<Fudget_is_resource_id<R>() && Fudget_is_resource_id<T>(), void>::type SetForwarding(R res_id, T forward_id)
    {
        SetForwarding((int)res_id, (int)forward_id);
    }

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

    /// <summary>
    /// Mapping between a class full name to the style name they use by default in this theme.
    /// </summary>
    Dictionary<String, String> _class_styles;

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
    // Style names for styles as a default hierarchy for the built in painters and to inherit in
    // user-made styles.

    /// <summary>
    /// Base style for controls with a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String FIELD_CONTROL_STYLE;

    /// <summary>
    /// Base style for controls with a text input field
    /// </summary>
    API_FIELD(ReadOnly) static const String TEXT_INPUT_STYLE;

    /// <summary>
    /// Base style for controls with a text input field
    /// </summary>
    API_FIELD(ReadOnly) static const String SINGLELINE_TEXT_INPUT_STYLE;

    /// <summary>
    /// Base style for controls with a text input field
    /// </summary>
    API_FIELD(ReadOnly) static const String MULTILINE_TEXT_INPUT_STYLE;

    /// <summary>
    /// Base style for controls with a text input field
    /// </summary>
    API_FIELD(ReadOnly) static const String FRAMED_SINGLELINE_TEXT_INPUT_STYLE;

    /// <summary>
    /// Base style for controls with a text input field
    /// </summary>
    API_FIELD(ReadOnly) static const String FRAMED_MULTILINE_TEXT_INPUT_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String BUTTON_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String IMAGE_BUTTON_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String COMBOBOX_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String COMBOBOX_EDITOR_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String COMBOBOX_BUTTON_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String COMBOBOX_LIST_STYLE;

    /// <summary>
    /// Base style for controls with a text input field and a frame
    /// </summary>
    API_FIELD(ReadOnly) static const String LISTBOX_STYLE;

    /// <summary>
    /// Style holding values for scrollbars that should be referenced by other styles.
    /// This is a default scrollbar with no extra buttons and the page up and page down
    /// role for the scrollbar track. This can be changed by setting the appropriate resources
    /// in the theme.
    /// </summary>
    API_FIELD(ReadOnly) static const String SCROLLBAR_DEFAULT_STYLE;
    /// <summary>
    /// Style holding values for scrollbars that should be referenced by other styles.
    /// This is a scrollbar with up/down buttons for lineup/linedown behavior on the ends of
    /// the scrollbar track, and the page up and page down role for the scrollbar track.
    /// </summary>
    API_FIELD(ReadOnly) static const String SCROLLBAR_WINDOWS_BUTTONS_STYLE;
    /// <summary>
    /// Style holding values for scrollbars that should be referenced by other styles.
    /// This is a scrollbar with up/down buttons for lineup/linedown behavior at the bottom
    /// end of the scrollbar track, and the page up and page down role for the scrollbar
    /// track.
    /// </summary>
    API_FIELD(ReadOnly) static const String SCROLLBAR_OLDMAC_BUTTONS_STYLE;

    /// <summary>
    // Name of the default theme.
    /// </summary>
    API_FIELD(ReadOnly) static const String MAIN_THEME;


    /// <summary>
    /// Initializes the themes by allocating the main data structures. Call Uninitialize to free them. There are
    /// two sets of data when run in the editor, to make sure the running game is not corrupted when editing.
    /// Use the argument to specify which one is initialized. Set it to true from the normal plugin and false
    /// in the editor plugin.
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

    /// <summary>
    /// Sets up the main theme and adds the default styles.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() static void CreateDefaultThemesAndStyles();

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
    /// its class-appropriate style. If no style is found for the names, null is returned.
    /// </summary>
    /// <param name="theme">A theme that provides the style names used for each (class) name.</param>
    /// <param name="class_names">An array of names that are looked up one by one until one matches a style</param>
    /// <returns>The style that matches one of the names or null</returns>
    API_FUNCTION() static FudgetStyle* FindMatchingStyle(FudgetTheme *theme, const Array<String> &class_names);

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
    /// <summary>
    /// Used by FudgetDrawableBuilder to store the draw instruction list it created which can be used to make a FudgetDrawable.
    /// </summary>
    /// <param name="drawlist">Draw instruction list to register.</param>
    /// <returns>Index of the registered draw instructions list, or -1 on error.</returns>
    static int RegisterDrawInstructionList(const Array<uint64> &statelist, const std::vector<FudgetDrawInstructionList*> &drawlist);
    /// <summary>
    /// Returns the draw instruction list registered with RegisterDrawInstructionList by its index
    /// </summary>
    static bool GetDrawInstructionList(int drawlist_index, Array<uint64> &states_result, std::vector<FudgetDrawInstructionList*> &list_result);

    struct StatedDrawInstructions
    {
        StatedDrawInstructions(const Array<uint64> &states, const std::vector<FudgetDrawInstructionList*> &instructions) : _states(states), _instructions(instructions) {}

        Array<uint64> _states;
        std::vector<FudgetDrawInstructionList*> _instructions;
    };

    struct Data
    {
        // A collection of styles that provide values, like colors or floats to an element painter.
        Dictionary<String, FudgetStyle*> _style_map;

        // A collection of themes. Each theme is a collection of data in itself to provide values that can be used by
        // styles and element painters.
        Dictionary<String, FudgetTheme*> _theme_map;

        Dictionary<int, FontAsset*> _font_asset_map;

        std::vector<StatedDrawInstructions> _draw_list;
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

    // Number of times Initialize was called without a paired Uninitialize.
    static int _initialized_count;

    friend class FudgetDrawableBuilder;
    friend class FudgetStyle;
};
