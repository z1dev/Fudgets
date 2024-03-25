#pragma once

// Documentation in progress. I update this a bit at a time
// 
// FudgetControls is the base of everything in Fudgets that need to have a presence on screen in the UI.
// Every other control type derives from this class. Controls can draw themselves with styles, update themselves
// on tick and, handle input. Controls with user interaction can have focus, but only one control at a time. This
// means that when a key is pressed on the keyboard, the one control will receive it and can react in some way.
// to be continued...
// 
// Creating controls:
// 
// Initialization:
// override GetInitFlags. call base
// 
// (WARNING: Styling might change a lot in the future!)
// Style:
// 1. Create enum or static const int ids required to read resources from styles. The values MUST be distinct from other ids.
// 2. Override `OnInitialize` for compound controls to create child controls.
// 3. Override `OnStyleInitialize` and read the resource for the ids into member variables. Sane defaults need
//    to be provided if the resource does not exist.
// 
// Drawing:
// 1. Create member variables for the different style painters if the control is using styles.  TODO: details
// 2. Override `OnDraw` and call the painters for the drawing
// 3. Add any custom painting 
// 
// Input:
// mouse down up move functions, keydown up, oncharinput, wants navigation keys
// 
// Data:
// use the protected _data_proxy to register with IFudgetDataProvider objects. It in turn calls the virtual Data*** functions
// in the control.


#include <map>

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Vector3.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Types/Span.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Serialization/Serialization.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/KeyboardKeys.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Platform/Base/WindowBase.h"

#include "Styling/StyleStructs.h"
#include "Styling/Style.h"
#include "Styling/Themes.h"

#include "DataInterfaces.h"


class FudgetContainer;
class FudgetGUIRoot;
class FudgetTheme;
class TextureBase;
struct SpriteHandle;
class MaterialBase;
struct TextLayoutOptions;
struct TextRange;
class GPUTexture;
class GPUTextureView;
class Font;
struct FudgetDrawArea;
struct FudgetPadding;
struct FudgetBorder;
struct FudgetFontSettings;
struct FudgetFont;
struct FudgetDrawColors;
class FudgetPartPainter;
struct FudgetDrawInstructionList;
class FudgetDrawable;
class FudgetControlDataConsumerProxy;

enum class FudgetVisualControlState : uint64;


/// <summary>
/// Used for any function call in controls and layouts that need one specific size of controls.
/// </summary>
API_ENUM()
enum class FudgetSizeType : uint8
{
    /// <summary>
    /// Specifies the hint size. The hint size is the size a container prefers to have if there's
    /// enough space. Unless the layout enforces it, the control should have this size independent
    /// of the available space
    /// </summary>
    Hint,
    /// <summary>
    /// The minimum suggested size which still makes use of the control possible. Layouts are allowed
    /// to ignore this when there isn't enough space. Negative values have the same effect as 0
    /// </summary>
    Min,
    /// <summary>
    /// The maximum suggested size for a control. Layouts might make a control grow larger than the
    /// hint size, but can use this size to limit that. Layouts are allowed to ignore this value based
    /// on a slot's settings or the layout's function.
    /// Negative values count as "infinite" size (near int limits.)
    /// </summary>
    Max,
};


/// <summary>
/// Indicates a change in a control that might cause a layout recalculation
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetLayoutDirtyReason : uint8
{
    /// <summary>
    /// A control's size changed
    /// </summary>
    Size = 1 << 0,
    /// <summary>
    /// A control's position changed in its parent
    /// </summary>
    Position = 1 << 1,
    /// <summary>
    /// A control's index changed in its parent
    /// </summary>
    Index = 1 << 2,

    /// <summary>
    /// The notification is the result of a container's change
    /// </summary>
    Container = 1 << 3,

    /// <summary>
    /// Both a control's size and position changed
    /// </summary>
    SizePos = Size | Position,

    /// <summary>
    /// Flag corresponding to size, position, and index
    /// </summary>
    All = Size | Position | Index
};
DECLARE_ENUM_OPERATORS(FudgetLayoutDirtyReason);

/// <summary>
/// Flags describing a control's current state
/// </summary>
API_ENUM(Attributes="Flags")
enum class FudgetControlState : uint16
{
    /// <summary>
    /// Empty state
    /// </summary>
    None = 0,
    /// <summary>
    /// Set after the OnInitialize function ran once
    /// </summary>
    Initialized = 1 << 0,
    /// <summary>
    /// Set when the styles were processed and the OnStyleInitialized function ran. This will be removed when moving
    /// the control to a different parent to make sure the theme is updated.
    /// </summary>
    StyleInitialized = 1 << 1,
    /// <summary>
    /// The control gets update notifies by having its Update function called
    /// </summary>
    Updating = 1 << 2,
    /// <summary>
    /// Enabled controls can be interacted with, they can get focus and might be drawn different to disabled. For a
    /// control to be enabled, it needs to be set to enabled and must not have a disabled parent in the hierarchy.
    /// </summary>
    Enabled = 1 << 3,
    /// <summary>
    /// Whether one of the parents of this control in the hierarchy are disabled.
    /// </summary>
    ParentDisabled = 1 << 4,
    /// <summary>
    /// True when _cached_global_to_local_translation is valid.
    /// </summary>
    Global2LocalCached = 1 << 5,
    /// <summary>
    /// Whether the mouse input is currently captured by this control
    /// </summary>
    MouseIsCaptured = 1 << 6,
    /// <summary>
    // Set during layout phase when the control's position was changed by a layout.
    /// </summary>
    PositionUpdated = 1 << 7,
    /// <summary>
    // Set during layout phase when the control's size was changed by a layout.
    /// </summary>
    SizeUpdated = 1 << 8,
    /// <summary>
    /// The control is not drawn, can't be focused and won't react to user input. It will receive some events
    /// that are not related to these. If added in a layout, it'll appear as empty space where the control would be.
    /// </summary>
    Invisible = 1 << 9,
    /// <summary>
    /// The control is not drawn, can't be focused and won't react to user input. It won't receive most events.
    /// It won't appear in layouts and won't take up any space.
    /// </summary>
    Hidden = 1 << 10,
    /// <summary>
    /// A parent container up the chain is hidden which prevents drawing and user interaction.
    /// </summary>
    ParentHidden = 1 << 11,
    /// <summary>
    /// The control has a painter created to draw its frame.
    /// </summary>
    FrameCreated = 1 << 12,
};
DECLARE_ENUM_OPERATORS(FudgetControlState);

/// <summary>
/// Visibility of a control determines both whether it's shown in the UI and if it takes up space when hidden.
/// </summary>
API_ENUM()
enum class FudgetControlVisibility
{
    /// <summary>
    /// The control is drawn, can be focused and can react to user input. It'll receive all relevant events. It will
    /// be positioned in layouts and take up space.
    /// </summary>
    Visible,
    /// <summary>
    /// The control is not drawn, can't be focused and won't react to user input. It will receive some events
    /// that are not related to these. If added in a layout, it'll appear as empty space where the control would be.
    /// </summary>
    Invisible,
    /// <summary>
    /// The control is not drawn, can't be focused and won't react to user input. It won't receive most events.
    /// It won't appear in layouts and won't take up space. Also the control's OnUpdate will be disabled as well.
    /// </summary>
    Hidden
};


/// <summary>
/// Flags for controls that describe their behavior
/// </summary>
API_ENUM(Attributes="Flags")
enum class FudgetControlFlag : uint32
{
    /// <summary>
    /// Empty flag
    /// </summary>
    None = 0,
    /// <summary>
    /// Controls with this flag will refresh their flags from GetInitFlags when added to a parent. By default,
    /// controls start with this flag and nothing else. Make sure to override GetInitFlags to set the proper flags.
    /// </summary>
    ResetFlags = 1 << 0,
    /// <summary>
    /// FudgetContainers will add this flag themselves for derived classes, while other classes should
    /// never set this. Don't change this flag in a running game.
    /// </summary>
    ContainerControl = 1 << 1,
    /// <summary>
    /// For containers that hold other controls that together behave like a single control. The main container is
    /// the face of the control, and should expose every property that can be changed on it and its contents.
    /// Ignored for non-container controls.
    /// </summary>
    CompoundControl = 1 << 2,
    /// <summary>
    /// Supports mouse button up and down inputs. This should be set for any control that might not react
    /// to mouse events by default but is not transparent. Controls can decide to handle the events per call.
    /// </summary>
    CanHandleMouseUpDown = 1 << 3,
    /// <summary>
    /// Supports mouse move inputs. This can be set for any control that might not react to mouse events by
    /// default, but is not transparent. The top control with this flag will be the only one receiving the
    /// event if it's under the mouse.
    /// </summary>
    CanHandleMouseMove = 1 << 4,
    /// <summary>
    /// Supports mouse enter and mouse leave events, but only while CanHandleMouseMove is also set.
    /// </summary>
    CanHandleMouseEnterLeave = 1 << 5,
    /// <summary>
    /// Makes the control "eat" all mouse messages if the mouse is over it, even if it does not have any of the
    /// other flags for mouse events. Ignored while the mouse is captured by another control. Mouse hooks can
    /// still peek and modify the mouse events before the control can access them.
    /// </summary>
    BlockMouseEvents = 1 << 6,
    /// <summary>
    /// Set the input focus for keyboard on the control when pressing the left mouse button over it. Ignored
    /// while another control is capturing the mouse input.
    /// </summary>
    FocusOnMouseLeft = 1 << 7,
    /// <summary>
    /// Set the input focus for keyboard on the control when pressing the right mouse button over it. Ignored
    /// while another control is capturing the mouse input.
    /// </summary>
    FocusOnMouseRight = 1 << 8,
    /// <summary>
    /// Set the mouse capture on the control when the left button is pressed down, and release it when the
    /// same button is released.
    /// </summary>
    CaptureReleaseMouseLeft = 1 << 9,
    /// <summary>
    /// Set the mouse capture on the control when the right button is pressed down, and release it when the
    /// same button is released.
    /// </summary>
    CaptureReleaseMouseRight = 1 << 10,
    /// <summary>
    /// Controls with this flag will have their OnUpdate function called, by registering them automatically with
    /// RegisterToUpdate(true). Removing the flag with SetControlFlags will also unregister them from updates.
    /// </summary>
    RegisterToUpdates = 1 << 11,
    /// <summary>
    /// Only for top-level controls which are direct children of the gui root. It's ignored for others. Places
    /// the top-level control above other top-level controls that don't have the AlwaysOnTop flag. Providing the
    /// flag in GetInitFlags or changing it in SetControlFlags will change the control's position in the root, but
    /// only for children of the root. It's best to call SetAlwaysOnTop instead after adding control to the root.
    /// </summary>
    AlwaysOnTop = 1 << 12,
    /// <summary>
    /// Supports key presses and releases, and character input. Normally the focused control receives these events,
    /// if it has this flag set. Without this flag, the nearest parent will be notified of the event that has the flag.
    /// </summary>
    CanHandleKeyEvents = 1 << 13,
    /// <summary>
    /// Only considered if CanHandleKeyEvents is also set. If not set, the control won't receive key down and up events
    /// on navigation key (usually arrow and tab key) presses, and instead let the UI use them to change which
    /// control has the focus. If set, the control's WantsNavigationKey function is called first to decide if it wants to
    /// get the key up and down events.
    /// </summary>
    CanHandleNavigationKeys = 1 << 14,
    /// <summary>
    /// The control draws a background and displays a frame inside its edges, its contents can be constrainted within this
    /// frame. When it is set, GetFramePadding returns valued paddings.
    /// TODO: make the layouts and controls respect the frame's padding automatically.
    /// </summary>
    Framed = 1 << 15,

};
DECLARE_ENUM_OPERATORS(FudgetControlFlag);

/// <summary>
/// Which size, if any, will be provided by the control using these values. Controls that can calculate their own sizes will
/// override the Get***Size functions to change the width, height or both of the result.
/// </summary>
API_ENUM()
enum class FudgetAutoSizing
{
    /// <summary>
    /// No size is automatic
    /// </summary>
    None,
    /// <summary>
    /// Width is calculated by the control
    /// </summary>
    Width,
    /// <summary>
    /// Height is calculated by the control
    /// </summary>
    Height,
    /// <summary>
    /// Both dimensions are calculated by the control
    /// </summary>
    Both
};

/// <summary>
/// Types of mouse and touch inputs a control can handle.
/// </summary>
API_ENUM()
enum class FudgetPointerInput
{
    /// <summary>
    /// Mouse button pressed
    /// </summary>
    MouseDown,
    /// <summary>
    /// Mouse button released
    /// </summary>
    MouseUp,
    /// <summary>
    /// Mouse button double clicked
    /// </summary>
    MouseDoubleClick,
    /// <summary>
    /// Mouse movement over a control
    /// </summary>
    MouseMove,
    /// <summary>
    /// First mouse event after the mouse enters a control
    /// </summary>
    MouseEnter,
    /// <summary>
    /// Last mouse event after the mouse leaves a control
    /// </summary>
    MouseLeave,
};

/// <summary>
/// A set of options to decide what should happen when the mouse button was pressed over a control. 
/// </summary>
API_ENUM()
enum class FudgetInputResult
{
    /// <summary>
    /// The mouse or key press event should be ignored. For mouse events, this won't let the event pass through
    /// to the control below, but will prevent any automatic handling, like changing the focused control.
    /// For key events, this will let the system handle the input as if no control received it.
    /// </summary>
    Ignore,
    /// <summary>
    /// The mouse or key press event should be ignored for the control. The event processing will continue
    /// like the control didn't have the right flag to handle the event, and the event will be posted to the
    /// control below that expects the input event.
    /// </summary>
    PassThrough,
    /// <summary>
    /// The control used the mouse or key event, preventing it to be sent to controls below it. For mouse events this
    /// will let the control take focus if it has the right flag. For key events, the control takes the responsibility
    /// of dealing with the input, preventing the system to handle it.
    /// </summary>
    Consume
};

/// <summary>
/// Base class for controls, including containers and panels.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetControl : public ScriptingObject, public ISerializable
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetControl);
public:
    ~FudgetControl();

    /// <summary>
    /// Called the first time the control is added to a parent. Derived types might need to call their base implementation.
    /// </summary>
    API_FUNCTION() virtual void OnInitialize() {}

    /// <summary>
    /// Called when the control's style is first accessed, or after the style changes. Derived types might need to call the
    /// base implementation.
    /// </summary>
    API_FUNCTION() virtual void OnStyleInitialize() {}

    /// <summary>
    /// Draws the control's surface. Derived controls can call Render2D methods here.
    /// </summary>
    API_FUNCTION() virtual void OnDraw() {}

    /// <summary>
    /// Called when the control's enabled state changed. Not called when a parent container became enabled/disabled, and might not cause
    /// a change in the appearance and behavior of the control. If the appearance changes as well due to this change, OnVirtuallyEnabledChanged
    /// is called after this function returns.
    /// </summary>
    API_FUNCTION() virtual void OnEnabledChanged() {}

    /// <summary>
    /// Called when the control's virtual enabled state changed. This can mean a parent became enabled/disabled that might make this control
    /// appear enabled/disabled as well.
    /// </summary>
    API_FUNCTION() virtual void OnVirtuallyEnabledChanged() {}


    /// <summary>
    /// Called when the keyboard input focus changes from one control to another. It is called both on the
    /// control losing focus and the control gaining focus before the change happens.
    /// Make sure to call the base implementation upon receiving this event.
    /// </summary>
    /// <param name="focused">Whether this control gains the focus or not</param>
    /// <param name="other">The control which loses or gains the focus. Can be null</param>
    API_FUNCTION() virtual void OnFocusChanging(bool focused, FudgetControl *other) {}

    /// <summary>
    /// Called when the keyboard input focus changes from one control to another. It is called both on the
    /// control losing focus and the control gaining focus after the change happened.
    /// Make sure to call the base implementation upon receiving this event.
    /// </summary>
    /// <param name="focused">Whether this control gained the focus or not</param>
    /// <param name="other">The control which lost or gained the focus. Can be null</param>
    API_FUNCTION() virtual void OnFocusChanged(bool focused, FudgetControl *other) {}

    /// <summary>
    /// Called when a control or its parent became visible, making it appear in the UI.
    /// </summary>
    API_FUNCTION() virtual void OnShow() {}

    /// <summary>
    /// Called when a control or its parent became hidden or invisible. Both hidden and invisible controls don't
    /// show up in the UI, but invisible controls still take up space in their layout.
    /// </summary>
    API_FUNCTION() virtual void OnHide() {}

    /// <summary>
    /// Called when the control starts capturing the mouse input.
    /// </summary>
    API_FUNCTION() virtual void OnMouseCaptured() {}
    /// <summary>
    /// Called when the control stops capturing the mouse input.
    /// </summary>
    API_FUNCTION() virtual void OnMouseReleased() {}

    /// <summary>
    /// Called when the mouse cursor was in a call to SetCursor. Don't set the cursor directly as a response to this
    /// call to avoid infinite recursion.
    /// </summary>
    API_FUNCTION() virtual void OnCursorChanged() {}

    /// <summary>
    /// Called after the control's position changed in its parent. This might be due to direct change of the control's
    /// position, or because of layout changes. In the latter case OnSizeChanged might be called right after if the
    /// size was also changed by the layout, and then OnPositionOrSizeChanged is called as well.
    /// </summary>
    API_FUNCTION() virtual void OnPositionChanged() {}

    /// <summary>
    /// Called after the control's size changed. This is always called before drawing and after the control was laid
    /// out on the same frame. OnPositionChanged might have been called right before if the position was also changed
    /// by the layout, and then OnPositionOrSizeChanged is called as well.
    /// </summary>
    API_FUNCTION() virtual void OnSizeChanged() {}

    /// <summary>
    /// Called after the control's position or size changed due to being laid out in a parent. This is not called after
    /// manual changes to a control's position. When this function is called, OnPositionChanged and OnSizeChanged have
    /// already been called if the position or size changed.
    /// </summary>
    /// <param name="position">The position was changed by the parent layout</param>
    /// <param name="size">The size was changed by the parent layout</param>
    API_FUNCTION() virtual void OnPositionOrSizeChanged(bool position, bool size) {}

    /// <summary>
    /// Fetches the parent who is managing this control. The parent is also responsible for its destruction.
    /// In case the parent has a layout, it might set the size and position of this control as well.
    /// </summary>
    /// <returns>The parent managing this control's dimensions and lifetime.</returns>
    API_PROPERTY() FudgetContainer* GetParent() const { return _parent; }

    /// <summary>
    /// Changes the parent who is managing this control. The parent is also responsible for its destruction.
    /// In case the parent has a layout, it might set the size and position of this control as well.
    /// </summary>
    /// <param name="value">The new parent or null</param>
    API_PROPERTY() void SetParent(FudgetContainer *value);

    /// <summary>
    /// Changes the parent who is managing this control. The parent is also responsible for its destruction.
    /// In case the parent has a layout, it might set the size and position of this control as well.
    /// </summary>
    /// <param name="value">The new parent or null</param>
    /// <param name="index">The order inside the parent that determines when drawing and callbacks are called.</param>
    API_FUNCTION() void SetParent(FudgetContainer *value, int index);

    /// <summary>
    /// Gets the control's order among other controls in its parent. The index determines the control's drawing order
    /// and which control's methods are called first. The control with the highest index is drawn last, covering others below.
    /// </summary>
    /// <returns>The order inside the parent that determines when drawing and callbacks are called.</returns>
    API_PROPERTY() int GetIndexInParent() const { return _index; }

    /// <summary>
    /// Sets the control's order among other controls in its parent. The index determines the control's drawing order
    /// and which control's methods are called first. The control with the highest index is drawn last, covering others below.
    /// </summary>
    /// <param name="value">The requested order in the parent. Invalid values result in no change.</param>
    API_PROPERTY() void SetIndexInParent(int value);

    /// <summary>
    /// A custom name that can be used to identify the control. Names don't have to be unique.
    /// </summary>
    /// <returns>Name of the control</returns>
    API_PROPERTY() String GetName() const { return _name; }

    /// <summary>
    /// Set a custom name that can be used to identify the control. Names don't have to be unique.
    /// </summary>
    /// <param name="value">The control's new name</param>
    API_PROPERTY() void SetName(String value);

    /// <summary>
    /// True if the control is in a UI hierarchy, created in the game scene and the game is currently running.
    /// </summary>
    API_PROPERTY() bool IsInRunningGame() const;

    // Size and position

    /// <summary>
    /// The size preferred by the control that serves as a hint to the layout, which might ignore it if it doesn't fit.
    /// The standard controls should offer a sane value by default, but it can be changed by modifying the HintSize
    /// property (C#) or calling SetHintSize(C++).
    /// </summary>
    /// <returns>The size requested by the control or its user. Not always same as the actual size.</returns>
    API_PROPERTY() virtual Int2 GetHintSize() const { return _hint_size; }

    /// <summary>
    /// The size preferred by the control that serves as a hint to the layout, which might ignore it if it doesn't fit.
    /// The standard controls should offer a sane value by default.
    /// </summary>
    /// <param name="value">The preferred size for the control.</param>
    API_PROPERTY() virtual void SetHintSize(Int2 value);

    /// <summary>
    /// The size preferred by the control for layouting purposes. This is different from GetHintSize as it can differ
    /// from the value set by the user. The default implementation returns the same value as GetHintSize, but it can be
    /// overriden in derived classes that need to calculate a different size. This value might not be valid until the
    /// parent layout is finalized.
    /// </summary>
    /// <returns>The size requested by the control from the parent's layout</returns>
    API_PROPERTY(Attributes="HideInEditor, NoSerialize") virtual Int2 GetLayoutHintSize() const { return _hint_size; }

    /// <summary>
    /// Gets the minimum size the control can be resized to while still showing its contents. The control might become
    /// smaller than this size if the layout it is in doesn't have enough space.
    /// 
    /// </summary>
    /// <returns>The minimum size the control needs to show its contents</returns>
    API_PROPERTY() virtual Int2 GetMinSize() const { return _min_size; }

    /// <summary>
    /// Sets the minimum size for the control which is a hint for the layout when it has to compress its controls.
    /// </summary>
    /// <param name="value">The new minimum size.</param>
    API_PROPERTY() virtual void SetMinSize(Int2 value);

    /// <summary>
    /// The smallest size the control can fit in for layouting purposes. This is different from GetMinSize as it can
    /// differ from the value set by the user. The default implementation returns the same value as GetMinSize, but it
    /// can be overriden in derived classes that need to calculate a different size. This value might not be valid until
    /// the parent layout is finalized.
    /// </summary>
    /// <returns>The minimum size requested by the control from the parent's layout</returns>
    API_PROPERTY(Attributes = "HideInEditor, NoSerialize") virtual Int2 GetLayoutMinSize() const { return _min_size; }

    /// <summary>
    /// Gets the size that determines how big a control can grow at most. The control might become larger
    /// than this size if the layout it is in doesn't respect the property.
    /// </summary>
    /// <returns>The maximum size the control is allowed to have</returns>
    API_PROPERTY() virtual Int2 GetMaxSize() const { return _max_size; }

    /// <summary>
    /// Gets the size that determines how big a control can grow at most. The control might become larger
    /// than this size if the layout it is in doesn't respect the property.
    /// </summary>
    /// <returns>The maximum size the control is allowed to have</returns>
    API_PROPERTY() virtual void SetMaxSize(Int2 value);

    /// <summary>
    /// The largest size the control needs for layouting purposes. This is different from GetMaxSize as it can
    /// differ from the value set by the user. The default implementation returns the same value as GetMaxSize, but it
    /// can be overriden in derived classes that need to calculate a different size. This value might not be valid until
    /// the parent layout is finalized.
    /// </summary>
    /// <returns>The maximum size requested by the control from the parent's layout</returns>
    API_PROPERTY(Attributes = "HideInEditor, NoSerialize") virtual Int2 GetLayoutMaxSize() const { return _max_size; }

    /// <summary>
    /// Gets the size that the control currently occupies in its parent's layout. Be aware that this might result in
    /// layout recalculation of some parents in the tree. This value can't be directly modified. Use SetHintSize to
    /// change it.
    /// </summary>
    /// <returns>The current size of the control.</returns>
    API_PROPERTY() virtual Int2 GetSize() const;

    /// <summary>
    /// Gets the position of this control in its parent's layout, relative to the top-left corner of the parent's
    /// contents' area (inside padding). Be aware that this might result in layout recalculation of some parents in
    /// the tree.
    /// </summary>
    /// <returns>The control's current position</returns>
    API_PROPERTY() Int2 GetPosition() const;

    /// <summary>
    /// Modifies the position of the control in its parent's layout, relative to the top-left corner if
    /// the layout allows free positioning.
    /// </summary>
    /// <param name="value">The requested new position</param>
    API_PROPERTY() virtual void SetPosition(Int2 value);

    /// <summary>
    /// The position of the control as set in SetPosition or in the last layout frame. Calling this is safe from
    /// most contexts but might result in an outdated value.
    /// </summary>
    API_PROPERTY() virtual Int2 GetHintPosition() const { return _pos; }

    /// <summary>
    /// Gets the current width the control takes up in its parent's  layout.
    /// </summary>
    /// <returns>The current width of the control</returns>
    API_PROPERTY() virtual int GetWidth() const { return GetSize().X; }

    /// <summary>
    /// Gets the current height the control takes up in its parent's  layout.
    /// </summary>
    /// <returns>The current height of the control</returns>
    API_PROPERTY() virtual int GetHeight() const { return GetSize().Y; }

    /// <summary>
    /// Gets the local bounding rectangle of the control relative to its parent, calculating the size if necessary.
    /// </summary>
    API_PROPERTY() Rectangle GetBoundsInParent() const { return Rectangle(Float2(GetPosition()), Float2(GetSize())); }

    /// <summary>
    /// Gets the bounding rectangle of the control relative to itself, calculating the size if necessary. The
    /// top-left corner of the rectangle is always at zero coordinates.
    /// </summary>
    API_PROPERTY() Rectangle GetBounds() const { return Rectangle(Float2(0.f), Float2(GetSize())); }

    /// <summary>
    /// Gets the left side's coordinate of this control in its parent's layout, relative to the top-left corner.
    /// </summary>
    /// <returns>The X component of the position</returns>
    API_PROPERTY() virtual int GetLeft() const { return _pos.X; }

    /// <summary>
    /// Gets the top side's coordinate of this control in its parent's layout, relative to the top-left corner.
    /// </summary>
    /// <returns>The Y component of the position</returns>
    API_PROPERTY() virtual int GetTop() const { return _pos.Y; }

    /// <summary>
    /// Gets the right side's coordinate of this control in its parent's layout, relative to the top-left corner.
    /// </summary>
    /// <returns>The coordinate of the control's right side</returns>
    API_PROPERTY() int GetRight() const { return GetLeft() + GetWidth(); }

    /// <summary>
    /// Gets the bottom side's coordinate of this control in its parent's layout, relative to the top-left corner.
    /// </summary>
    /// <returns>The coordinate of the control's bottom side</returns>
    API_PROPERTY() int GetBottom() const { return GetTop() + GetHeight(); }

    /// <summary>
    /// Whether the control might change any of its requested sizes depending on the available space. For example when
    /// calculating word wrapping.
    /// </summary>
    API_PROPERTY() virtual bool SizeDependsOnSpace() const { return false; }

    /// <summary>
    /// Reports sizes of this control to the parent's layout based on available space. This function may be called
    /// several times during layout calculation and it must provide consistent results.
    /// Controls that have different sizes if the available space changes (as returned by SizeDependsOnSpace) need
    /// to report the same results for the same space. This result should be below the available space if it's
    /// possible.
    /// If any dimension of the available space is negative, the control is unrestricted in that dimension.
    /// </summary>
    /// <param name="available">Available space in the parent layout or negative if the space is not restricted.</param>
    /// <param name="wanted">The dimensions the control requests that's enough to fit it properly</param>
    /// <param name="min_size">The smallest size in either direction the control can still fit in.</param>
    /// <param name="max_size">The largest size that might be useful for the control.</param>
    /// <returns>Whether the control (or its contents in case of container) would return true when calling SizeDependsOnSpace</returns>
    API_FUNCTION() virtual bool OnMeasure(Int2 available, API_PARAM(Out) Int2 &wanted, API_PARAM(Out) Int2 &min_size, API_PARAM(Out) Int2 &max_size);

    /// <summary>
    /// Marks layouts dirty when the control changes in a way that influences its size.
    /// </summary>
    API_FUNCTION() virtual void SizeModified();

    /// <summary>
    /// Marks layouts dirty when the control changes in a way that influences its position.
    /// </summary>
    API_FUNCTION() virtual void PositionModified();

    /// <summary>
    /// Returns whether the control is a direct child control of the GUI root and is always placed above not
    /// AlwaysOnTop controls.
    /// </summary>
    API_PROPERTY() bool GetAlwaysOnTop() const;
    /// <summary>
    /// For controls that are direct child to the GUI root, sets whether the control should always be placed
    /// above not AlwaysOnTop controls. Call after adding control to the root.
    /// </summary>
    /// <param name="valuse">Set or unset the flag</param>
    API_PROPERTY() void SetAlwaysOnTop(bool value);

    /// <summary>
    /// Notification about parent change, when the control is added to a container or removed from one.
    /// </summary>
    /// <param name="old_parent">The previous parent which can be null</param>
    API_PROPERTY() virtual void OnParentChanged(FudgetContainer *old_parent) {}

    /// <summary>
    /// Called before the root changes. The control might not be initialized yet at this point.
    /// </summary>
    /// <param name="new_root">The root to be set to the control</param>
    API_FUNCTION() virtual void OnRootChanging(FudgetGUIRoot *new_root) {}
    /// <summary>
    /// Called after the root changes and the control was initialized, if it's the first time. This is called
    /// before OnParentChanged.
    /// </summary>
    /// <param name="old_root">The previous root control</param>
    API_FUNCTION() virtual void OnRootChanged(FudgetGUIRoot *old_root) {}

    // Update callback

    /// <summary>
    /// Called on each frame if the control is registered to receive events
    /// </summary>
    /// <param name="delta_time">The time passed since the last update</param>
    API_FUNCTION() virtual void OnUpdate(float delta_time) {}

    /// <summary>
    /// Registers or unregisters the control to receive the global update tick.
    /// </summary>
    /// <param name="value">Register on true and unregister on false</param>
    API_FUNCTION() void RegisterToUpdate(bool value);

    /// <summary>
    /// Whether the control is executing its OnUpdate after registering it with RegisterToUpdate.
    /// </summary>
    API_PROPERTY() bool IsUpdateRegistered() const { return (_state_flags & FudgetControlState::Updating) == FudgetControlState::Updating; }

    // Point transformation

    /// <summary>
    /// Converts a coordinate from local control space to global UI space.
    /// </summary>
    /// <param name="local">The coordinate relative to the top-left corner of the control</param>
    /// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
    API_FUNCTION() virtual Float2 LocalToGlobal(Float2 local) const;

    /// <summary>
    /// Converts a coordinate from global UI space to local control space.
    /// </summary>
    /// <param name="global">The coordinate relative to the top-left corner of the UI screen</param>
    /// <returns>The coordinate relative to the top-left corner of the control</returns>
    API_FUNCTION() virtual Float2 GlobalToLocal(Float2 global) const;

    /// <summary>
    /// Converts a rectangle from local control space to global UI space.
    /// </summary>
    /// <param name="local">The rectangle with a location relative to the top-left corner of the control</param>
    /// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
    API_FUNCTION() virtual Rectangle LocalToGlobal(const Rectangle &local) const;

    /// <summary>
    /// Converts a rectangle from global UI space to local control space.
    /// </summary>
    /// <param name="global">TThe rectangle with a location relative to the top-left corner of the UI screen</param>
    /// <returns>The coordinate relative to the top-left corner of the control</returns>
    API_FUNCTION() virtual Rectangle GlobalToLocal(const Rectangle &global) const;

    /// <summary>
    /// Calculates and saves the difference between global to local and stores it. This is necessary before the first
    /// CachedGlobalToLocal or CachedLocalToGlobal call on each draw frame, but calling it once per frame is enough. Does
    /// nothing if this value has already been saved. The calculated value is invalidated at the start of each Draw but
    /// it can also be invalidated with InvalidateGlobalToLocalCache
    /// </summary>
    API_FUNCTION() void CacheGlobalToLocal();

    /// <summary>
    /// Discards the result of the precalculated global to local difference, that was calculated in CacheGlobalToLocal.
    /// Calling CacheGlobalToLocal will calculate the value again, which is necessary before the first call to
    /// CachedGlobalToLocal or CachedLocalToGlobal.
    /// Avoid calling this function if unnecessary, because it will force recalculation of the cached difference. 
    /// </summary>
    API_FUNCTION() void InvalidateGlobalToLocalCache();

    /// <summary>
    /// Converts a coordinate from local control space to global UI space. Can be used multiple times after
    /// CacheGlobalToLocal was called once on the current draw frame.
    /// </summary>
    /// <param name="local">The coordinate relative to the top-left corner of the control</param>
    /// <param name="offset">Offset added to the resulting global coordinates.</param>
    /// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
    API_FUNCTION() virtual Float2 CachedLocalToGlobal(Float2 local, Float2 offset = Float2::Zero) const;

    /// <summary>
    /// Converts a coordinate from global UI space to local control space. Can be used multiple times after
    /// CacheGlobalToLocal was called once on the current draw frame.
    /// </summary>
    /// <param name="global">The coordinate relative to the top-left corner of the UI screen</param>
    /// <param name="offset">Offset added to the resulting local coordinates.</param>
    /// <returns>The coordinate relative to the top-left corner of the control</returns>
    API_FUNCTION() virtual Float2 CachedGlobalToLocal(Float2 global, Float2 offset = Float2::Zero) const;

    /// <summary>
    /// Converts a rectangle from local control space to global UI space. Can be used multiple times after
    /// CacheGlobalToLocal was called once on the current draw frame.
    /// </summary>
    /// <param name="local">The rectangle with a location relative to the top-left corner of the control</param>
    /// <param name="offset">Offset added to the resulting global coordinates of the rectangle's location.</param>
    /// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
    API_FUNCTION() virtual Rectangle CachedLocalToGlobal(const Rectangle &local, Float2 offset = Float2::Zero) const;

    /// <summary>
    /// Converts a rectangle from global UI space to local control space. Can be used multiple times after
    /// CacheGlobalToLocal was called once on the current draw frame.
    /// </summary>
    /// <param name="global">TThe rectangle with a location relative to the top-left corner of the UI screen</param>
    /// <param name="offset">Offset added to the resulting local coordinates of the rectangle's location.</param>
    /// <returns>The coordinate relative to the top-left corner of the control</returns>
    API_FUNCTION() virtual Rectangle CachedGlobalToLocal(const Rectangle &global, Float2 offset = Float2::Zero) const;

    // Control flags

    /// <summary>
    /// Gets flags describing the control's behavior or appearance.
    /// </summary>
    /// <returns>The flags set for the control</returns>
    API_PROPERTY(Attributes="HideInEditor") virtual FudgetControlFlag GetControlFlags() const;

    /// <summary>
    /// Sets flags describing the control's behavior or appearance.
    /// TODO: this has never been tested. Remove if proves to be useless
    /// </summary>
    /// <param name="flags">The new flags</param>
    API_PROPERTY() virtual void SetControlFlags(FudgetControlFlag flags);

    /// <summary>
    /// Matches the given flags with those of the control, returning true only if all the flags were found.
    /// </summary>
    /// <param name="flags">The flags to look for</param>
    /// <returns>True  if all the flags were found on the control</returns>
    API_FUNCTION() virtual bool HasAllFlags(FudgetControlFlag flags) const;

    /// <summary>
    /// Matches the given flags with those of the control, returning true if any of the flags was found.
    /// </summary>
    /// <param name="flags">The flags to look for</param>
    /// <returns>True if at least one flag was found on the control</returns>
    API_FUNCTION() virtual bool HasAnyFlag(FudgetControlFlag flags) const;

    /// <summary>
    /// Indicates whether the control has a visible frame. Controls with a frame draw the frame inside their bounds and
    /// constrain their contents within this frame. The frame drawing depends on the style of the frame and the control's
    /// style and theme. GetFramePadding determines the size of the inner padding.
    /// </summary>
    /// <returns>The current frame type</returns>
    API_PROPERTY() FORCE_INLINE bool GetFramed() const { return HasAnyFlag(FudgetControlFlag::Framed); }

    /// <summary>
    /// Sets whether the control has a visible frame. Controls with a frame draw the frame inside their bounds and
    /// constrain their contents within this frame. The frame drawing depends on the style of the frame and the
    /// control's style and theme. The control's styles must reference a valid DrawablePainter in the theme with the
    /// FudgetFramedControlPartIds::FramePainter id.
    /// GetFramePadding determines the size of the inner padding.
    /// </summary>
    /// <param name="value">The type of the frame to set</param>
    API_PROPERTY() void SetFramed(bool value);

    // Input:

    /// <summary>
    /// Called when a mouse button was pressed over this control. The control can start capturing the mouse
    /// with StartMouseCapture to be sure to receive OnMouseUp and OnMouseMove even when the mouse pointer is
    /// not over it.
    /// The control needs to have the CanHandleMouseUpDown flag. If CaptureReleaseMouse* flags are set, the
    /// mouse will be automatically captured on the corresponding mouse input.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just pressed</param>
    /// <param name="double_click">Whether the call is the result of double clicking</param>
    /// <returns>One of the result options that decide how to deal with the mouse event</returns>
    API_FUNCTION() virtual FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) { return FudgetInputResult::Consume; }

    /// <summary>
    /// Called when a mouse button was released over this control. The control should stop capturing the mouse
    /// with StopMouseCapture if it started capturing it on OnMouseDown for the same button. Return true if the
    /// control wants to prevent other controls below to get the mouse event. This should mirror the result of
    /// OnMouseDown.
    /// The control needs to have the CanHandleMouseUpDown flag. If CaptureReleaseMouse* flags are
    /// set, the mouse will be automatically released on the corresponding mouse input.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just released</param>
    /// <returns>Prevent OnMouseUp reaching other controls or not</returns>
    API_FUNCTION() virtual bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) { return true; }

    /// <summary>
    /// Called during mouse event handling to make sure this control wants to handle mouse events at
    /// the passed position. This can be used for controls with transparent pixels to ignore the mouse
    /// events there.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <returns>Whether the control wants to handle mouse events at pos or not</returns>
    API_FUNCTION() virtual bool WantsMouseEventAtPos(Float2 pos, Float2 global_pos) { return GetBounds().Contains(pos); }

    /// <summary>
    /// Notification that the mouse moved while over this control, or while the control was capturing
    /// the mouse. The control needs to have the CanHandleMouseMove flag. This function is not called
    /// if the mouse is currently captured by a different control.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    API_FUNCTION() virtual void OnMouseMove(Float2 pos, Float2 global_pos) { }

    /// <summary>
    /// Notification that the mouse just moved over this control or before it starts receiving mouse
    /// move events. The control needs the CanHandleMouseEnterLeave flag. This function is not called
    /// if the mouse is currently captured by a different control.
    /// </summary>
    /// <param name="pos">Local mouse position></param>
    /// <param name="global_pos">Global mouse position</param>
    API_FUNCTION() virtual void OnMouseEnter(Float2 pos, Float2 global_pos) { }

    /// <summary>
    /// Notification that the mouse just left this control while it wasn't capturing it. It's also
    /// called if the control was capturing the mouse but released it, and the mouse wasn't over it.
    /// The control needs the CanHandleMouseEnterLeave flag.
    /// </summary>
    API_FUNCTION() virtual void OnMouseLeave() {}

    /// <summary>
    /// Call in OnMouseDown to direct future mouse events to this control until ReleaseMouseCapture is called.
    /// Make sure to call ReleaseMouseCapture on OnMouseUp. Mouse can only be captured for a single button.
    /// If a different control tries to capture the mouse, it'll be released first, before the other control
    /// starts capturing it.
    /// </summary>
    API_FUNCTION() virtual void CaptureMouseInput();

    /// <summary>
    /// Call in OnMouseUp when mouse capturing was started on this control for the same button. Unlike when calling
    /// the same function on FudgetGUIRoot directly, this function only releases the mouse if the control was
    /// capturing it.
    /// </summary>
    API_FUNCTION() virtual void ReleaseMouseInput();

    /// <summary>
    /// Whether the mouse input is currently captured by this control
    /// </summary>
    API_PROPERTY() bool MouseIsCaptured() const;

    /// <summary>
    /// Called if the control has the CanHandleKeyEvents and CanHandleNavigationKeys flags, to decide if it wants
    /// to use the key or lets the UI system use it. Returning true here and Ignore from the key down event will also
    /// allow the UI to handle the navigation event.
    /// </summary>
    /// <param name="key">The navigation key</param>
    /// <returns>Whether the control wants to get OnKeyDown and OnKeyUp events for the given navigation key</returns>
    API_FUNCTION() virtual bool WantsNavigationKey(KeyboardKeys key) { return false; }

    /// <summary>
    /// Called when a key is pressed while the control has the keyboard focus. The control needs to have the
    /// CanHandleKeyEvents flag.
    /// </summary>
    /// <param name="key">The key that was pressed</param>
    /// <returns>How the UI should continue processing the key input</returns>
    API_FUNCTION() virtual FudgetInputResult OnKeyDown(KeyboardKeys key) { return FudgetInputResult::Consume; }

    /// <summary>
    /// Called when a key is released that was pressed while this control had the keyboard focus, or the focus
    /// shifts to a different control and the key up was not received by this control yet. Return true if the
    /// control wants to prevent other controls below to get the key event. This should mirror the result of
    /// OnKeyDown.
    /// The control needs to have the CanHandleKeyEvents flag.
    /// </summary>
    /// <param name="key">The key that was released</param>
    /// <returns>How the UI should continue processing the key input</returns>
    API_FUNCTION() virtual bool OnKeyUp(KeyboardKeys key) { return true; }

    /// <summary>
    /// A key was pressed over this control while it had the keyboard focus, which could be translated to a character.
    /// The control needs to have the CanHandleKeyEvents flag.
    /// </summary>
    /// <param name="ch">The character resulting from the key press</param>
    /// <returns>How the UI should continue processing the key input</returns>
    API_FUNCTION() virtual FudgetInputResult OnCharInput(Char ch) { return FudgetInputResult::Consume; }

    /// <summary>
    /// Returns whether this control has the keyboard input focus or not. Focused controls can have a distinct
    /// appearance to make it easy to distinguish between their focused and unfocused states.
    /// </summary>
    API_PROPERTY() virtual bool GetFocused() const;

    /// <summary>
    /// Returns whether this control should appear in the focused state when drawing. Doesn't affect whether the control
    /// is focused or not.
    /// </summary>
    API_PROPERTY() virtual bool VirtuallyFocused() const;

    /// <summary>
    /// Sets the keyboard input focus to this control or removes focus from it. Focused controls can have a distinct
    /// appearance to make it easy to distinguish between their focused and unfocused states.
    /// </summary>
    API_PROPERTY() virtual void SetFocused(bool value);

    /// <summary>
    /// Returns whether this control has the mouse pointer over an uncovered part of its surface. For compound controls, this
    /// can be true if the mouse pointer is over a child control of the compound control. The control needs to have the
    /// CanHandleMouseMove and CanHandleMouseEnterLeave flags.
    /// </summary>
    API_PROPERTY() virtual bool GetHovered() const;

    /// <summary>
    /// Returns whether this control should appear like it is in the hovered state when drawing. Usually true when the control
    /// is hovered, or it is the child of a compound control which is hovered.
    /// </summary>
    API_PROPERTY() virtual bool VirtuallyHovered() const;

    /// <summary>
    /// The control's appearance to the user. Usually this is one of the FudgetVisualControlState enum flags, but can hold any
    /// custom state flag as well.
    /// </summary>
    API_PROPERTY() FORCE_INLINE virtual uint64 GetVisualState() const { return _visual_state; }

    /// <summary>
    /// The control's appearance to the user. Internally this is a 64 bit unsigned integer. The FudgetVisualControlState
    /// enum flags define the standard states, but custom values can be set as well.
    /// </summary>
    API_PROPERTY() FORCE_INLINE virtual FudgetVisualControlState GetVisualStateAsEnum() const { return (FudgetVisualControlState)_visual_state; }

    /// <summary>
    /// Returns the container at the root of the UI, which covers the UI usable area. For example the screen.
    /// </summary>
    /// <returns>The root UI container</returns>
    API_PROPERTY() FORCE_INLINE FudgetGUIRoot* GetGUIRoot() const { return _guiRoot; }

    /// <summary>
    /// Returns whether this control has the compound control flag. Compound controls hold other controls but behave
    /// like a single unit.
    /// </summary>
    API_PROPERTY() bool IsCompoundControl() const { return HasAnyFlag(FudgetControlFlag::CompoundControl); }

    /// <summary>
    /// For controls that can handle input or be focused, indicates whether this control wants to get keyboard focus
    /// or is ready to handle other input. For containers, only enabled containers' child controls should  behave like
    /// they are enabled.
    /// Check VirtuallyEnabled() to see if this control is enabled and all its parents in the hierarchy are also
    /// enabled.
    /// </summary>
    API_PROPERTY() bool GetEnabled() const;
    /// <summary>
    /// Makes the control enabled or disabled. Only enabled controls can accept input or focus.
    /// Setting a control to enabled only takes effect if all parents in the hierarchy are also enabled.
    /// </summary>
    /// <param name="value"></param>
    API_PROPERTY() virtual void SetEnabled(bool value);

    /// <summary>
    /// Returns whether the control is set to be enabled and all its parents in the hierarchy are also enabled.
    /// </summary>
    API_PROPERTY() bool VirtuallyEnabled() const;


    /// <summary>
    /// Whether the control is drawn and can handle user input. The control might still take up space in the UI
    /// if its visibility state was changed with MakeInvisible instead of Hide or SetVisible. To check if a control
    /// is made invisible, check the Invisible state flag.
    /// </summary>
    API_PROPERTY(Attributes="HideInEditor") bool IsVisible() const;
    /// <summary>
    /// Whether the control is hidden inside its parent's layout. Invisible but not hidden controls still take up
    /// space in their layout. The result of this property is not always the opposite of IsVisible, which checks
    /// for invisible too, and is influenced by the parent's visibility state as well.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() FORCE_INLINE bool IsHiddenInLayout() const { return HasAnyState(FudgetControlState::Hidden); }
    /// <summary>
    /// Shows the control in its layout. Calling this function for controls that have a hidden or invisible parent
    /// will only change the visibility state flag for this control, but might not make it visible.
    /// </summary>
    API_FUNCTION() FORCE_INLINE void Show() { SetVisible(true); }
    /// <summary>
    /// Hides the control in its layout. If the control was hidden with MakeInvisible, the Invisible state is removed
    /// and the Hidden state is added, causing it to not take up space in its layout anymore. If a parent control
    /// is already hidden, this only sets the state flag.
    /// </summary>
    API_FUNCTION() FORCE_INLINE void Hide() { SetVisible(false); }

    /// <summary>
    /// Gets one of the visibility values of the control. Controls can be Visible, Invisible or Hidden. Invisible
    /// controls are not drawn and don't receive user input, but are present in the UI by taking up space in
    /// their parent's layout. Hidden controls are also not drawn and don't receive input, but they also don't get
    /// notified about events or updates, and don't have any presence in their parent's layout.
    /// Visibility will be Visible even if any of the parents are hidden.
    /// </summary>
    /// <returns>The visibility state of the control</returns>
    API_PROPERTY() FudgetControlVisibility GetVisibility() const;
    /// <summary>
    /// Sets one of the visibility values of the control. Controls can be Visible, Invisible or Hidden. Invisible
    /// controls are not drawn and don't receive user input, but are present in the UI by taking up space in
    /// their parent's layout. Hidden controls are also not drawn and don't receive input, but they also don't get
    /// notified about events or updates, and don't have any presence in their parent's layout.
    /// Visibility will be Visible even if any of the parents are hidden.
    /// Setting this to Invisible when the control was hidden will first make it visible before it is turned invisible.
    /// </summary>
    /// <param name="value">The visibility state to set</param>
    API_PROPERTY() void SetVisibility(FudgetControlVisibility value);

    /// <summary>
    /// Changes the visibility state of the control, showing or hiding it. Calling this function with true will
    /// show the control in its layout, and false will make it hidden. The result is equivalent to calling Show or Hide.
    /// It will have no visible effect if a parent control is already hidden, but it will still change the state flags.
    /// </summary>
    /// <param name="value">Whether to show or hide the control</param>
    API_FUNCTION() virtual void SetVisible(bool value);
    /// <summary>
    /// Makes the control invisible in its layout, while still taking up space. It has no effect if the control is already
    /// hidden. Child controls of invisible controls will appear invisible as well.
    /// </summary>
    API_FUNCTION() virtual void MakeInvisible();


    // Render2D wrapper:

    /// <summary>
    /// Wrapper to Render2D's FillRectangle.
    /// </summary>
    /// <param name="pos">Position relative to the control</param>
    /// <param name="size">Size of control</param>
    /// <param name="color">Color to use for filling</param>
    API_FUNCTION() void FillRectangle(Int2 pos, Int2 size, Color color);

    /// <summary>
    /// Wrapper to Render2D's FillRectangle.
    /// </summary>
    /// <param name="rect">The rectangle to fill.</param>
    /// <param name="color">Color to use for filling</param>
    API_FUNCTION() void FillRectangle(const Rectangle &rect, Color color);

    /// <summary>
    /// Wrapper to Render2D's FillRectangle.
    /// </summary>
    /// <param name="rect">The rectangle to fill.</param>
    /// <param name="color1">The color to use for upper left vertex.</param>
    /// <param name="color2">The color to use for upper right vertex.</param>
    /// <param name="color3">The color to use for bottom right vertex.</param>
    /// <param name="color4">The color to use for bottom left vertex.</param>
    API_FUNCTION() void FillRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4);

    /// <summary>
    /// Wrapper to Render2D's DrawRectangle.
    /// </summary>
    /// <param name="pos">Position relative to the control</param>
    /// <param name="size">Size of control</param>
    /// <param name="color">Color to use for filling</param>
    /// <param name="thickness">Thickness of the rectangle's lines</param>
    API_FUNCTION() void DrawRectangle(Float2 pos, Float2 size, Color color, float thickness = 1.2f);

    /// <summary>
    /// Wrapper to Render2D's DrawRectangle.
    /// </summary>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">Color to use for filling</param>
    /// <param name="thickness">Thickness of the rectangle's lines</param>
    API_FUNCTION() void DrawRectangle(const Rectangle &rect, Color color, float thickness = 1.2f);

    /// <summary>
    /// Wrapper to Render2D's DrawRectangle.
    /// </summary>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color1">The color to use for upper left vertex.</param>
    /// <param name="color2">The color to use for upper right vertex.</param>
    /// <param name="color3">The color to use for bottom right vertex.</param>
    /// <param name="color4">The color to use for bottom left vertex.</param>
    /// <param name="thickness">The line thickness.</param>
    API_FUNCTION() void DrawRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness = 1.0f);

    /// <summary>
    /// Wrapper to Render2D's Draw9SlicingTexture.
    /// </summary>
    /// <param name="t">The texture to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom)</param>
    /// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom)</param>
    /// <param name="color">The color to multiply all texture pixels</param>
    /// <returns></returns>
    API_FUNCTION() void Draw9SlicingTexture(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's Draw9SlicingTexturePoint.
    /// </summary>
    /// <param name="t">The texture to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom)</param>
    /// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom)</param>
    /// <param name="color">The color to multiply all texture pixels</param>
    /// <returns></returns>
    API_FUNCTION() void Draw9SlicingTexturePoint(TextureBase* t, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's Draw9SlicingSprite
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom).</param>
    /// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom).</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void Draw9SlicingSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's Draw9SlicingSpritePoint
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom).</param>
    /// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom).</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void Draw9SlicingSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White);

    /// <summary>
    /// Draws 9-slicing texture by calling Draw9SlicingTexture, using borderWidths to calculate the border and UV parameters.
    /// </summary>
    /// <param name="t">The texture to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="borderWidths">The size of the stationary border on each side</param>
    /// <param name="color">The color to multiply drawn pixels with</param>
    /// <param name="alignment">Positioning and sizing of the texture inside the rectangle.</param>
    API_FUNCTION() void Draw9SlicingPrecalculatedTexture(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White, FudgetImageAlignment alignment = FudgetImageAlignment::Fit);

    /// <summary>
    /// Draws 9-slicing texture by calling Draw9SlicingTexturePoint, using borderWidths to calculate the border and UV parameters.
    /// </summary>
    /// <param name="t">The texture to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="borderWidths">The size of the stationary border on each side</param>
    /// <param name="color">The color to multiply drawn pixels with</param>
    /// <param name="alignment">Positioning and sizing of the texture inside the rectangle.</param>
    API_FUNCTION() void Draw9SlicingPrecalculatedTexturePoint(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White, FudgetImageAlignment alignment = FudgetImageAlignment::Fit);

    /// <summary>
    /// Draws 9-slicing sprite by calling Draw9SlicingSprite, using borderWidths to calculate the border and UV parameters.
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="borderWidths">The size of the stationary border on each side</param>
    /// <param name="color">The color to multiply drawn pixels with</param>
    /// <param name="alignment">Positioning and sizing of the sprite inside the rectangle.</param>
    API_FUNCTION() void Draw9SlicingPrecalculatedSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White, FudgetImageAlignment alignment = FudgetImageAlignment::Fit);

    /// <summary>
    /// Draws 9-slicing sprite by calling Draw9SlicingSpritePoint, using borderWidths to calculate the border and UV parameters.
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw</param>
    /// <param name="rect">The rectangle to draw in</param>
    /// <param name="borderWidths">The size of the stationary border on each side</param>
    /// <param name="color">The color to multiply drawn pixels with</param>
    /// <param name="alignment">Positioning and sizing of the sprite inside the rectangle.</param>
    API_FUNCTION() void Draw9SlicingPrecalculatedSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White, FudgetImageAlignment alignment = FudgetImageAlignment::Fit);

    /// <summary>
    /// Wrapper to Render2D's DrawBezier
    /// </summary>
    /// <param name="p1">The start point.</param>
    /// <param name="p2">The first control point.</param>
    /// <param name="p3">The second control point.</param>
    /// <param name="p4">The end point.</param>
    /// <param name="color">The line color</param>
    /// <param name="thickness">The line thickness.</param>
    API_FUNCTION() void DrawBezier(const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Color& color, float thickness = 1.0f);

    /// <summary>
    /// Wrapper to Render2D's DrawBlur
    /// </summary>
    /// <param name="rect">The target rectangle to draw (blurs its background).</param>
    /// <param name="blurStrength">The blur strength defines how blurry the background is. Larger numbers increase blur, resulting in a larger runtime cost on the GPU.</param>
    API_FUNCTION() void DrawBlur(const Rectangle& rect, float blurStrength);

    /// <summary>
    /// Wrapper to Render2D's DrawLine
    /// </summary>
    /// <param name="p1">The start point.</param>
    /// <param name="p2">The end point.</param>
    /// <param name="color">The line color.</param>
    /// <param name="thickness">The line thickness.</param>
    API_FUNCTION() void DrawLine(const Float2& p1, const Float2& p2, const Color& color, float thickness = 1.0f);

    /// <summary>
    /// Wrapper to Render2D's DrawLine
    /// </summary>
    /// <param name="p1">The start point.</param>
    /// <param name="p2">The end point.</param>
    /// <param name="color1">The line start color.</param>
    /// <param name="color2">The line end color.</param>
    /// <param name="thickness">The line thickness.</param>
    API_FUNCTION() void DrawLine(const Float2& p1, const Float2& p2, const Color& color1, const Color& color2, float thickness = 1.0f);

    /// <summary>
    /// Wrapper to Render2D's DrawMaterial
    /// </summary>
    /// <param name="material">The material to render. Must be a GUI material type.</param>
    /// <param name="rect">The target rectangle to draw.</param>
    /// <param name="color">The color to use.</param>
    API_FUNCTION() void DrawMaterial(MaterialBase* material, const Rectangle& rect, const Color& color);

    /// <summary>
    /// Wrapper to Render2D's DrawSprite
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawSpritePoint
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Draws a sprite in a rectangle with tiling or clipping as necessary.
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="size">The size to use for the sprite</param>
    /// <param name="offset">Offset the sprite with some amount</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawSpriteTiled(const SpriteHandle& spriteHandle, Float2 size, Float2 offset, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Draws a sprite in a rectangle with tiling or clipping as necessary. Uses point sampler.
    /// </summary>
    /// <param name="spriteHandle">The sprite to draw.</param>
    /// <param name="size">The size to use for the sprite</param>
    /// <param name="offset">Offset the sprite with some amount</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawSpritePointTiled(const SpriteHandle& spriteHandle, Float2 size, Float2 offset, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawText
    /// </summary>
    /// <param name="font">The font to use.</param>
    /// <param name="text">The text to render.</param>
    /// <param name="color">The text color.</param>
    /// <param name="location">The text location.</param>
    /// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
    API_FUNCTION() void DrawText(Font* font, const StringView& text, const Color& color, const Int2& location, MaterialBase* customMaterial = nullptr);

    /// <summary>
    /// Wrapper to Render2D's DrawText
    /// </summary>
    /// <param name="font">The font to use.</param>
    /// <param name="text">The text to render.</param>
    /// <param name="textRange">The input text range (substring range of the input text parameter).</param>
    /// <param name="color">The text color.</param>
    /// <param name="location">The text location.</param>
    /// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
    API_FUNCTION() void DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, const Int2& location, MaterialBase* customMaterial = nullptr);

    /// <summary>
    /// Wrapper to Render2D's DrawText
    /// </summary>
    /// <param name="font">The font to use.</param>
    /// <param name="text">The text to render.</param>
    /// <param name="color">The text color.</param>
    /// <param name="layout">The text layout properties.</param>
    /// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
    API_FUNCTION() void DrawText(Font* font, const StringView& text, const Color& color, API_PARAM(Ref) TextLayoutOptions& layout, MaterialBase* customMaterial = nullptr);

    /// <summary>
    /// Wrapper to Render2D's DrawText 
    /// </summary>
    /// <param name="font">The font to use.</param>
    /// <param name="text">The text to render.</param>
    /// <param name="textRange">The input text range (substring range of the input text parameter).</param>
    /// <param name="color">The text color.</param>
    /// <param name="layout">The text layout properties.</param>
    /// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
    API_FUNCTION() void DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, API_PARAM(Ref) TextLayoutOptions& layout, MaterialBase* customMaterial = nullptr);

    /// <summary>
    /// Wrapper to Render2D's DrawTexture 
    /// </summary>
    /// <param name="rt">The render target handle to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTexture(GPUTextureView* rt, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawTexture 
    /// </summary>
    /// <param name="t">The texture to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTexture(GPUTexture* t, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawTexture 
    /// </summary>
    /// <param name="t">The texture to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTexture(TextureBase* t, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Draws a texture in a rectangle with tiling and clipping as necessary.
    /// </summary>
    /// <param name="t">The texture to draw.</param>
    /// <param name="size">The size to use for the texture</param>
    /// <param name="offset">Offset the texture with some amount</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTextureTiled(GPUTexture *t, Float2 size, Float2 offset, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawTexturePoint 
    /// </summary>
    /// <param name="t">The texture to draw.</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTexturePoint(GPUTexture* t, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Draws a texture in a rectangle with tiling or clipping as necessary. Uses point sampler.
    /// </summary>
    /// <param name="t">The texture to draw.</param>
    /// <param name="size">The size to use for the texture</param>
    /// <param name="offset">Offset the texture with some amount</param>
    /// <param name="rect">The rectangle to draw.</param>
    /// <param name="color">The color to multiply all texture pixels.</param>
    API_FUNCTION() void DrawTexturePointTiled(GPUTexture* t, Float2 size, Float2 offset, const Rectangle& rect, const Color& color = Color::White);

    /// <summary>
    /// Wrapper to Render2D's DrawTexturedTriangles 
    /// </summary>
    /// <param name="t">The texture.</param>
    /// <param name="vertices">The vertices array.</param>
    /// <param name="uvs">The uvs array.</param>
    API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs);

    /// <summary>
    /// Wrapper to Render2D's DrawTexturedTriangles 
    /// </summary>
    /// <param name="t">The texture.</param>
    /// <param name="vertices">The vertices array.</param>
    /// <param name="uvs">The uvs array.</param>
    /// <param name="color">The color.</param>
    API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Color& color);

    /// <summary>
    /// Wrapper to Render2D's DrawTexturedTriangles 
    /// </summary>
    /// <param name="t">The texture.</param>
    /// <param name="vertices">The vertices array.</param>
    /// <param name="uvs">The uvs array.</param>
    /// <param name="colors">The colors array.</param>
    API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors);

    /// <summary>
    /// Wrapper to Render2D's DrawTexturedTriangles 
    /// </summary>
    /// <param name="t">The texture.</param>
    /// <param name="indices">The indices array.</param>
    /// <param name="vertices">The vertices array.</param>
    /// <param name="uvs">The uvs array.</param>
    /// <param name="colors">The colors array.</param>
    API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<uint16>& indices, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors);

    /// <summary>
    /// Wrapper to Render2D's FillTriangles
    /// </summary>
    /// <param name="vertices">The vertices array.</param>
    /// <param name="colors">The colors array.</param>
    /// <param name="useAlpha">If true alpha blending will be enabled.</param>
    API_FUNCTION() void FillTriangles(const Span<Float2>& vertices, const Span<Color>& colors, bool useAlpha);

    /// <summary>
    /// Wrapper to Render2D's FillTriangle
    /// </summary>
    /// <param name="p0">The first point.</param>
    /// <param name="p1">The second point.</param>
    /// <param name="p2">The third point.</param>
    /// <param name="color">The color.</param>
    API_FUNCTION() void FillTriangle(const Float2& p0, const Float2& p1, const Float2& p2, const Color& color);

    /// <summary>
    /// Fills a rectangular area with texture or sprite, depending on the area settings
    /// </summary>
    /// <param name="area">Settings for filling the rectangle</param>
    /// <param name="rect">Rectangle to fill</param>
    /// <param name="tint">Color that is multiplied with every pixel when drawing.</param>
    API_FUNCTION() void DrawArea(const FudgetDrawArea &area, const Rectangle &rect, const Color &tint = Color::White);

    /// <summary>
    /// Fills a rectangular area with texture or sprite, depending on the area settings
    /// </summary>
    /// <param name="area">Settings for filling the rectangle</param>
    /// <param name="pos">Position of the rectangle</param>
    /// <param name="size">Size of the rectangle</param>
    /// <param name="tint">Color that is multiplied with every pixel when drawing.</param>
    API_FUNCTION() void DrawArea(const FudgetDrawArea &area, Float2 pos, Float2 size, const Color &tint = Color::White);

    /// <summary>
    /// Draws a border with single color, texture or sprite, depending on the border settings
    /// </summary>
    /// <param name="area">Settings for filling the rectangle</param>
    /// <param name="rect">Rectangle to fill</param>
    /// <param name="tint">Color that is multiplied with every pixel when drawing.</param>
    API_FUNCTION() void DrawBorder(const FudgetDrawBorder &area, const Rectangle &rect, const Color &tint = Color::White);

    /// <summary>
    /// Draws a border with single color, texture or sprite, depending on the border settings
    /// </summary>
    /// <param name="area">Settings for filling the rectangle</param>
    /// <param name="pos">Position of the rectangle</param>
    /// <param name="size">Size of the rectangle</param>
    /// <param name="tint">Color that is multiplied with every pixel when drawing.</param>
    API_FUNCTION() void DrawBorder(const FudgetDrawBorder &area, Float2 pos, Float2 size, const Color &tint = Color::White);

    /// <summary>
    /// Draws a drawable inside a rectangle.
    /// </summary>
    /// <param name="drawable">The drawable to draw.</param>
    /// <param name="stateindex">The index of the state in the drawable</param>
    /// <param name="rect">The bounds for drawing</param>
    /// <param name="tint">A color to multiply with every drawn pixel</param>
    API_FUNCTION() void DrawDrawable(FudgetDrawable *drawable, int stateindex, const Rectangle &rect, const Color &tint = Color::White);

    /// <summary>
    /// Draws a drawable inside a rectangle.
    /// </summary>
    /// <param name="drawable">The drawable to draw.</param>
    /// <param name="stateindex">The index of the state in the drawable</param>
    /// <param name="pos">The position of the bounds for drawing</param>
    /// <param name="size">The size of the bounds for drawing</param>
    /// <param name="tint">A color to multiply with every drawn pixel</param>
    API_FUNCTION() void DrawDrawable(FudgetDrawable *drawable, int stateindex, Float2 pos, Float2 size, const Color &tint = Color::White);

    /// <summary>
    /// Wrapper to Render2D's PushClip
    /// </summary>
    /// <param name="rect">Clipping rectangle in local coordinates</param>
    API_FUNCTION() void PushClip(const Rectangle &rect);

    /// <summary>
    /// Wrapper to Render2D's PopClip. Prevents calling more times than the number of times PushClip was called.
    /// </summary>
    API_FUNCTION() void PopClip();

    // Styling

    /// <summary>
    /// Resets all cached values that are used for styling. Many values are only checked once and not retrieved again, unless
    /// the style or theme is changed. Call this function if a change should result in changed appearance but the cached values
    /// prevent that.
    /// </summary>
    /// <param name="forced">Whether to reinitialize the styles and mark layout as size changed even if there was no cached style.</param>
    /// <returns>Whether there was any cache that needed to be cleared. Always true if forced is true.</returns>
    API_FUNCTION() virtual bool ClearStyleCache(bool forced = false);
    
    /// <summary>
    /// Gets the style set explicitly for the control to decide the look of the control. The functions that look up a
    /// resource for the control using its style all check the style returned by GetStyle first, and if a value was not
    /// found, they try it with the style returned by the style returned from GetClassStyle.
    /// </summary>
    API_PROPERTY() FudgetStyle* GetStyle();

    /// <summary>
    /// Sets the style set explicitly for the control to decide the look of the control.
    /// Call with null to reset to the default style.
    /// </summary>
    /// <param name="value">The new style.</param>
    API_PROPERTY() void SetStyle(FudgetStyle *value);

    /// <summary>
    /// The style that's used for this type of control to decide the look of the control. Use GetStyle instead to
    /// get the style that was explicitly set with SetStyle. The functions that get a resource for the control's
    /// style all check that style first, and if a value is not found, they try it with the style returned by this
    /// function.
    /// The style returned is cached. It can be reset by calling ClearStyleCache.
    /// Returns null if the control hasn't been initialized yet.
    /// </summary>
    API_PROPERTY() FudgetStyle* GetClassStyle();

    /// <summary>
    /// Gets the theme if one is set for this control, which is the storage for colors and other resources used for styling
    /// the control. A theme is in effect even if this is null.
    /// </summary>
    API_PROPERTY() FudgetTheme* GetTheme() const { return _theme; }
    /// <summary>
    /// Sets the theme which gives the colors and other values used for styling the controls
    /// </summary>
    API_PROPERTY() void SetTheme(FudgetTheme *value);

    /// <summary>
    /// The theme data which is retrieved using the theme id set to this control, or if not set, the theme in the closets
    /// parent that has a theme. If no theme is set to any control, the main theme is returned.
    /// </summary>
    API_PROPERTY() FudgetTheme* GetActiveTheme();

    /// <summary>
    /// Constructs a painter object based on a painter mapping identified by mapping_id and initializes it with the mapping. Mappings contain the name of
    /// the painter's type to be created and the mapping of painter ids to the ids of a style. That is, when the painter tries to look up a resource by an
    /// id, another id will be used to get the resource.
    /// The mapping_id is looked up in the control's styles.
    /// When a new painter is created in place of an existing one, the existing painter can be passed as the first argument to this function. Otherwise
    /// if the painter is no longer needed, it should be deleted manually. Painters created by a control are automatically deleted when the control
    /// is deleted.
    /// </summary>
    /// <typeparam name="T">Base type for the painter. The new painter must be this type or a type derived from it.</typeparam>
    /// <param name="current">The painter that was created before, possibly on the same line. It's only needed to make sure it's freed before a
    /// new painter is created.</param>
    /// <param name="mapping_id">Id of the painter mapping in the control's style.</param>
    /// <returns>The part painter that was created or null if no painter mapping matches the mapping id or the template argument.</returns>
    template<typename T>
    T* CreateStylePainter(T *current, int mapping_id)
    {
        FudgetPartPainterMapping painter_data;
        FudgetPartPainter *painter = nullptr;

        if (current != nullptr && current->GetOwner() == this)
            Delete(current);

        bool valid = GetStylePainterMapping(mapping_id, painter_data);

        if (!valid)
            return nullptr;

        painter = FudgetThemes::CreatePainter(painter_data.PainterType);

        T *result = dynamic_cast<T*>(painter);
        if (painter != nullptr && result == nullptr)
        {
            Delete(painter);
            painter = nullptr;
        }

        if (result != nullptr)
            RegisterStylePainterInternal(result, painter_data);

        return result;
    }

    /// <summary>
    /// Returns a value for an id in the control's style.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the value in the active styles</param>
    /// <param name="result">Variable that receives the value</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleValue(int id, API_PARAM(Out) Variant &result);

    /// <summary>
    /// Returns a style for an id in the control's style. The id should refer to a string resource in the theme
    /// that will be used as the style name to look up.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the string in the active styles</param>
    /// <param name="result">Variable that receives the style</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleStyle(int id, API_PARAM(Out) FudgetStyle* &result);

    /// <summary>
    /// Returns a part painter mapping for an id in the control's style. The id should refer to a FudgetPartPainterMapping
    /// structure resource in the theme that will be used to create a part painter.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the part painter mapping in the active styles</param>
    /// <param name="result">Variable that receives the part painter mapping</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStylePainterMapping(int id, API_PARAM(Out) FudgetPartPainterMapping &result);

    /// <summary>
    /// Returns a string for an id in the control's style.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the string in the active styles</param>
    /// <param name="result">Variable that receives the string</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleString(int id, API_PARAM(Out) String &result);

    /// <summary>
    /// Returns a color for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the color in the active styles</param>
    /// <param name="result">Variable that receives the color</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleColor(int id, API_PARAM(Out) Color &result);

    /// <summary>
    /// Returns a list of draw colors for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the draw colors in the active styles</param>
    /// <param name="result">Variable that receives the color</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleDrawColors(int id, API_PARAM(Out) FudgetDrawColors &result);

    /// <summary>
    /// Returns a bool value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the bool in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleBool(int id, API_PARAM(Out) bool &result);

    /// <summary>
    /// Returns a float value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the float in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFloat(int id, API_PARAM(Out) float &result);

    /// <summary>
    /// Returns a Float2 value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the Float2 in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFloat2(int id, API_PARAM(Out) Float2 &result);

    /// <summary>
    /// Returns a Float3 value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the Float3 in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFloat3(int id, API_PARAM(Out) Float3 &result);

    /// <summary>
    /// Returns a Float4 value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the Float4 in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFloat4(int id, API_PARAM(Out) Float4 &result);

    /// <summary>
    /// Returns an int value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the int in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleInt(int id, API_PARAM(Out) int &result);

    /// <summary>
    /// Returns an Int2 value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the int in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleInt2(int id, API_PARAM(Out) Int2 &result);

    /// <summary>
    /// Returns an int value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the int in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleInt3(int id, API_PARAM(Out) Int3 &result);

    /// <summary>
    /// Returns an int value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the int in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleInt4(int id, API_PARAM(Out) Int4 &result);

    /// <summary>
    /// Returns an enum value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <typeparam name="T">The enum type</typeparam>
    /// <param name="id">Id associated with the int in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    template<typename T>
    bool GetStyleEnum(int id, T &result)
    {
        if (TryGetStyleEnumInner<T>(GetStyle(), id, false, result))
            return true;
        return TryGetStyleEnumInner<T>(GetClassStyle(), id, true, result);
    }

    /// <summary>
    /// Returns a padding value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the padding value in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStylePadding(int id, API_PARAM(Out) FudgetPadding &result);

    /// <summary>
    /// Returns a border value for the control based on an id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id associated with the border value in the active styles</param>
    /// <param name="result">Variable that receives the result</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleBorder(int id, API_PARAM(Out) FudgetBorder &result);

    /// <summary>
    /// Use this function to check settings for a font in the style, when font creation is not necessary.
    /// To create the font and get its cached value directly, call GetStyleFont instead.
    /// Returns settings for a font asset, which includes its type id, size and styles. 
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id for the FudgetFontSettings value in the active styles</param>
    /// <param name="result">Structure with font creation settings</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFontSettings(int id, API_PARAM(Out) FudgetFontSettings &result);

    /// <summary>
    /// Sets result to a font object and the settings that were used to create it. The control might have
    /// to create the font the first time this function is called. In case any font data should be updated,
    /// call ResetCreatedFonts.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id of a FudgetFontSettings value in the active styles</param>
    /// <param name="result">Structure with font object and its settings</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleFont(int id, API_PARAM(Out) FudgetFont &result);

    /// <summary>
    /// Clears cached font data that was generated by calls to GetStyleFont.
    /// </summary>
    /// <returns>Whether there was a font in the cache before reset.</returns>
    API_FUNCTION() bool ResetCreatedFonts();

    /// <summary>
    /// Tries to get a draw area settings for the passed id. The result will be a valid draw area
    /// whether the original value is a draw area, a color, a texture or a struct holding these values.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id of a draw area value in the active styles</param>
    /// <param name="result">The draw area that can be passed to DrawArea</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleDrawArea(int id, API_PARAM(Out) FudgetDrawArea &result);

    /// <summary>
    /// Tries to get a drawable for the passed id. The result will be a valid drawable if the original
    /// value is an instruction list created with FudgetDrawableBuilder, or one of the types that can be
    /// drawn with a drawable.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id of an area list value in the active styles</param>
    /// <param name="drawable_owner">A part painter which will cause the destruction of the drawable when it is destroyed.</param>
    /// <param name="result">Value that receives the drawable on success.</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleDrawable(int id, FudgetPartPainter *drawable_owner, API_PARAM(Out) FudgetDrawable* &result);
       
    /// <summary>
    /// Tries to get a texture for the passed id. The result will be a valid 
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id of a draw area value in the active styles</param>
    /// <param name="result">The texture asset reference to receive the result</param>
    /// <returns>Whether a texture was found for the id</returns>
    API_FUNCTION() bool GetStyleTexture(int id, API_PARAM(Out) TextureBase* &result);

    /// <summary>
    /// Tries to get the font draw settings for the passed id. The result will be a valid draw settings
    /// whether the original value is a draw setting, a Int2, a color or a material id.
    /// The resulting value depends on both the style and the theme currently active for this control.
    /// </summary>
    /// <param name="id">Id of a font draw settings value in the active styles</param>
    /// <param name="result">The settings for font drawing</param>
    /// <returns>Whether a valid value was found for the id</returns>
    API_FUNCTION() bool GetStyleTextDrawSettings(int id, API_PARAM(Out) FudgetTextDrawSettings &result);

    // Serialization

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

    // Initialization

    /// <summary>
    /// Returns whether the Initialize function has been called once on this control. By default, the main Fudget
    /// initializes controls after deserialization is over. During manual control creation, Initialize is called
    /// once the control has been added oo a parent that is in the gui hierarchy
    /// </summary>
    API_PROPERTY() FORCE_INLINE bool IsInitialized() const { return (_state_flags & FudgetControlState::Initialized) == FudgetControlState::Initialized; }

    /// <summary>
    /// Returns whether the Initialize function has been called once on this control. By default, the main Fudget
    /// initializes controls after deserialization is over. During manual control creation, Initialize is called
    /// once the control has been added oo a parent that is in the gui hierarchy
    /// </summary>
    API_PROPERTY() FORCE_INLINE bool IsStyleInitialized() const { return (_state_flags & FudgetControlState::StyleInitialized) == FudgetControlState::StyleInitialized; }

    /// <summary>
    /// Called when redrawing the control. Derived controls should override OnDraw instead. 
    /// </summary>
    API_FUNCTION() virtual void DoDraw();

    /// <summary>
    /// Called on each frame if the control is registered to receive events. Derived controls should override OnUpdate instead.
    /// </summary>
    /// <param name="delta_time">The time passed since the last update</param>
    API_FUNCTION() virtual void DoUpdate(float delta_time);

    /// <summary>
    /// Called when the control's enabled state changed. Not called when a parent container became enabled/disabled, and might not cause
    /// a change in the appearance and behavior of the control. If the appearance changes as well due to this change, DoVirtuallyEnabledChanged
    /// is called after this function returns. Derived controls should override OnEnabledChanged instead. 
    /// </summary>
    API_FUNCTION() virtual void DoEnabledChanged();

    /// <summary>
    /// Called when the control's virtual enabled state changed. This can mean a parent became enabled/disabled that might make this control
    /// appear enabled/disabled as well. Derived controls should override OnVirtuallyEnabledChanged instead. 
    /// </summary>
    API_FUNCTION() virtual void DoVirtuallyEnabledChanged();

    /// <summary>
    /// Called when the keyboard input focus changes from one control to another. It is called both on the
    /// control losing focus and the control gaining focus before the change happens.
    /// Make sure to call the base implementation upon receiving this event.
    /// </summary>
    /// <param name="focused">Whether this control gains the focus or not</param>
    /// <param name="other">The control which loses or gains the focus. Can be null</param>
    API_FUNCTION() virtual void DoFocusChanging(bool focused, FudgetControl *other);

    /// <summary>
    /// Called when the keyboard input focus changes from one control to another. It is called both on the
    /// control losing focus and the control gaining focus after the change happened.
    /// Make sure to call the base implementation upon receiving this event.
    /// </summary>
    /// <param name="focused">Whether this control gained the focus or not</param>
    /// <param name="other">The control which lost or gained the focus. Can be null</param>
    API_FUNCTION() virtual void DoFocusChanged(bool focused, FudgetControl *other);

    /// <summary>
    /// Called when a control or its parent became visible, making it appear in the UI.
    /// Derived controls should override OnShow instead. 
    /// </summary>
    API_FUNCTION() virtual void DoShow();

    /// <summary>
    /// Called when a control or its parent became hidden or invisible. Both hidden and invisible controls don't
    /// show up in the UI, but invisible controls still take up space in their layout.
    /// Derived controls should override OnHide instead. 
    /// </summary>
    API_FUNCTION() virtual void DoHide();

    /// <summary>
    /// Called when the control starts capturing the mouse input.
    /// </summary>
    API_FUNCTION() virtual void DoMouseCaptured();
    /// <summary>
    /// Called when the control stops capturing the mouse input.
    /// </summary>
    API_FUNCTION() virtual void DoMouseReleased();

    /// <summary>
    /// Called when a mouse button was pressed over this control. The control needs to have the
    /// CanHandleMouseUpDown flag. If CaptureReleaseMouse* flags are set, the mouse will be automatically
    /// captured on the corresponding mouse input.
    /// Override OnMouseDown instead of this function.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just pressed</param>
    /// <param name="double_click">Whether the call is the result of double clicking</param>
    /// <returns>One of the result options that decide how to deal with the mouse event</returns>
    API_FUNCTION() virtual FudgetInputResult DoMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click);

    /// <summary>
    /// Called when a mouse button was released over this control. Return true if the control wants to prevent
    /// other controls below to get the mouse event. This should mirror the result of DoMouseDown.
    /// The control needs to have the CanHandleMouseUpDown flag. If CaptureReleaseMouse* flags are
    /// set, the mouse will be automatically released on the corresponding mouse input.
    /// Override OnMouseUp instead of this function.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just released</param>
    /// <returns>Prevent OnMouseUp reaching other controls or not</returns>
    API_FUNCTION() virtual bool DoMouseUp(Float2 pos, Float2 global_pos, MouseButton button);

    /// <summary>
    /// Notification that the mouse moved while over this control, or while the control was capturing
    /// the mouse. The control needs to have the CanHandleMouseMove flag. This function is not called
    /// if the mouse is currently captured by a different control.
    /// Override OnMouseMove instead of this function.
    /// </summary>
    /// <param name="pos">Local mouse position</param>
    /// <param name="global_pos">Global mouse position</param>
    API_FUNCTION() virtual void DoMouseMove(Float2 pos, Float2 global_pos);

    /// <summary>
    /// Notification that the mouse just moved over this control or before it starts receiving mouse
    /// move events. The control needs the CanHandleMouseEnterLeave flag. This function is not called
    /// if the mouse is currently captured by a different control.
    /// </summary>
    /// <param name="pos">Local mouse position></param>
    /// <param name="global_pos">Global mouse position</param>
    API_FUNCTION() virtual void DoMouseEnter(Float2 pos, Float2 global_pos);

    /// <summary>
    /// Notification that the mouse just left this control while it wasn't capturing it. It's also
    /// called if the control was capturing the mouse but released it, and the mouse wasn't over it.
    /// The control needs the CanHandleMouseEnterLeave flag.
    /// </summary>
    API_FUNCTION() virtual void DoMouseLeave();

    /// <summary>
    /// Gets the displayed cursor when the mouse pointer is moved above the control, as long as no other control
    /// captured the mouse, and this control doesn't ignore mouse movement. If this value is Default, the actual
    /// cursor shown can be context dependent. Read GetContextCursor for the current value.
    /// </summary>
    /// <returns>The cursor type set for this control</returns>
    API_PROPERTY() CursorType GetCursor() const { return _cursor; }
    /// <summary>
    /// Sets the displayed cursor when the mouse pointer is moved above the control, as long as no other control
    /// captured the mouse, and this control doesn't ignore mouse movement. If this value is Default, the actual
    /// cursor shown can be context dependent. Read GetContextCursor for the current value.
    /// </summary>
    API_PROPERTY() void SetCursor(CursorType value);

    /// <summary>
    /// The displayed cursor when the mouse pointer is moved above the control, as long as no other control
    /// captured the mouse, and this control doesn't ignore mouse movement. This is the same as GetCursor if
    /// there is a non default cursor set. Otherwise the control can decide to provide a different cursor,
    /// for example based on pointer position.
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() virtual CursorType GetContextCursor() const { return _cursor; }


    /// <summary>
    /// Checks if the control has any of the passed flags in its state
    /// </summary>
    /// <param name="states">State flags to check</param>
    /// <returns>Whether any of the flags match the current state of the control</returns>
    API_FUNCTION() virtual bool HasAnyState(FudgetControlState states) const;
    /// <summary>
    /// Checks if the control has every passed flags in its state
    /// </summary>
    /// <param name="states">State flags to check</param>
    /// <returns>Whether every flag is found in the current state of the control</returns>
    API_FUNCTION() virtual bool HasAllStates(FudgetControlState states) const;

    /// <summary>
    /// Padding inside the bounds of the control occupied by a frame. If this is not a framed control, it might return
    /// an empty padding. Derived classes might override this function to include parts, like FudgetScrollingControl
    /// that includes the scrollbars.
    /// </summary>
    API_PROPERTY() virtual FudgetPadding GetFramePadding() const;
protected:
    /// <summary>
    /// Adds or removes the passed state flag or flags depending on the value.
    /// </summary>
    /// <param name="states">The flag or flags to set or unset</param>
    /// <param name="value">True if the flag should be set, false if it should be unset </param>
    API_FUNCTION() virtual void SetState(FudgetControlState states, bool value);

    /// <summary>
    /// Adds or removes the passed visual state enum flag or flags depending on the value. The state is
    /// a 64 bit unsigned integer, but FudgetVisualControlState can be used to set the default states.
    /// </summary>
    /// <param name="state">The flag or flags to set or unset</param>
    /// <param name="value">True if the flag should be set, false if it should be unset </param>
    API_FUNCTION() void SetVisualState(FudgetVisualControlState state, bool value);

    /// <summary>
    /// Adds or removes the passed visual state flag or flags depending on the value. The state is
    /// a 64 bit unsigned integer, but FudgetVisualControlState can be used to set the default states.
    /// </summary>
    /// <param name="state">The flag or flags to set or unset</param>
    /// <param name="value">True if the flag should be set, false if it should be unset </param>
    API_FUNCTION() void SetVisualState(uint64 state, bool value);

    /// <summary>
    /// Called recursively by the gui root when the deserialization was complete on a Fudget object. This function shouldn't
    /// do anything if there is no gui root set. Override OnInitialize for class initialization in derived types. Make sure
    /// to call the base implementation if overriden.
    /// </summary>
    API_FUNCTION() virtual void DoInitialize();

    /// <summary>
    /// Called when the parent changes and the styles have not been initialized yet. This function shouldn't do anything if
    /// there is no parent or no gui root set. Override OnStyleInitialize for initializing style of child controls and to
    /// cache style values to avoid looking them up every draw frame. Make sure to call the base implementation if overriden.
    /// </summary>
    API_FUNCTION() virtual void DoStyleInitialize();

    /// <summary>
    /// Called each time the control is added to a parent. The base implementation checks for the presence of the ResetFlags
    /// in _flags and if found, resets the flags with GetInitFlags. Override in derived classes only if a new flag type is
    /// added that needs initialization.
    /// </summary>
    API_FUNCTION() virtual void InitializeFlags();

    /// <summary>
    /// Called when the control is added to a parent and the ResetFlags flag is present among the flags (which
    /// is the default at control creation.) It can be modified individually later by calling SetControlFlags.
    /// This function can be called multiple times, so relying on special behavior for initialization can have
    /// unwanted side effects.
    /// Setting the RegisterToUpdates flag either by this function or by calling SetControlFlags will automatically call
    /// RegisterToUpdate(true) when added to a parent and RegisterToUpdate(false) when removed.
    /// Include the result of the base control's GetInitFlags if it's needed for correct behavior.
    /// </summary>
    /// <returns>Control flags that should be set to the control.</returns>
    API_FUNCTION() virtual FudgetControlFlag GetInitFlags() const { return FudgetControlFlag::None; }

    /// <summary>
    /// Notifies the UI root that the cursor of this control changed. It's only necessary if the cursor change is not
    /// the result of a mouse move event.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() void DoCursorChanged();

    /// <summary>
    /// Requests the immediate recalculation of a control's sizes and the positioning of its contents. The control
    /// can choose to ignore this call if its contents are cached and didn't change since the last layout request.
    /// Only call this function on the root of the control tree, or on a control which can layout its children
    /// without invalidating its parent's layout.
    /// In the container's implementation, if no layout is present, the controls are moved to their requested size
    /// and position. Compound controls can override this function to layout their children if they don't have a
    /// layout.
    /// </summary>
    API_FUNCTION() virtual void RequestLayout() {}

    /// <summary>
    /// Called by inner size or position changing functions to deal with changes. This implementation
    /// notifies a parent to mark itself dirty.
    /// </summary>
    /// <param name="dirt_flags">Flags of what changed</param>
    API_FUNCTION() virtual void SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags);

    /// <summary>
    /// Called by inner visibility changing functions to notify the parent about a possible dirty layout.
    /// </summary>
    API_FUNCTION() virtual void VisibilityModified();

    /// <summary>
    /// Called by a parent container's SetParentDisabledRecursive to notify its children that they have to be disabled.
    /// </summary>
    /// <param name="value">Whether the parent was disabled</param>
    API_FUNCTION() virtual void SetParentDisabled(bool value);

    /// <summary>
    /// Called by a parent container's SetParentVisibilityRecursive to notify its children that they have to be disabled.
    /// </summary>
    /// <param name="value">Whether the parent is visible</param>
    API_FUNCTION() virtual void SetParentVisibility(bool value);

    /// <summary>
    /// Used internally to deregister the control from OnUpdate calls. This happens after the parent was changed, but
    /// the root hasn't changed yet, and the control is not initialized. Override OnRootChanging instead.
    /// </summary>
    /// <param name="new_root">The root to be set to the control</param>
    API_FUNCTION() virtual void DoRootChanging(FudgetGUIRoot *new_root);
    /// <summary>
    /// Used internally to register control to OnUpdate calls. This happens after the parent and the root both
    /// changed and the control was initialized. Override OnRootChanged instead.
    /// </summary>
    /// <param name="old_root">The previous root control</param>
    API_FUNCTION() virtual void DoRootChanged(FudgetGUIRoot *old_root);

    /// <summary>
    /// Notification about parent change, when the control is added to a container or removed from one. Override
    /// OnParentChanged in derived classes
    /// </summary>
    /// <param name="old_parent">The previous parent which can be null</param>
    API_FUNCTION() virtual void DoParentChanged(FudgetContainer *old_parent);

    /// <summary>
    /// Notification when a state changed in the parent that the control needs to know of. The default implementation
    /// sets or unsets ParentDisabled and ParentHidden flats.
    /// </summary>
    API_FUNCTION() virtual void DoParentStateChanged();


    // IFudgetDataConsumer default implementations to make it easier to make a control into a data consumer.
    // These do nothing.

    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataChangeBegin() {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataChangeEnd(bool changed) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeReset() {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataReset() {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeCleared() {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataCleared() {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeUpdated(int index) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataUpdated(int index) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeAdded(int count) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataAdded(int count) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeRemoved(int index, int count) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataRemoved(int index, int count) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataToBeInserted(int index, int count) {}
    /// <summary>
    /// Called by _data_proxy if it is registered as a data consumer in an IFudgetDataProvider. The default
    /// implementation doesn't do anything. Only the useful functions need to be overriden.
    /// </summary>
    API_FUNCTION() virtual void DataInserted(int index, int count) {}


    /// <summary>
    /// Can be used to register the control with an IFudgetDataProvider data provider. Calls the data
    /// functions above like the control itself was an IFudgetDataConsumer.
    /// </summary>
    API_FIELD() FudgetControlDataConsumerProxy *_data_proxy;
private:
    /// <summary>
    /// Gets the name of the control's style. To look up the active style, the control first checks if a style for
    /// its type exists, and if so, if it has a derived style with this name. If none are true, it checks the first
    /// ancestor if it has a style and a derived style with this name. It continues up the chain of ancestors
    /// until one style is found. The first style with the matching name is used. If the name is not found, the
    /// style for the nearest type is used.
    /// </summary>
    /// <returns>The name of the style for this control</returns>
    API_FUNCTION() String GetStyleName() const { return _style_name; }

    /// <summary>
    /// Sets the name of the control's style. To look up the active style, the control first checks if a style for
    /// its type exists, and if so, if it has a derived style with this name. If none are true, it checks the first
    /// ancestor if it has a style and a derived style with this name. It continues up the chain of ancestors
    /// until one style is found. The first style with the matching name is used. If the name is not found, the
    /// style for the nearest type is used.
    /// </summary>
    /// <param name="value">The new style name.</param>
    API_FUNCTION() void SetStyleName(const String &value);

    void DrawDrawableInstructions(const FudgetDrawInstructionList &area, const Rectangle &rect, const Color &tint = Color::White);
    void DrawTextureInner(TextureBase *t, SpriteHandle sprite_handle, Float2 scale, Float2 offset, Rectangle rect, Color tint, FudgetImageAlignment alignment, bool point);
    void DrawTiled(GPUTexture *t, SpriteHandle sprite_handle, bool point, Float2 size, Float2 offset, const Rectangle& rect, const Color& color);
    void Draw9SlicingPrecalculatedInner(TextureBase *t, SpriteHandle sprite_handle, Rectangle rect, const FudgetPadding &borderWidths, const Color &color, FudgetImageAlignment alignment, bool point);

    // Modifies the rectangle based on the texture/sprite size and the alignment
    void AlignDrawRectangle(Int2 tex_size, FudgetImageAlignment alignment, /*modified*/ Rectangle &rect);

    /// <summary>
    /// See GetStyleEnum.
    /// </summary>
    template<typename T>
    bool TryGetStyleEnumInner(FudgetStyle *style, int id, bool check_theme, T &result)
    {
        if (style == nullptr)
        {
            result = (T)0;
            return false;
        }

        return FudgetStyle::GetEnumResource<T>(style, GetActiveTheme(), id, check_theme, result);
    }

    /// <summary>
    /// The gui root is at the root of the control hierarchy. It forwards input and OnUpdate calls. It can be requested to
    /// capture the mouse input and keeps track of the focused control.
    /// </summary>
    FudgetGUIRoot *_guiRoot;

    /// <summary>
    /// Controls in a layout usually can't be moved freely. This function determines if that's the case or not.
    /// </summary>
    /// <returns>True if the control is allowed to directly change its position</returns>
    bool IsPositionChangePermitted() const;

    /// <summary>
    /// Setting the size hint of a control will automatically set its size as well. This is usually false
    /// in a layout that is responsible for the control dimensions.
    /// </summary>
    /// <returns></returns>
    bool IsDirectSizeChangePermitted() const;

    /// <summary>
    /// Directly changes the position and size of the control. Only to be called by FudgetLayout.
    /// </summary>
    /// <param name="pos">The new position</param>
    /// <param name="size">The new size</param>
    virtual void LayoutUpdate(Int2 pos, Int2 size);

    // Creates an array of names based on the control's type and its ancestor classes' type names, starting with the most derived class.
    void CreateClassNames();


    /// <summary>
    /// Don't call. Exposed for C# to make CreateStylePainter possible. Saves painter to the list of painters that
    /// will be freed once the control is destroyed.
    /// </summary>
    API_FUNCTION() void RegisterStylePainterInternal(FudgetPartPainter *painter, API_PARAM(Ref)  const FudgetPartPainterMapping &mapping);

    /// <summary>
    /// Don't call. Exposed for C# to make CreateStylePainter possible. Removes painter from the list of painters and
    /// destroys the object.
    /// </summary>
    API_FUNCTION() void UnregisterStylePainterInternal(FudgetPartPainter *painter);

    // Drawables need to be destroyed by this control, so they are added to an internal list. When the control or
    // the painter is destroyed, so is the drawable.
    void RegisterDrawable(FudgetPartPainter *drawable_owner, FudgetDrawable *drawable);

    // Parent control is responsible for positioning and destruction of this control. 
    FudgetContainer *_parent;
    // Index inside parent.
    int _index;
    // Readable name for control to let the user identify it in the editor. Not unique.
    String _name;

    FudgetControlFlag _flags;

    // Cursor shown for controls that don't let the mouse pass through on mouse move.
    CursorType _cursor;

    Int2 _pos;
    Int2 _size;

    Int2 _hint_size;
    Int2 _min_size;
    Int2 _max_size;

    FudgetControlState _state_flags;

    uint64 _visual_state;

    // This is reset on each Draw and calculated when needed to simplify GlobalToLocal and LocalToGlobal in draw calls.
    // It's only used during the drawing functions. It can be manually reset when needed
    Float2 _cached_global_to_local_translation;

    // Number of times PushClip was called without a PopClip
    int _clipping_count;

    // Used locally to avoid double calling functions from the parent.
    bool _changing;

    // Null or the style used to decide the look of the control. When null, the active style is based on the type
    // or the default style name.
    FudgetStyle *_style;

    // Style used for drawing the control if a specific style hasn't been set. The style is determined based on the
    // class name, if such a style is present, or the parent class name etc. If none are found, the theme's basic style
    // is used, but it might lack required settings.
    FudgetStyle *_cached_style;

    // Name of the control's default style. If this is not set, the class name and its parent class names are used
    // to look up the default style. If a style is explicitely set for the control, that's used instead.
    String _style_name;

    // The theme set for this control. If not set, nearest parent's theme is used with a set theme. If none
    // found, the main theme is assumed.
    FudgetTheme *_theme;

    // The theme used for drawing determined by the _theme_id or the nearest parent's theme id. Resolved once on draw.
    FudgetTheme *_cached_theme;

    // A list of strings created for this control that includes the name of every control in the inheritance
    // chain, up to FudgetControl. It is mainly used to get a style appropriate for this control when drawing.
    // The list might not be created until the control needs to access the active style.
    Array<String> _class_names;

    std::map<int, FudgetFont> _cached_fonts;

    // Painters created for this control that should be destroyed by this control
    Array<FudgetPartPainter*> _painters;

    // Drawable objects created by part painters. The drawables are destroyed when the painter or this control is destroyed.
    Dictionary<FudgetDrawable*, FudgetPartPainter*> _drawables;

    friend class FudgetLayout;
    friend class FudgetContainer;
    friend class FudgetGUIRoot;
    friend class FudgetPartPainter;
    friend class FudgetDrawable;
    friend class FudgetControlDataConsumerProxy;
};


/// <summary>
/// Flax doesn't allow default implementation to exist for interface functions, nor to make
/// an interface function non-public. This proxy object is created for every FudgetControl
/// and can be used to register with a data provider. It calls its owning control's protected
/// functions that do nothing. Those can be overriden in derived classes for proper functioning.
/// </summary>
API_CLASS()
class FudgetControlDataConsumerProxy : public ScriptingObject, public IFudgetDataConsumer
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetControlDataConsumerProxy);
public:
    /// <inheritdoc />
    FORCE_INLINE void DataChangeBegin() override { _owner->DataChangeBegin(); }
    /// <inheritdoc />
    FORCE_INLINE void DataChangeEnd(bool changed) override { _owner->DataChangeEnd(changed); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeReset() override { _owner->DataToBeReset(); }
    /// <inheritdoc />
    FORCE_INLINE void DataReset() override { _owner->DataReset(); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeCleared() override { _owner->DataToBeCleared(); }
    /// <inheritdoc />
    FORCE_INLINE void DataCleared() override { _owner->DataCleared(); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeUpdated(int index) override { _owner->DataToBeUpdated(index); }
    /// <inheritdoc />
    FORCE_INLINE void DataUpdated(int index) override { _owner->DataUpdated(index); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeAdded(int count) override { _owner->DataToBeAdded(count); }
    /// <inheritdoc />
    FORCE_INLINE void DataAdded(int count) override { _owner->DataAdded(count); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeRemoved(int index, int count) override { _owner->DataToBeRemoved(index, count); }
    /// <inheritdoc />
    FORCE_INLINE void DataRemoved(int index, int count) override { _owner->DataRemoved(index, count); }
    /// <inheritdoc />
    FORCE_INLINE void DataToBeInserted(int index, int count) override { _owner->DataToBeInserted(index, count); }
    /// <inheritdoc />
    FORCE_INLINE void DataInserted(int index, int count) override { _owner->DataInserted(index, count); }

    FudgetControl *_owner;
};


