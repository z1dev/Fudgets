#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Vector4.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Core/Types/Span.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Serialization/Serialization.h"

#include "Utils/SmartPointers.h"
#include "Styling/Token.h"
#include "Styling/ElementPainter.h"


class FudgetContainer;
class FudgetGUIRoot;
class FudgetStyle;
class FudgetTheme;
class TextureBase;
struct SpriteHandle;
class MaterialBase;
struct TextLayoutOptions;
struct TextRange;
class GPUTexture;
class GPUTextureView;
class Font;
struct FudgetFillAreaSettings;
struct FudgetPadding;
struct FudgetFontSettings;
struct FudgetFont;


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
	/// Negative values count as "infinite" size (near float limits.)
	/// </summary>
	Max,
};


/// <summary>
/// Used for any function call in controls and layouts that need to know if it affects size or position
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetDirtType : uint8
{
	/// <summary>
	/// Flag corresponding to size. Used for example to indicate when a control's size changes
	/// </summary>
	Size = 1,
	/// <summary>
	/// Flag corresponding to position. Used for example to indicate when a control's position changes
	/// </summary>
	Position = 2,

	/// <summary>
	/// Flag corresponding to both size and position.
	/// </summary>
	All = Size | Position
};

DECLARE_ENUM_OPERATORS(FudgetDirtType);

/// <summary>
/// Flags for controls that describe their behavior
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetControlFlags
{
	/// <summary>
	/// Empty flag
	/// </summary>
	None = 0,
	/// <summary>
	/// FudgetContainers will add this flag themselves for derived classes, while other classes should
	/// never set this
	/// </summary>
	ContainerControl = 1 << 0,
	/// <summary>
	/// Supports mouse button up and down inputs. This should be set for any control that might not react
	/// to mouse events by default but is not transparent. Controls can decide to handle the events per call.
	/// </summary>
	CanHandleMouseUpDown = 1 << 1,
	/// <summary>
	/// Supports mouse move inputs. This can be set for any control that might not react to mouse events by
	/// default, but is not transparent. The top control with this flag will be the only one receiving the
	/// event if it's under the mouse.
	/// </summary>
	CanHandleMouseMove = 1 << 2,
	/// <summary>
	/// Supports mouse enter and mouse leave events, but only while CanHandleMouseMove is also set.
	/// </summary>
	CanHandleMouseEnterLeave = 1 << 3,
	/// <summary>
	/// Makes control "eat" all mouse messages if the mouse is over it, even if it does not have any of the
	/// other flags for mouse events. Ignored while the mouse is captured by another control. This doesn't
	/// prevent parents peeking at the message early (when it's implemented)
	/// </summary>
	BlockMouseEvents = 1 << 4,
	/// <summary>
	/// Set the input focus for keyboard on the control when pressing the left mouse button over it. Ignored
	/// while another control is capturing the mouse input.
	/// </summary>
	FocusOnMouseLeft = 1 << 5,
	/// <summary>
	/// Set the input focus for keyboard on the control when pressing the right mouse button over it. Ignored
	/// while another control is capturing the mouse input.
	/// </summary>
	FocusOnMouseRight = 1 << 6,
	/// <summary>
	/// Set the mouse capture on the control when the left button is pressed down, and release it when the
	/// same button is released.
	/// </summary>
	CaptureReleaseMouseLeft = 1 << 7,
	/// <summary>
	/// Set the mouse capture on the control when the right button is pressed down, and release it when the
	/// same button is released.
	/// </summary>
	CaptureReleaseMouseRight = 1 << 8,
	/// <summary>
	/// Controls with this flag will have their OnUpdate function called, by registering them automatically with
	/// RegisterToUpdate(true). Removing the flag with SetControlFlags will also unregister them from updates.
	/// </summary>
	RegisterToUpdates = 1 << 9,
	/// <summary>
	/// Set on the control before adding it to a parent to reset its flags from GetInitFlags.
	/// </summary>
	ResetFlags = 1 << 10,
	/// <summary>
	/// Only for top-level controls which are direct children of the gui root. It's ignored for others. Places
	/// the top-level control above other top-level controls that don't have the AlwaysOnTop flag. Providing the
	/// flag with GetInitFlags or changing it in SetControlFlags will change the control's position in the root.
	/// </summary>
	AlwaysOnTop = 1 << 11,
};
DECLARE_ENUM_OPERATORS(FudgetControlFlags);

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
enum class FudgetMouseButtonResult
{
	/// <summary>
	/// The mouse button press event should be ignored. This won't let the event pass through to the control
	/// below, but will prevent any automatic handling, like focusing the control.
	/// </summary>
	Ignore,
	/// <summary>
	/// The mouse button press event should be ignored for the control. The event processing will continue
	/// and the event will be posted to the control below that expects mouse events.
	/// </summary>
	PassThrough,
	/// <summary>
	/// The control used the mouse event, preventing it to be sent to controls below it. Automatic handling, like
	/// setting focus on the control is allowed, if the control has the right flags.
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
	/// Called when redrawing the control. Inherited controls can call Render2D methods here.
	/// Always call the derived Draw in the overriden method for styled controls.
	/// </summary>
	API_FUNCTION() virtual void Draw();

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

	// Size and position

	/// <summary>
	/// The size preferred by the control that serves as a hint to the layout, which might ignore it if it doesn't fit.
	/// The standard controls should offer a sane value by default, but it can be changed by modifying the HintSize
	/// property (C#) or calling SetHintSize(C++).
	/// </summary>
	/// <returns>The size requested by the control or its user. Not always same as the actual size.</returns>
	API_PROPERTY() virtual Float2 GetHintSize() const { return _hint_size; }

	/// <summary>
	/// The size preferred by the control that serves as a hint to the layout, which might ignore it if it doesn't fit.
	/// The standard controls should offer a sane value by default.
	/// </summary>
	/// <param name="value">The preferred size for the control.</param>
	API_PROPERTY() virtual void SetHintSize(Float2 value);

	/// <summary>
	/// Gets the size that the control currently occupies in its parent's layout. This value can't be directly changed. Use
	/// SetHintSize to change it.
	/// </summary>
	/// <returns>The current size of the control.</returns>
	API_PROPERTY() virtual Float2 GetSize() const;

	/// <summary>
	/// Gets the minimum size the control can be resized to while still showing its contents. The control might become smaller
	/// than this size if the layout it is in doesn't have enough space.
	/// </summary>
	/// <returns>The minimum size the control needs to show its contents</returns>
	API_PROPERTY() virtual Float2 GetMinSize() const { return _min_size; }

	/// <summary>
	/// Sets the minimum size for the control which is a hint for the layout when it has to compress its controls.
	/// </summary>
	/// <param name="value">The new minimum size.</param>
	API_PROPERTY() virtual void SetMinSize(Float2 value);

	/// <summary>
	/// Gets the size that determines how big a control can grow at most. The control might become larger
	/// than this size if the layout it is in doesn't respect the property.
	/// </summary>
	/// <returns>The maximum size the control is allowed to have</returns>
	API_PROPERTY() virtual Float2 GetMaxSize() const { return _max_size; }
	/// <summary>
	/// Gets the size that determines how big a control can grow at most. The control might become larger
	/// than this size if the layout it is in doesn't respect the property.
	/// </summary>
	/// <returns>The maximum size the control is allowed to have</returns>
	API_PROPERTY() virtual void SetMaxSize(Float2 value);

	/// <summary>
	/// Gets one of the possible sizes of the control. Use as an alternative to GetHintSize, GetMinSize, GetMaxSize
	/// and GetSize.
	/// </summary>
	/// <param name="sizeType">Which size to return</param>
	/// <returns>The control's size specified in type</returns>
	API_FUNCTION() Float2 GetRequestedSize(FudgetSizeType sizeType) const;

	/// <summary>
	/// Gets the current width the control takes up in its parent's  layout.
	/// </summary>
	/// <returns>The current width of the control</returns>
	API_PROPERTY() virtual float GetWidth() const { return GetSize().X; }

	/// <summary>
	/// Gets the current height the control takes up in its parent's  layout.
	/// </summary>
	/// <returns>The current height of the control</returns>
	API_PROPERTY() virtual float GetHeight() const { return GetSize().Y; }

	/// <summary>
	/// Gets the position of this control in its parent's layout, relative to the top-left corner.
	/// </summary>
	/// <returns>The control's current position</returns>
	API_PROPERTY() Float2 GetPosition() const;

	/// <summary>
	/// Modifies the position of the control in its parent's layout, relative to the top-left corner if
	/// the layout allows free positioning.
	/// </summary>
	/// <param name="value">The requested new position</param>
	API_PROPERTY() virtual void SetPosition(Float2 value);

	/// <summary>
	/// Gets the local bounding rectangle of the control relative to its parent, calculating the size if necessary.
	/// </summary>
	API_PROPERTY() Rectangle GetBoundsInParent() const { return Rectangle(GetPosition(), GetSize()); }

	/// <summary>
	/// Gets the bounding rectangle of the control relative to itself, calculating the size if necessary. The
	/// top-left corner of the rectangle is always at zero coordinates.
	/// </summary>
	API_PROPERTY() Rectangle GetBounds() const { return Rectangle(Float2(0.f), GetSize()); }

	/// <summary>
	/// Gets the left side's coordinate of this control in its parent's layout, relative to the top-left corner.
	/// </summary>
	/// <returns>The X component of the position</returns>
	API_PROPERTY() virtual float GetLeft() const { return _pos.X; }

	/// <summary>
	/// Gets the top side's coordinate of this control in its parent's layout, relative to the top-left corner.
	/// </summary>
	/// <returns>The Y component of the position</returns>
	API_PROPERTY() virtual float GetTop() const { return _pos.Y; }

	/// <summary>
	/// Gets the right side's coordinate of this control in its parent's layout, relative to the top-left corner.
	/// </summary>
	/// <returns>The coordinate of the control's right side</returns>
	API_PROPERTY() float GetRight() const { return GetLeft() + GetWidth(); }

	/// <summary>
	/// Gets the bottom side's coordinate of this control in its parent's layout, relative to the top-left corner.
	/// </summary>
	/// <returns>The coordinate of the control's bottom side</returns>
	API_PROPERTY() float GetBottom() const { return GetTop() + GetHeight(); }

	/// <summary>
	/// Called by inner size or position changing functions to deal with changes. This implementation
	/// notifies a parent to mark itself dirty.
	/// </summary>
	/// <param name="dirt_flags">Flags of what changed</param>
	API_FUNCTION() virtual void SizeOrPosModified(FudgetDirtType dirt_flags);

	/// <summary>
	/// Returns whether the control is a direct child control of the GUI root and is always placed above not
	/// AlwaysOnTop controls.
	/// </summary>
	API_PROPERTY() bool GetAlwaysOnTop() const;
	/// <summary>
	/// For controls that are direct child to the GUI root, sets whether the control should always be placed
	/// above not AlwaysOnTop controls.
	/// </summary>
	/// <param name="valuse"></param>
	API_PROPERTY() void SetAlwaysOnTop(bool value);

	// Update callback

	/// <summary>
	/// Called on each frame if the control is registered to receive events
	/// </summary>
	/// <param name="delta_time">The time passed since the last update</param>
	API_FUNCTION() virtual void OnUpdate(float delta_time);

	/// <summary>
	/// Registers or unregisters the control to receive the global update tick.
	/// </summary>
	/// <param name="value">Register on true and unregister on false</param>
	API_FUNCTION() void RegisterToUpdate(bool value);

	/// <summary>
	/// Whether the control is executing its OnUpdate after registering it with RegisterToUpdate.
	/// </summary>
	API_PROPERTY() bool IsUpdateRegistered() const { return _updating_registered; }

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
	/// CachedGlobalToLocal or CachedLocalToGlobal call on each draw frame, but calling it once is enough. Does nothing if this
	/// value has already been saved. The calculated value is invalidated at the start of each Draw but it can also be invalidated
	/// with InvalidateGlobalToLocalCache
	/// </summary>
	API_FUNCTION() void CacheGlobalToLocal() const;

	/// <summary>
	/// Discards the result of the precalculated global to local difference, that was calculated in CacheGlobalToLocal.
	/// Calling CacheGlobalToLocal will calculate the value again, which is necessary before the first call to
	/// CachedGlobalToLocal or CachedLocalToGlobal.
	/// Avoid calling this function if unnecessary, because it will force recalculation of the cached difference. 
	/// </summary>
	API_FUNCTION() void InvalidateGlobalToLocalCache() const;

	/// <summary>
	/// Converts a coordinate from local control space to global UI space. Can be used multiple times after CacheGlobalToLocal was
	/// called once on the current draw frame.
	/// </summary>
	/// <param name="local">The coordinate relative to the top-left corner of the control</param>
	/// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
	API_FUNCTION() virtual Float2 CachedLocalToGlobal(Float2 local) const;

	/// <summary>
	/// Converts a coordinate from global UI space to local control space. Can be used multiple times after CacheGlobalToLocal was
	/// called once on the current draw frame.
	/// </summary>
	/// <param name="global">The coordinate relative to the top-left corner of the UI screen</param>
	/// <returns>The coordinate relative to the top-left corner of the control</returns>
	API_FUNCTION() virtual Float2 CachedGlobalToLocal(Float2 global) const;

	/// <summary>
	/// Converts a rectangle from local control space to global UI space. Can be used multiple times after CacheGlobalToLocal was
	/// called once on the current draw frame.
	/// </summary>
	/// <param name="local">The rectangle with a location relative to the top-left corner of the control</param>
	/// <returns>The coordinate relative to the top-left corner of the UI screen</returns>
	API_FUNCTION() virtual Rectangle CachedLocalToGlobal(const Rectangle &local) const;

	/// <summary>
	/// Converts a rectangle from global UI space to local control space. Can be used multiple times after CacheGlobalToLocal was
	/// called once on the current draw frame.
	/// </summary>
	/// <param name="global">TThe rectangle with a location relative to the top-left corner of the UI screen</param>
	/// <returns>The coordinate relative to the top-left corner of the control</returns>
	API_FUNCTION() virtual Rectangle CachedGlobalToLocal(const Rectangle &global) const;

	// Control flags

	/// <summary>
	/// Gets flags describing the control's behavior or appearance.
	/// </summary>
	/// <returns>The flags set for the control</returns>
	API_PROPERTY() virtual FudgetControlFlags GetControlFlags() const;

	/// <summary>
	/// Sets flags describing the control's behavior or appearance.
	/// </summary>
	/// <param name="flags">The new flags</param>
	API_PROPERTY() virtual void SetControlFlags(FudgetControlFlags flags);

	/// <summary>
	/// Matches the given flags with those of the control, returning true only if all the flags were found.
	/// </summary>
	/// <param name="flags">The flags to look for</param>
	/// <returns>True  if all the flags were found on the control</returns>
	API_FUNCTION() virtual bool HasAllFlags(FudgetControlFlags flags) const;

	/// <summary>
	/// Matches the given flags with those of the layout, returning true if any of the flags was found.
	/// </summary>
	/// <param name="flags">The flags to look for</param>
	/// <returns>True if at least one flag was found on the control</returns>
	API_FUNCTION() virtual bool HasAnyFlag(FudgetControlFlags flags) const;

	// Input:

	/// <summary>
	/// Called when a mouse button was pressed over this control. The control can start capturing
	/// the mouse with StartMouseCapture to be sure to receive OnMouseUp and OnMouseMove even when the
	/// mouse pointer is not over it.
	/// Return true if the control wants to prevent other controls below to get the mouse event.
	/// </summary>
	/// <param name="pos">Local mouse position</param>
	/// <param name="global_pos">Global mouse position</param>
	/// <param name="button">Button that was just pressed</param>
	/// <param name="double_click">Whether the call is the result of double clicking</param>
	/// <returns>One of the result options that decide how to deal with the mouse event</returns>
	API_FUNCTION() virtual FudgetMouseButtonResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) { return FudgetMouseButtonResult::Consume; }

	/// <summary>
	/// Called when a mouse button was released over this control. The control should stop capturing
	/// the mouse with StopMouseCapture if it started capturing it on OnMouseDown for the same button.
	/// Return true if the control wants to prevent other controls below to get the mouse event. This
	/// should mirror the result of OnMouseDown.
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
	API_FUNCTION() virtual bool WantsMouseEventAtPos(Float2 pos, Float2 global_pos) { return true; }

	/// <summary>
	/// Notification that the mouse just moved over this control
	/// </summary>
	/// <param name="pos">Local mouse position</param>
	/// <param name="global_pos">Global mouse position</param>
	API_FUNCTION() virtual void OnMouseMove(Float2 pos, Float2 global_pos) { }

	/// <summary>
	/// Notification that the mouse just moved over this control somewhere
	/// </summary>
	/// <param name="pos">Local mouse position></param>
	/// <param name="global_pos">Global mouse position</param>
	API_FUNCTION() virtual void OnMouseEnter(Float2 pos, Float2 global_pos) { }

	/// <summary>
	/// Notification that the mouse just left this control while it wasn't capturing it. It's also
	/// called if the control was capturing the mouse but released it, and the mouse wasn't over it.
	/// </summary>
	API_FUNCTION() virtual void OnMouseLeave() {}

	/// <summary>
	/// Returns the container at the root of the UI, which covers the UI usable area. For example the screen.
	/// </summary>
	/// <returns>The root UI container</returns>
	API_PROPERTY() FudgetGUIRoot* GetGUIRoot() const { return _guiRoot; }

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
	/// Returns whether this control has the keyboard input focus or not. Focused controls can have a distinct
	/// appearance to make it easy to distinguish between their focused and unfocused states.
	/// </summary>
	API_PROPERTY() virtual bool GetFocused() const;

	/// <summary>
	/// Sets the keyboard input focus to this control or removes focus from it. Focused controls can have a distinct
	/// appearance to make it easy to distinguish between their focused and unfocused states.
	/// </summary>
	API_PROPERTY() virtual void SetFocused(bool value);

	// Render2D wrapper:

	/// <summary>
	/// Wrapper to Render2D's FillRectangle.
	/// </summary>
	/// <param name="pos">Position relative to the control</param>
	/// <param name="size">Size of control</param>
	/// <param name="color">Color to use for filling</param>
	API_FUNCTION() void FillRectangle(Float2 pos, Float2 size, Color color) const;

	/// <summary>
	/// Wrapper to Render2D's FillRectangle.
	/// </summary>
	/// <param name="rect">The rectangle to fill.</param>
	/// <param name="color">Color to use for filling</param>
	API_FUNCTION() void FillRectangle(const Rectangle &rect, Color color) const;

	/// <summary>
	/// Wrapper to Render2D's FillRectangle.
	/// </summary>
	/// <param name="rect">The rectangle to fill.</param>
	/// <param name="color1">The color to use for upper left vertex.</param>
	/// <param name="color2">The color to use for upper right vertex.</param>
	/// <param name="color3">The color to use for bottom right vertex.</param>
	/// <param name="color4">The color to use for bottom left vertex.</param>
	API_FUNCTION() void FillRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4) const;

	/// <summary>
	/// Wrapper to Render2D's DrawRectangle.
	/// </summary>
	/// <param name="pos">Position relative to the control</param>
	/// <param name="size">Size of control</param>
	/// <param name="color">Color to use for filling</param>
	/// <param name="thickness">Thickness of the rectangle's lines</param>
	API_FUNCTION() void DrawRectangle(Float2 pos, Float2 size, Color color, float thickness = 1.2f) const;

	/// <summary>
	/// Wrapper to Render2D's DrawRectangle.
	/// </summary>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">Color to use for filling</param>
	/// <param name="thickness">Thickness of the rectangle's lines</param>
	API_FUNCTION() void DrawRectangle(const Rectangle &rect, Color color, float thickness = 1.2f) const;

	/// <summary>
	/// Wrapper to Render2D's DrawRectangle.
	/// </summary>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color1">The color to use for upper left vertex.</param>
	/// <param name="color2">The color to use for upper right vertex.</param>
	/// <param name="color3">The color to use for bottom right vertex.</param>
	/// <param name="color4">The color to use for bottom left vertex.</param>
	/// <param name="thickness">The line thickness.</param>
	API_FUNCTION() void DrawRectangle(const Rectangle& rect, const Color& color1, const Color& color2, const Color& color3, const Color& color4, float thickness = 1.0f) const;

	/// <summary>
	/// Wrapper to Render2D's Draw9SlicingTexture.
	/// </summary>
	/// <param name="t">The texture to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom)</param>
	/// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom)</param>
	/// <param name="color">The color to multiply all texture pixels</param>
	/// <returns></returns>
	API_FUNCTION() void Draw9SlicingTexture(TextureBase *t, const Rectangle &rect, const Float4 &border, const Float4 &borderUVs, const Color &color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's Draw9SlicingTexturePoint.
	/// </summary>
	/// <param name="t">The texture to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom)</param>
	/// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom)</param>
	/// <param name="color">The color to multiply all texture pixels</param>
	/// <returns></returns>
	API_FUNCTION() void Draw9SlicingTexturePoint(TextureBase* t, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's Draw9SlicingSprite
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom).</param>
	/// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom).</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void Draw9SlicingSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's Draw9SlicingSpritePoint
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="border">The borders for 9-slicing (inside rectangle, ordered: left, right, top, bottom).</param>
	/// <param name="borderUVs">The borders UVs for 9-slicing (inside rectangle UVs, ordered: left, right, top, bottom).</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void Draw9SlicingSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Float4& border, const Float4& borderUVs, const Color& color = Color::White) const;

	/// <summary>
	/// Draws 9-slicing texture by calling Draw9SlicingTexture, using borderWidths to calculate the border and UV parameters.
	/// </summary>
	/// <param name="t">The texture to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="borderWidths">The size of the stationary border on each side</param>
	/// <param name="color">The color to multiply drawn pixels with</param>
	API_FUNCTION() void Draw9SlicingPrecalculatedTexture(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White) const;

	/// <summary>
	/// Draws 9-slicing texture by calling Draw9SlicingTexturePoint, using borderWidths to calculate the border and UV parameters.
	/// </summary>
	/// <param name="t">The texture to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="borderWidths">The size of the stationary border on each side</param>
	/// <param name="color">The color to multiply drawn pixels with</param>
	API_FUNCTION() void Draw9SlicingPrecalculatedTexturePoint(TextureBase *t, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White) const;

	/// <summary>
	/// Draws 9-slicing sprite by calling Draw9SlicingSprite, using borderWidths to calculate the border and UV parameters.
	/// </summary>
	/// <param name="t">The sprite to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="borderWidths">The size of the stationary border on each side</param>
	/// <param name="color">The color to multiply drawn pixels with</param>
	API_FUNCTION() void Draw9SlicingPrecalculatedSprite(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White) const;

	/// <summary>
	/// Draws 9-slicing sprite by calling Draw9SlicingSpritePoint, using borderWidths to calculate the border and UV parameters.
	/// </summary>
	/// <param name="t">The sprite to draw</param>
	/// <param name="rect">The rectangle to draw in</param>
	/// <param name="borderWidths">The size of the stationary border on each side</param>
	/// <param name="color">The color to multiply drawn pixels with</param>
	API_FUNCTION() void Draw9SlicingPrecalculatedSpritePoint(const SpriteHandle& spriteHandle, const Rectangle &rect, const FudgetPadding &borderWidths, const Color &color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawBezier
	/// </summary>
	/// <param name="p1">The start point.</param>
	/// <param name="p2">The first control point.</param>
	/// <param name="p3">The second control point.</param>
	/// <param name="p4">The end point.</param>
	/// <param name="color">The line color</param>
	/// <param name="thickness">The line thickness.</param>
	API_FUNCTION() void DrawBezier(const Float2& p1, const Float2& p2, const Float2& p3, const Float2& p4, const Color& color, float thickness = 1.0f) const;

	/// <summary>
	/// Wrapper to Render2D's DrawBlur
	/// </summary>
	/// <param name="rect">The target rectangle to draw (blurs its background).</param>
	/// <param name="blurStrength">The blur strength defines how blurry the background is. Larger numbers increase blur, resulting in a larger runtime cost on the GPU.</param>
	API_FUNCTION() void DrawBlur(const Rectangle& rect, float blurStrength) const;

	/// <summary>
    /// Wrapper to Render2D's DrawLine
    /// </summary>
    /// <param name="p1">The start point.</param>
    /// <param name="p2">The end point.</param>
    /// <param name="color">The line color.</param>
    /// <param name="thickness">The line thickness.</param>
	API_FUNCTION() void DrawLine(const Float2& p1, const Float2& p2, const Color& color, float thickness = 1.0f) const;

    /// <summary>
    /// Wrapper to Render2D's DrawLine
    /// </summary>
    /// <param name="p1">The start point.</param>
    /// <param name="p2">The end point.</param>
    /// <param name="color1">The line start color.</param>
    /// <param name="color2">The line end color.</param>
    /// <param name="thickness">The line thickness.</param>
    API_FUNCTION() void DrawLine(const Float2& p1, const Float2& p2, const Color& color1, const Color& color2, float thickness = 1.0f) const;

	/// <summary>
	/// Wrapper to Render2D's DrawMaterial
	/// </summary>
	/// <param name="material">The material to render. Must be a GUI material type.</param>
	/// <param name="rect">The target rectangle to draw.</param>
	/// <param name="color">The color to use.</param>
	API_FUNCTION() void DrawMaterial(MaterialBase* material, const Rectangle& rect, const Color& color) const;

	/// <summary>
	/// Wrapper to Render2D's DrawSprite
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawSprite(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawSpritePoint
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawSpritePoint(const SpriteHandle& spriteHandle, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Draws a sprite in a rectangle with tiling or clipping as necessary.
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="size">The size to use for the sprite</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawSpriteTiled(const SpriteHandle& spriteHandle, Float2 size, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Draws a sprite in a rectangle with tiling or clipping as necessary. Uses point sampler.
	/// </summary>
	/// <param name="spriteHandle">The sprite to draw.</param>
	/// <param name="size">The size to use for the sprite</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawSpritePointTiled(const SpriteHandle& spriteHandle, Float2 size, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawText
	/// </summary>
	/// <param name="font">The font to use.</param>
	/// <param name="text">The text to render.</param>
	/// <param name="color">The text color.</param>
	/// <param name="location">The text location.</param>
	/// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
	API_FUNCTION() void DrawText(Font* font, const StringView& text, const Color& color, const Float2& location, MaterialBase* customMaterial = nullptr) const;

	/// <summary>
	/// Wrapper to Render2D's DrawText
	/// </summary>
	/// <param name="font">The font to use.</param>
	/// <param name="text">The text to render.</param>
	/// <param name="textRange">The input text range (substring range of the input text parameter).</param>
	/// <param name="color">The text color.</param>
	/// <param name="location">The text location.</param>
	/// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
	API_FUNCTION() void DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, const Float2& location, MaterialBase* customMaterial = nullptr) const;

	/// <summary>
	/// Wrapper to Render2D's DrawText
	/// </summary>
	/// <param name="font">The font to use.</param>
	/// <param name="text">The text to render.</param>
	/// <param name="color">The text color.</param>
	/// <param name="layout">The text layout properties.</param>
	/// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
	API_FUNCTION() void DrawText(Font* font, const StringView& text, const Color& color, API_PARAM(Ref) const TextLayoutOptions& layout, MaterialBase* customMaterial = nullptr) const;

	/// <summary>
	/// Wrapper to Render2D's DrawText 
	/// </summary>
	/// <param name="font">The font to use.</param>
	/// <param name="text">The text to render.</param>
	/// <param name="textRange">The input text range (substring range of the input text parameter).</param>
	/// <param name="color">The text color.</param>
	/// <param name="layout">The text layout properties.</param>
	/// <param name="customMaterial">The custom material for font characters rendering. It must contain texture parameter named Font used to sample font texture.</param>
	API_FUNCTION() void DrawText(Font* font, const StringView& text, API_PARAM(Ref) const TextRange& textRange, const Color& color, API_PARAM(Ref) const TextLayoutOptions& layout, MaterialBase* customMaterial = nullptr) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexture 
	/// </summary>
	/// <param name="rt">The render target handle to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTexture(GPUTextureView* rt, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexture 
	/// </summary>
	/// <param name="t">The texture to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTexture(GPUTexture* t, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexture 
	/// </summary>
	/// <param name="t">The texture to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTexture(TextureBase* t, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Draws a texture in a rectangle with tiling or clipping as necessary.
	/// </summary>
	/// <param name="t">The texture to draw.</param>
	/// <param name="size">The size to use for the texture</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTextureTiled(GPUTexture *t, Float2 size, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexturePoint 
	/// </summary>
	/// <param name="t">The texture to draw.</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTexturePoint(GPUTexture* t, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Draws a texture in a rectangle with tiling or clipping as necessary. Uses point sampler.
	/// </summary>
	/// <param name="t">The texture to draw.</param>
	/// <param name="size">The size to use for the texture</param>
	/// <param name="rect">The rectangle to draw.</param>
	/// <param name="color">The color to multiply all texture pixels.</param>
	API_FUNCTION() void DrawTexturePointTiled(GPUTexture* t, Float2 size, const Rectangle& rect, const Color& color = Color::White) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexturedTriangles 
	/// </summary>
	/// <param name="t">The texture.</param>
	/// <param name="vertices">The vertices array.</param>
	/// <param name="uvs">The uvs array.</param>
	API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexturedTriangles 
	/// </summary>
	/// <param name="t">The texture.</param>
	/// <param name="vertices">The vertices array.</param>
	/// <param name="uvs">The uvs array.</param>
	/// <param name="color">The color.</param>
	API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Color& color) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexturedTriangles 
	/// </summary>
	/// <param name="t">The texture.</param>
	/// <param name="vertices">The vertices array.</param>
	/// <param name="uvs">The uvs array.</param>
	/// <param name="colors">The colors array.</param>
	API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors) const;

	/// <summary>
	/// Wrapper to Render2D's DrawTexturedTriangles 
	/// </summary>
	/// <param name="t">The texture.</param>
	/// <param name="indices">The indices array.</param>
	/// <param name="vertices">The vertices array.</param>
	/// <param name="uvs">The uvs array.</param>
	/// <param name="colors">The colors array.</param>
	API_FUNCTION() void DrawTexturedTriangles(GPUTexture* t, const Span<uint16>& indices, const Span<Float2>& vertices, const Span<Float2>& uvs, const Span<Color>& colors) const;

	/// <summary>
	/// Wrapper to Render2D's FillTriangles
	/// </summary>
	/// <param name="vertices">The vertices array.</param>
	/// <param name="colors">The colors array.</param>
	/// <param name="useAlpha">If true alpha blending will be enabled.</param>
	API_FUNCTION() void FillTriangles(const Span<Float2>& vertices, const Span<Color>& colors, bool useAlpha) const;

	/// <summary>
	/// Wrapper to Render2D's FillTriangle
	/// </summary>
	/// <param name="p0">The first point.</param>
	/// <param name="p1">The second point.</param>
	/// <param name="p2">The third point.</param>
	/// <param name="color">The color.</param>
	API_FUNCTION() void FillTriangle(const Float2& p0, const Float2& p1, const Float2& p2, const Color& color) const;

	/// <summary>
	/// Draws a rectangular area filled with color or texture, depending on the area settings
	/// </summary>
	/// <param name="area">Settings for filling the rectangle</param>
	/// <param name="rect">Rectangle to fill</param>
	API_FUNCTION() void DrawFillArea(const FudgetFillAreaSettings &area, const Rectangle &rect) const;

	/// <summary>
	/// Draws a rectangular area filled with color or texture, depending on the area settings
	/// </summary>
	/// <param name="area">Settings for filling the rectangle</param>
	/// <param name="pos">Position of the rectangle</param>
	/// <param name="siz">Size of the rectangle</param>
	API_FUNCTION() void DrawFillArea(const FudgetFillAreaSettings &area, Float2 pos, Float2 siz) const;

	// Styling

	/// <summary>
	/// Resets all cached values that are used for styling. Many values are only checked once and not retrieved again, unless
	/// the style or theme is changed. Call this function if a change should result in changed appearance but the cached values
	/// prevent that.
	/// </summary>
	/// <param name="inherited">For container controls, whether to clear the style cache for controls in the container</param>
	/// <returns></returns>
	API_FUNCTION() void ClearStyleCache(bool inherited = false);
	
	/// <summary>
	/// Tries to retrieve the painter for this control. The painter is choosen by the style from a value set in the theme.
	/// </summary>
	/// <param name="token">The token for accessing the painter</param>
	/// <returns>The element painter associated with the token</returns>
	API_FUNCTION() FudgetElementPainter* GetElementPainter();

	/// <summary>
	/// Returns an object that can provide properties while painting this control. For example a button might need to
	/// provide whether it is pressed or down. Not every provider can give the required values, and usually zero defaults
	/// are used in that case.
	/// </summary>
	/// <returns>An object that can provide properties to an ElementPainter</returns>
	API_PROPERTY() FudgetPainterPropertyProvider* GetPainterPropertyProvider() { return _painter_provider; }

	/// <summary>
	/// Sets an object that can provide properties while painting this control. For example a button might need to
	/// provide whether it is pressed or down. Not every provider can give the required values, and usually zero defaults
	/// are used in that case.
	/// </summary>
	/// <param name="value">Property provider to set for the control</param>
	API_PROPERTY() void SetPainterPropertyProvider(FudgetPainterPropertyProvider *value);

	/// <summary>
	/// Gets the style that was set explicitly, that decides the look of the control
	/// </summary>
	API_PROPERTY() FudgetStyle* GetStyle() const { return _style; }

	/// <summary>
	/// Sets the style that decides the look of the control. Set to null to use the default style.
	/// </summary>
	API_PROPERTY() void SetStyle(FudgetStyle *value);

	/// <summary>
	/// The style that's currently used to decide the look of the control. It is the explicitly set style if
	/// a style was passed to SetStyle or to the Style property (in C#). Otherwise it's the style that was assigned
	/// to the class, or, if that's not present, the parent class, up to the theme's default style.
	/// </summary>
	API_PROPERTY() FudgetStyle* GetActiveStyle();

	/// <summary>
	/// Get the token for the theme which gives the colors and other values used for styling the controls
	/// </summary>
	API_PROPERTY() FudgetToken GetThemeId() const { return _theme_id; }
	/// <summary>
	/// Sets the token for the theme which gives the colors and other values used for styling the controls
	/// </summary>
	API_PROPERTY() void SetThemeId(FudgetToken value);

	/// <summary>
	/// The theme data which is retrieved using the theme id set to this control, or if not set, the theme in the closets
	/// parent that has a theme. If no theme is set to any control, the main theme is returned.
	/// </summary>
	API_PROPERTY() FudgetTheme* GetActiveTheme();

	/// <summary>
	/// Returns a value for the control based on a theme token. The returned value depends on both the active style and
	/// the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token associated with the value in the active style</param>
	/// <param name="result">Variable that receives the value</param>
	/// <returns>Whether the token was found as a valid value token</returns>
	API_FUNCTION() virtual bool GetStyleValue(FudgetToken token, API_PARAM(Out) Variant &result);

	/// <summary>
	/// Returns a color for the control based on a theme token. The returned value depends on both the active style and
	/// the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token associated with the color in the active style</param>
	/// <param name="result">Variable that receives the color</param>
	/// <returns>Whether the token was found as a valid color token</returns>
	API_FUNCTION() virtual bool GetStyleColor(FudgetToken token, API_PARAM(Out) Color &result);

	/// <summary>
	/// Returns a float value for the control based on a theme token. The returned value depends on both the active style
	/// and the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token associated with the float value in the active style</param>
	/// <param name="result">Variable that receives the float</param>
	/// <returns>Whether the token was found as a valid float token</returns>
	API_FUNCTION() virtual bool GetStyleFloat(FudgetToken token, API_PARAM(Out) float &result);

	/// <summary>
	/// Returns settings for filling a rectangular area for the control based on a theme token. The functions result is
	/// valid both if the token refers to a simple color or if it refers to the FudgetFillAreaSettings structure. In the
	/// first case, the area type will be set to Color.
	/// The returned value depends on both the active style and the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token associated with the FudgetFillAreaSettings value in the active style</param>
	/// <param name="result">Variable that receives the settings</param>
	/// <returns>Whether the token was found and references a valid fill area settings object</returns>
	API_FUNCTION() bool GetStyleFillSettings(FudgetToken token, API_PARAM(Out) FudgetFillAreaSettings &result);

	/// <summary>
	/// Use this function to check settings for a font in the style, when font creation is not necessary.
	/// To create the font and get its cached value directly, call GetStyleFont instead.
	/// Returns settings for a font asset, which includes its type token, size and styles. 
	/// The returned value depends on both the active style and the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token for the FudgetFontSettings value in the active style</param>
	/// <param name="result">Structure with font creation settings</param>
	/// <returns>Whether the token was found and references a valid font settings object</returns>
	API_FUNCTION() bool GetStyleFontSettings(FudgetToken token, API_PARAM(Out) FudgetFontSettings &result);

	/// <summary>
	/// Returns a struct with a font object and the settings that were used to create it. The control might have
	/// to create the font the first time this function is called. In case/ any font data should be updated, call
	/// ResetCreatedFonts.
	/// The returned value depends on both the active style and the theme currently set for this control.
	/// </summary>
	/// <param name="token">Token for a FudgetFontSettings value in the active style</param>
	/// <param name="result">Structure with font object and its settings</param>
	/// <returns></returns>
	API_FUNCTION() bool GetStyleFont(FudgetToken token, API_PARAM(OUT) FudgetFont &result);

	/// <summary>
	/// Clears cached font data that was generated by calls to GetStyleFont.
	/// </summary>
	API_FUNCTION() void ResetCreatedFonts();

	// Serialization

	void Serialize(SerializeStream& stream, const void* otherObj) override;
	void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

protected:

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
	API_FUNCTION() virtual FudgetControlFlags GetInitFlags() const { return FudgetControlFlags::None; }

private:

	void DrawTiled(GPUTexture *t, SpriteHandle sprite_handle, bool point, Float2 size, const Rectangle& rect, const Color& color) const;

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
	/// Directly changes the position and size of the control. Only to be called by FudgetLayout.
	/// </summary>
	/// <param name="pos">The new position</param>
	/// <param name="size">The new size</param>
	virtual void LayoutUpdate(Float2 pos, Float2 size);

	void CreateClassTokens();

	FudgetContainer *_parent;
	int _index;
	String _name;

	FudgetControlFlags _flags;

	Float2 _pos;
	Float2 _size;

	Float2 _hint_size;
	Float2 _min_size;
	Float2 _max_size;

	// This is reset on each Draw and calculated when needed to simplify GlobalToLocal and LocalToGlobal in draw calls.
	// It's only used during the drawing functions. It can be manually reset when needed
	mutable Float2 _cached_global_to_local_translation;
	// True when _cached_global_to_local_translation is valid.
	mutable bool _g2l_was_cached;

	// Used locally to avoid double calling functions from the parent.
	bool _changing;

	// The control's Update function is called with a delta time if this is true.
	bool _updating_registered;

	// TODO: if it is requested, this can be made available. Warning: SetElementPainter function is not implemented yet.
	//// The element painter set for this control to draw it. If not set, the painter is retrieved based on the class
	//// hierarchy and stored in _cached_painter.
	//FudgetElementPainter *_element_painter;

	// The cached element painter used to draw the full control. This is set during first draw and is not changed until
	// the cached style values are reset with ClearStyleCache, or invalidated by a change of theme or style.
	FudgetElementPainter *_cached_painter;

	// Null or the style used to decide the look of the control. When null, the style is decided based on class name.
	FudgetStyle *_style;

	// Style used for drawing the control if a specific style hasn't been set. The style is determined based on the
	// class name, if such a style is present, or the parent class name etc. If none are found, the theme's basic style
	// is used, but it might lack required settings.
	FudgetStyle *_cached_style;

	// The token for the theme set for this control. If not set, nearest parent's theme is used with a set theme. If none
	// found, the main theme is assumed.
	FudgetToken _theme_id;

	// The theme used for drawing determined by the _theme_id or the nearest parent's theme id. Resolved once on draw.
	FudgetTheme *_cached_theme;

	// A list of tokens created for this control that includes tokens for the name of every control in the inheritance
	// chain, up to FudgetControl. It is mainly used to get a style appropriate for this control when drawing.
	// The tokens might not be created until the control needs to access the active style.
	Array<FudgetToken> _class_token;

	UniquePtr<FudgetPainterPropertyProvider> _painter_provider;

	std::map<FudgetToken, FudgetFont> _cached_fonts;

	friend class FudgetLayout;
	friend class FudgetContainer;
	friend class FudgetGUIRoot;
};
