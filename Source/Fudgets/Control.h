#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"


class FudgetContainer;


/// <summary>
/// Used for any function call in controls and layouts that need one specific size of controls.
/// </summary>
API_ENUM()
enum class FudgetSizeType : uint8
{
	Hint,
	Min,
	Max,
	Current,

	None,
	All
};


/// <summary>
/// Base class for controls, including containers and panels.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetControl : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE(FudgetControl);
public:
	//FudgetControl();

	/// <summary>
	/// Called when redrawing the control. Inherited controls can call Render2D methods here.
	/// </summary>
	API_FUNCTION()
	virtual void Draw() {}

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
	API_PROPERTY() void SetMinSize(Float2 value);

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
	API_PROPERTY() void SetMaxSize(Float2 value);

	/// <summary>
	/// Gets one of the possible sizes of the control. Use as an alternative to GetHintSize, GetMinSize, GetMaxSize
	/// and GetSize.
	/// </summary>
	/// <param name="type">The size to return</param>
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
	API_PROPERTY() virtual Float2 GetPosition() const;

	/// <summary>
	/// Modifies the position of the control in its parent's layout, relative to the top-left corner, if
	/// the layout allows free positioning.
	/// </summary>
	/// <param name="value">The requested new position</param>
	API_PROPERTY() virtual void SetPosition(Float2 value);

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
	/// Called on each frame if the control is registered to receive events
	/// </summary>
	/// <param name="delta_time">The time passed since the last update</param>
	API_FUNCTION() virtual void Update(float delta_time) {}

protected:
	/// <summary>
	/// Notifies the parent to mark its layout as dirty, causing a calculation of child control sizes and positions
	/// </summary>
	/// <returns></returns>
	API_FUNCTION() void MakeParentLayoutDirty(FudgetSizeType sizeType);
private:
	void LayoutUpdate(Float2 pos, Float2 size);

	FudgetContainer *_parent;
	int _index;

	Float2 _pos;
	Float2 _size;

	Float2 _hint_size;
	Float2 _min_size;
	Float2 _max_size;

	// Used locally to avoid double calling functions from the parent.
	bool _changing;

	friend class FudgetLayout;
};
