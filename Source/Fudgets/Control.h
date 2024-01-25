#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Math/Color.h"
#include "Engine/Core/Types/BaseTypes.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Engine/Serialization/Serialization.h"

#include "Styling/Token.h"


class FudgetContainer;
class FudgetGUIRoot;
class FudgetElementPainter;
class FudgetPainterPropertyProvider;

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
	/// Controls with this flag will have their Update function called. This flag needs to be set in the
	/// constructor. Controls can register to be updated at any time, by calling TODO.
	/// </summary>
	RegisterToUpdates = 1 << 9,
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
/// A set of options to decide what should happen when the mouse button was pressed over a
/// control. 
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
	FudgetControl(const SpawnParams &params, FudgetControlFlags flags);
	~FudgetControl();

	/// <summary>
	/// Called when redrawing the control. Inherited controls can call Render2D methods here.
	/// </summary>
	API_FUNCTION() virtual void Draw();

	/// <summary>
	/// Fetches the parent who is managing this control. The parent is also responsible for destruction
	/// and, in case it has a layout, to set the size and position of this control.
	/// </summary>
	/// <returns>The parent managing this control's dimensions and lifetime.</returns>
	API_PROPERTY() FudgetContainer* GetParent() const { return _parent; }

	/// <summary>
	/// Changes the parent who is managing this control. The parent is also responsible for destruction
	/// and, in case it has a layout, to set the size and position of this control.
	/// </summary>
	/// <param name="value">The new parent or null</param>
	API_PROPERTY() void SetParent(FudgetContainer *value);

	/// <summary>
	/// Changes the parent who is managing this control. The parent is also responsible for destruction
	/// and, in case it has a layout, to set the size and position of this control.
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
	/// Gets the local bounding box of the control relative to its parent, calculating the size if necessary.
	/// </summary>
	API_PROPERTY() Rectangle GetBoundingBox() const { return Rectangle(GetPosition(), GetSize()); }

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
	/// <param name="pos">Position relative to the control</param>
	/// <param name="size">Size of control</param>
	/// <param name="color">Color to use for filling</param>
	/// <param name="thickness">Thickness of the rectangle's lines</param>
	API_FUNCTION() void DrawRectangle(Float2 pos, Float2 size, Color color, float thickness = 1.2f) const;

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

	// Styling
	
	//template<typename T>
	//T* GetElementPainter(FudgetToken &token) const
	//{
	//	auto root = GetGUIRoot();
	//	if (root == nullptr)
	//		return nullptr;
	//	return root->GetTheme()->GetElementPainter<T>(token);
	//}

	/// <summary>
	/// Shortcut to get the element painter from the theme for a token. Calls the theme's GetElementPainter.
	/// </summary>
	/// <param name="token">The token for accessing the painter</param>
	/// <returns>The element painter associated with the token</returns>
	/*API_FUNCTION()*/ FudgetElementPainter* GetElementPainter(FudgetToken token) const;

	/// <summary>
	/// Returns an object that can provide properties while painting this control. For example a button might need to provide
	/// whether it is pressed or down, but not every control can return a valid answer to that.
	/// </summary>
	/// <returns>An object that can provide properties to an ElementPainter</returns>
	API_PROPERTY() virtual FudgetPainterPropertyProvider* GetPainterPropertyProvider() { return nullptr; }

	// Serialization

	void Serialize(SerializeStream& stream, const void* otherObj) override;
	void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;
private:
	static FudgetGUIRoot *_guiRoot;

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

	FudgetContainer *_parent;
	int _index;
	String _name;

	FudgetControlFlags _flags;

	Float2 _pos;
	Float2 _size;

	Float2 _hint_size;
	Float2 _min_size;
	Float2 _max_size;

	// Used locally to avoid double calling functions from the parent.
	bool _changing;

	// The control's Update function is called with a delta time if this is true.
	bool _updating_registered;

	friend class FudgetLayout;
	friend class FudgetContainer;
	friend class FudgetGUIRoot;
};
