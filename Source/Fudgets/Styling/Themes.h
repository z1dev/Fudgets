#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Types/Variant.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Render2D/FontAsset.h"

#include "Token.h"
#include "Style.h"
#include "StyleStructs.h"

#include <map>


// Documentation of using themes and styles, and deriving new ones. Plus how to make controls use them to draw themselves.
// ============
// Summary of some important types:
// 
// - FudgetToken: (token or id for short) a simple identifier created from strings. Same string always results in the same
//      token while the program is running. It can be different between different runs with different assets. Because of
//      this, serialization should save the string and not the token. The special token FudgetToken::Invalid with a value of
//      -1 is always present and can be used to mark invalid values.
// 
// - FudgetTheme: (theme for short) a collection of resources (explained here) and painter object references (see below).
//      Resource can be any data used directly (color, padding) or indirectly (token). Each data can be accessed through a
//      token. For example a token might have been generated from the string "DarkBackgroundColor" and will correspond to a
//      color in the theme. Mostly styles will access resources directly (via a token). There is always a main theme that can
//      be accessed with FudgetThemes::MainThemeToken
// 
// FROM HERE ON: if this document writes "access the resource" or "gets the color for" etc, it should be assumed that tokens
//      were used. It will only mention tokens when they are relevant to the explanation. Also, when saying "id" it will refer
//      to tokens, and direct strings in quotes in the right context should also be taken as tokens unless otherwise stated.
// 
// - FudgetThemes: a static class that can be used to generate tokens or create new themes. Most other uses are considered
//      internal and should be avoided.
// 
// - FudgetStyle: (style for short) objects of this type can be used to query for resources (color, padding etc.) given a
//      theme. If a theme has resource for "DarkBackgroundColor", this token shouldn't be used directly, as the style might
//      not recognize it. Instead, a style might know of "ButtonBackground", which it identifies with "DarkBackgroundColor"
//      and returns the relevant resource from the theme.
// 
//      Styles can be inherited from other styles. This means that the inherited style will return the same resources for the
//      same tokens. It is also possible to override what resource is returned, by setting a 'resource override' or a 'value
//      override'.
//      A 'resource override' is overriding what token is looked up in the theme. A 'value override' stores the value for the
//      token directly in the style.
// 
//      Styles also can be used to access painter objects. This works similar to resources, as these are also stored in themes
//      and are accessed through tokens. Styles only support value override for painters. (for now?)
// 
// - FudgetElementPainter: (painter for short) Painters are types that can draw the whole surface or parts of a control.
//      Painters can use other painters for drawing. To make a painter, it needs to be derived from FudgetElementPainter, and
//      it has to override the Draw function. The draw function can use any logic to paint any type of control. (TODO: find a way
//      to connect painters and control behavior, which is to be implemented)
// 
//      When a control wants to draw itself, it should override its own Draw function, get the painter object by calling
//      GetElementPainter and call its Draw function. The painter will be looked up based on the full class name of the derived
//      control. The painter can be overriden in styles. Check the class FudgetSimpleButton for an example. (TODO: replace this
//      explanation when default controls are made and finalized. The original FudgetSimpleButton might not exist anymore)
// 
//      Painters might need to access the parent control to get an idea of its current state. For that, derive from the class
//      FudgetPainterPropertyProvider for each control. (This is not necessary if a control doesn't have any extra state beside
//      what it inherited)
// 
// - FudgetPainterPropertyProvider: Simple class to give the element painter information about the state of the control it is
//      painting. Beside values for the current state, property providers also have a DeltaTime property that painters can access
//      for animations. See an example of how the property provider works in SimpleButton.h and cpp, and how it is accessed in
//      SimpleButtonPainter.h and cpp.
// 
//      TODO: consider changing this class into an interface, or make every control provide its own properties. (This would make
//      them take more memory when a property provider is not necessary.)
// 
// 
// How to create a control to draw itself with styles:
// ---------
//      
//  1. Decide the stylable elements of the control: For example buttons can have a "background" (often same as the button's
//      surface), a "border", an "icon" or "text".
//
//  2. (optional) Inherit a style from the default style or from a style of an ancestor class. This style should be choosen
//      based on class hierarchy of controls using it, since they might already provide resources for "background" or "border".
//      The inherited style should be created with a token that matches the control's class name. It is possible to set a
//      different style for a constructed control, but it won't change the style for other controls of the same type.
//
// 3. (optional - if style was inherited) Add resource or value overrides to the inherited style.
//
// 4. Create a new painter class, which can be called by any name. This automatically registers the painter with FudgetThemes
//      by that name.
// 
// 5. Create the painter property provider. The property provider can return a value based on a token. For example a button
//      might have the value "pressed" with a token called "ButtonIsPressed". The painter that will be created will have to
//      know of the exact token name used. There's an example of this in SimpleButton.h / cpp and SimpleButtonPainter.h / cpp
// 
// 6. Add the painter to the PainterIds of the theme (or themes), by the token that matches the class name, and corresponds to
//      the painter's name. i.e. `theme->PainterIds[TEXT("FudgetCustomControl")] = new_painter_name_token`.
//
//      TODO: this step might be made automatic in the future? I don't yet see how, since different themes can use different
//      painters for the same control.
//
// 7. Implement the painter's draw function. It might be as simple as calling DrawWithPainter with existing painters, or can
//      have its own painting logic. Make sure to use the drawing functions of the control instead of Render2D, since those
//      are made relative to the control's coordinates. Any clipping or transformation should be popped from Render2D that
//      were added. TODO: make this unnecessary with guard objects and clipping/transform functions in control
// 
//      DrawWithPainter respects the current style, so it is possible to override the painter in the style called for example
//      "ButtonBackgroundPainter" and provide a different painter. (Such painter might not currently exist, this is just an
//      example.)
//
//
// How to override a control's appearance:
// ---------
//
// There are multiple options to do this. The simplest one is to inherit from the style used for the control's class, override
// its resources and set that new style to the control.
// 
// It's also possible to create an entirely new theme, by duplicating an existing theme, and changing values in the new theme.
// Setting the theme in a container will change it for every child control. The theme can be changed for a single control as
// well if it doesn't have child controls or is not a container.
// 
// Creating an entirely new painter and setting that painter in the theme for the control's class, or for the style as a painter
// override will allow controls to use new logic to paint themselves. Make sure this reflects the behavior, which is a TODO to
// experiment with
// 
// 
// How to know what tokens exist for what?
// ----------
// TODO: this will have to wait until we have created standard controls. I think the FudgetThemes class needs to have all the used
// tokens as public static fields (like it already does with the example tokens) with proper documentation what they do and what type
// of resource they represent.
// 
// Important note: the resources are stored in Variant (object in C#) which means they can hold anything, but painters won't be able
// to work with those unless they are of specific type in the Variant. This will need to be documented properly.


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

    API_FUNCTION() bool GetResource(FudgetToken token, API_PARAM(Out) Variant &result) const;
    API_FUNCTION() void SetResource(FudgetToken token, Variant value);
private:
    /// <summary>
    /// Creates a theme that is a duplicate of this theme. All values will be matching. This also means that if an object
    /// is stored as a pointer, changes on its properties will change the same object in other themes.
    /// </summary>
    /// <returns></returns>
    FudgetTheme* Duplicate() const;


    /// <summary>
    /// The values in this theme, that can be referenced by styles. Although it's not read-only, the values
    /// shouldn't be changed directly, because styles cache the values and they won't be refreshed.
    /// </summary>
    std::map<FudgetToken, Variant> _resources;

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
    //FudgetThemes();
    //~FudgetThemes();

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

    // Token for the base theme that's guaranteed to be present. "FudgetThemes_MainTheme"
    API_FIELD(ReadOnly) static const FudgetToken MainThemeToken;

    // Token to get the default style that holds basic values. "FudgetThemes_DefaultStyle"
    API_FIELD(ReadOnly) static const FudgetToken DefaultStyleToken;



    // Tokens for values that are common in many UI systems.  These are here to make it unnecessary to register them every
    // time a new style or theme is created from scratch, but they are not necessary to use and have no special meaning.
    // All tokens start with "FudgetThemes_" and have the same string value following it as their name, but excluding "Token".


    // Tokens for colors and images:

    /// <summary>
    /// Background color for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BackgroundColorToken;
    /// <summary>
    /// Background color for controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldColorToken;
    /// <summary>
    /// Background color for hovered controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldColorToken;
    /// <summary>
    /// Background color for focused controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldColorToken;
    /// <summary>
    /// Background color for disabled controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldColorToken;
    /// <summary>
    /// Border color for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderColorToken;
    /// <summary>
    /// Border color for hovered controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredBorderColorToken;
    /// <summary>
    /// Border color for focused controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedBorderColorToken;
    /// <summary>
    /// Border color for disabled controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledBorderColorToken;
    /// <summary>
    /// Border color for controls that have a separate outer border and show field values, like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldBorderColorToken;
    /// <summary>
    /// Border color for hovered controls that have a separate outer border and show field values, like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldBorderColorToken;
    /// <summary>
    /// Border color for focused controls that have a separate outer border and show field values, like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldBorderColorToken;
    /// <summary>
    /// Border color for disabled controls that have a separate outer border and show field values, like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldBorderColorToken;
    /// <summary>
    /// Dark color representing a theme.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DarkColorToken;
    /// <summary>
    /// Light color representing a theme.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken LightColorToken;
    /// <summary>
    /// Medium color representing a theme.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken MediumColorToken;
    /// <summary>
    /// Color representing a theme that has some special purpose. For example color of a special button.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken AccentColorToken;
    /// <summary>
    /// Background color  for selected items in controls with a neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectionColorToken;
    /// <summary>
    /// Background hovered for selected items in focused controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectionColorToken;
    /// <summary>
    /// Background color for selected items in focused controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectionColorToken;
    /// <summary>
    /// Background color  for selected items in disabled controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectionColorToken;
    /// <summary>
    /// Text color for controls in a neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken TextColorToken;
    /// <summary>
    /// Text color for hovered controls
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredTextColorToken;
    /// <summary>
    /// Text color for focused controls
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedTextColorToken;
    /// <summary>
    /// Text color for disabled controls
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledTextColorToken;
    /// <summary>
    /// Text color in fields for controls in a neutral state. For example text box text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldTextColorToken;
    /// <summary>
    /// Text color in fields for hovered controls. For example text box text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldTextColorToken;
    /// <summary>
    /// Text color in fields for focused controls. For example text box text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldTextColorToken;
    /// <summary>
    /// Text color in fields for disabled controls. For example text box text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldTextColorToken;
    /// <summary>
    /// Selected text color in fields for controls in a neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedTextColorToken;
    /// <summary>
    /// Selected text color in fields for hovered controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectedTextColorToken;
    /// <summary>
    /// Selected text color in fields for focused controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedTextColorToken;
    /// <summary>
    /// Selected text color in fields for disabled controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedTextColorToken;

    /// <summary>
    /// Color of the main area of the control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its hovered selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectedControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its disabled down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledDownControlColorToken;
    /// <summary>
    /// Color of the main area of the control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedControlColorToken;

    /// <summary>
    /// Color of the main area of a button control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledButtonColorToken;
    /// <summary>
    /// Color of the main area of a button control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedButtonColorToken;


    /// <summary>
    /// Background image for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BackgroundImageToken;
    /// <summary>
    /// Background image for controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldImageToken;
    /// <summary>
    /// Background image for hovered controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldImageToken;
    /// <summary>
    /// Background image for focused controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldImageToken;
    /// <summary>
    /// Background image for disabled controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldImageToken;
    /// <summary>
    /// Border image for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderImageToken;
    /// <summary>
    /// Border image for hovered controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredBorderImageToken;
    /// <summary>
    /// Border image for focused controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedBorderImageToken;
    /// <summary>
    /// Border image for disabled controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledBorderImageToken;
    /// <summary>
    /// Border image for controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldBorderImageToken;
    /// <summary>
    /// Border image for hovered controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldBorderImageToken;
    /// <summary>
    /// Border image for focused controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldBorderImageToken;
    /// <summary>
    /// Background image  for selected items in controls in a neutral sate.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectionImageToken;
    /// <summary>
    /// Background image for selected items in hovered controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectionImageToken;
    /// <summary>
    /// Background image for selected items in focused controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectionImageToken;
    /// <summary>
    /// Border image for disabled controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldBorderImageToken;
    /// <summary>
    /// Background image  for selected items in disabled controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectionImageToken;

    /// <summary>
    /// Image of the main area of the control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its hovered selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectedControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its disabled down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledDownControlImageToken;
    /// <summary>
    /// Image of the main area of the control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedControlImageToken;

    /// <summary>
    /// Image of the main area of a button control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledButtonImageToken;
    /// <summary>
    /// Image of the main area of a button control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedButtonImageToken;



    /// <summary>
    /// Background for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BackgroundDrawToken;
    /// <summary>
    /// Background for controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldDrawToken;
    /// <summary>
    /// Background for hovered controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldDrawToken;
    /// <summary>
    /// Background for focused controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldDrawToken;
    /// <summary>
    /// Background for disabled controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldDrawToken;
    /// <summary>
    /// Border for controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderDrawToken;
    /// <summary>
    /// Border draw area for hovered controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredBorderDrawToken;
    /// <summary>
    /// Border draw area for focused controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedBorderDrawToken;
    /// <summary>
    /// Border draw area for disabled controls that hold other controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledBorderDrawToken;
    /// <summary>
    /// Border for controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldBorderDrawToken;
    /// <summary>
    /// Border for hovered controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredFieldBorderDrawToken;
    /// <summary>
    /// Border for focused controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedFieldBorderDrawToken;
    /// <summary>
    /// Border for disabled controls that show field values like text.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledFieldBorderDrawToken;
    /// <summary>
    /// Background  for selected items in controls in a neutral sate.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectionDrawToken;
    /// <summary>
    /// Background for selected items in hovered controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectionDrawToken;
    /// <summary>
    /// Background for selected items in focused controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectionDrawToken;
    /// <summary>
    /// Background for selected items in disabled controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectionDrawToken;

    /// <summary>
    /// Draw area of the main area of the control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its hovered selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredSelectedControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its disabled down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledDownControlDrawToken;
    /// <summary>
    /// Draw area of the main area of the control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedControlDrawToken;


    /// <summary>
    /// Draw area of the main area of a button control in its neutral state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its pressed state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PressedButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its down state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its hovered state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HoveredButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its focused state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its focused selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FocusedSelectedButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SelectedButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its disabled state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledButtonDrawToken;
    /// <summary>
    /// Draw area of the main area of a button control in its disabled selected state.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DisabledSelectedButtonDrawToken;


    // Tokens for fonts:

    /// <summary>
    /// Standard font used on control surfaces
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ControlFontToken;
    /// <summary>
    /// Small font used on control surfaces
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken SmallControlFontToken;
    /// <summary>
    /// Large font used on control surfaces
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken LargeControlFontToken;
    /// <summary>
    /// Font used in headers
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HeaderFontToken;
    /// <summary>
    /// Standard font used in input fields
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldFontToken;


    // Tokens for padding:

    /// <summary>
    /// Padding inside a control to restrict the space of the contents. Controls might choose to use both control padding and field padding.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken ControlPaddingToken;
    /// <summary>
    /// Padding inside a control to restrict the background's draw rectangle.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BackgroundPaddingToken;
    /// <summary>
    /// Padding inside a control to restrict a border's draw rectangle.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderPaddingToken;
    /// <summary>
    /// Padding inside a control to restrict the space of field contents, like text. Controls might choose to use both control padding and field padding.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken FieldPaddingToken;


    // Tokens for general values:

    /// <summary>
    /// Time in seconds while a caret in text controls is shown or hidden.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken CaretBlinkTimeToken;
    /// <summary>
    /// Width of the caret used in text controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken CaretPixelWidthToken;
    /// <summary>
    /// Number of characters to scroll in view when a text box' caret is moved outside its rectangle.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken CharacterScrollCountToken;
    /// <summary>
    /// Horizontal alignment for fields or controls
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken HorizontalAlignmentToken;
    /// <summary>
    /// Vertical alignment for fields or controls
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken VerticalAlignmentToken;
    /// <summary>
    /// Generic value for left direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken LeftToken;
    /// <summary>
    /// Generic value for right direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken RightToken;
    /// <summary>
    /// Generic value for top direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken TopToken;
    /// <summary>
    /// Generic value for bottom direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BottomToken;
    /// <summary>
    /// Generic value for upwards direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken UpToken;
    /// <summary>
    /// Generic value for downwards direction or side.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken DownToken;
    /// <summary>
    /// Padding width for fields or controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PaddingWidthToken;
    /// <summary>
    /// Left padding value for fields or controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PaddingLeftToken;
    /// <summary>
    /// Right padding value for fields or controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PaddingRightToken;
    /// <summary>
    /// Top padding value for fields or controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PaddingTopToken;
    /// <summary>
    /// Bottom padding value for fields or controls.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken PaddingBottomToken;
    /// <summary>
    /// Generic value for border width.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderWidthToken;
    /// <summary>
    /// Generic value for left side border width.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderWidthLeftToken;
    /// <summary>
    /// Generic value for right side border width.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderWidthRightToken;
    /// <summary>
    /// Generic value for top side border width.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderWidthTopToken;
    /// <summary>
    /// Generic value for bottom side border width.
    /// </summary>
    API_FIELD(ReadOnly) static const FudgetToken BorderWidthBottomToken;


    /// <summary>
    /// Returns a token from the string that was used to create it. (Only the character sequence matters, not the
    /// actual variable) The same string always returns the same or equivalent token. To make a token use RegisterToken.
    /// </summary>
    /// <param name="token_name">String that generated the token</param>
    /// <returns>The token</returns>
    API_FUNCTION() static FudgetToken GetToken(String token_name);

    /// <summary>
    /// Returns the string that generated the token
    /// </summary>
    API_FUNCTION() static String GetTokenName(FudgetToken token);

    /// <summary>
    /// Registers a new token from a string. The string can be anything but it must contain characters and it's case sensitive.
    /// If duplicate_is_error is true, this function returns the FudgetToken::Invalid token for strings that have already been
    /// used to generate a token.
    /// </summary>
    /// <param name="token_name">String to use for generating the token</param>
    /// <param name="duplicate_is_error">Whether it's an error to pass the same string a second time</param>
    /// <returns>The token if the call was valid, otherwise FudgetToken::Invalid</returns>
    API_FUNCTION() static FudgetToken RegisterToken(String token_name, bool duplicate_is_error = false);

    /// <summary>
    /// Stores a font asset, that can be retrieved with GetFontAsset using the same token. The token must be unique among
    /// font assets.
    /// </summary>
    /// <param name="token">Token to associate with the font asset</param>
    /// <param name="asset">The font asset to store</param>
    /// <returns>Whether the asset was successfully stored. False if token was already taken</returns>
    API_FUNCTION() static bool RegisterFontAsset(FudgetToken token, FontAsset *asset);

    /// <summary>
    /// Returns a font asset associated with the given token, or null if no asset was found
    /// </summary>
    API_FUNCTION() static FontAsset* GetFontAsset(FudgetToken token);

    /// <summary>
    /// Tries to retrieve a theme for the token or null if no token was found.
    /// </summary>
    /// <param name="token">Token associated with the theme</param>
    /// <returns>The theme if found, or null</returns>
    API_FUNCTION() static FudgetTheme* GetTheme(FudgetToken token);

    /// <summary>
    /// Creates a copy of the theme if the source token represents a theme and the destination token is not taken already.
    /// Themes not created this way are not registered and can't be used.
    /// </summary>
    /// <param name="source_token">Token for theme to duplicate. This token must be for a valid theme</param>
    /// <param name="dest_token">Token of the new theme. This token cannot be taken by another theme</param>
    /// <returns>The theme created, or null if one of the tokens were invalid</returns>
    API_FUNCTION() static FudgetTheme* DuplicateTheme(FudgetToken source_token, FudgetToken dest_token);

    /// <summary>
    /// Creates and registers a new style object with a unique name. If the name is already taken, the result will
    /// be null. The style can be accessed with GetStyle later with the token matching the name. Styles with a
    /// name matching a control's full class name will be used for that control by default.
    /// </summary>
    /// <param name="token">Token for the style</param>
    /// <returns>The created style, or null if the token was invalid or used by another style</returns>
    static FudgetStyle* CreateStyle(String name);

    /// <summary>
    /// Creates and registers a new style object with a unique token. If the token is already taken, the result will
    /// be null. The style can be accessed with GetStyle later with the token. Styles with a name matching a control's
    /// full class name will be used for that control by default.
    /// </summary>
    /// <param name="token">Token for the style</param>
    /// <returns>The created style, or null if the token was invalid or used by another style</returns>
    API_FUNCTION(NoProxy) static FudgetStyle* CreateStyle(FudgetToken token);

    /// <summary>
    /// Tries to retrieve a style for the name. Styles with the name matching a control's full class name are used
    /// for that control by default.
    /// </summary>
    /// <param name="name">Token associated with the style</param>
    /// <returns>The style if found or null</returns>
    API_FUNCTION() static FudgetStyle* GetStyle(String name);

    /// <summary>
    /// Tries to retrieve a style for the token. Styles with the token matching a control's full class name are used
    /// for that control by default.
    /// </summary>
    /// <param name="token">Token associated with the style</param>
    /// <returns>The style if found or null</returns>
    API_FUNCTION() static FudgetStyle* GetStyle(FudgetToken token);

    /// <summary>
    /// Returns a style for the first matching token in the array. The tokens are usually coming from a control
    /// when it wants to get its class appropriate style. If no style is found for the tokens, the default style
    /// of the theme is returned.
    /// </summary>
    /// <param name="class_tokens">A chain of tokens that are looked up one by one until one matches a style</param>
    /// <returns>The style that matches one of the tokens or the default style of the theme</returns>
    API_FUNCTION() static FudgetStyle* GetControlStyleOrDefault(const Array<FudgetToken> &class_tokens);

#ifdef USE_EDITOR
    API_PROPERTY() static bool GetRuntimeUse() { return _runtime_use; }
    API_PROPERTY() static void SetRuntimeUse(bool value);
#endif

private:
    static std::map<String, FudgetToken> _token_map;
    static std::map<FudgetToken, String> _string_map;
    static int _highest_token;

    struct Data
    {
        // A collection of styles that provide values, like colors or floats to an element painter.
        std::map<FudgetToken, FudgetStyle*> _style_map;

        // A collection of themes. Each theme is a collection of data in itself to provide values that can be used by
        // styles and element painters.
        std::map<FudgetToken, FudgetTheme*> _theme_map;

        std::map<FudgetToken, FontAsset*> _font_asset_map;
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
