#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Collections/Array.h"

class FudgetControl;
class FudgetContainer;

enum class FudgetSizeType : uint8;


/// <summary>
/// Horizontal alignment of a control in the slots or cells of layouts that can align controls horizontally in different ways.
/// </summary>
API_ENUM()
enum class FudgetAlignHorizontal
{
	Left,
	Center,
	Right,
	Fill,
	FillLeft,
	FillCenter,
	FillRight,
};

/// <summary>
/// Vertical alignment of a control in the slots or cells of layouts that can align controls vertically in different ways.
/// </summary>
API_ENUM()
enum class FudgetAlignVertical
{
	Top,
	Center,
	Bottom,
	Fill,
	FillTop,
	FillCenter,
	FillBottom,
};

/// <summary>
/// Orientation of layouts that that can be either horizontal or vertical.
/// </summary>
API_ENUM()
enum class FudgetOrientation
{
	Horizontal,
	Vertical
};



/// <summary>
/// Base class for "slots" in a layout that can be used to assign properties for controls for layouting
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetLayoutSlot : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayoutSlot);

	FudgetLayoutSlot(FudgetControl *control);

	// Hide in editor
	FudgetControl *_control;
	Float2 _hint_size;
	Float2 _max_size;
	Float2 _min_size;
};


/// <summary>
/// Base class for all layouts that can be assigned to container controls. Layouts don't directly contain controls,
/// instead they access the controls of the container to set sizes and positions
/// </summary>
API_CLASS(Abstract, NoSpawn)
class FUDGETS_API FudgetLayout : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayout);
public:
	FudgetLayout();
	~FudgetLayout();

	/// <summary>
	/// Gets the container that holds the controls this layout can reposition or resize
	/// </summary>
	/// <returns>The container the layout is attached to</returns>
	API_PROPERTY() FudgetContainer* GetOwner() const { return _owner; }

	/// <summary>
	/// Sets the container that holds the controls this layout can reposition or resize
	/// </summary>
	/// <param name="value">The new container this layout will be assined to</param>
	API_PROPERTY() void SetOwner(FudgetContainer *value);

	/// <summary>
	/// Gets the dirty state of the stored values in the layout and child control slots. If this is true,
	/// the controls layout will be recalculated on the next frame.
	/// </summary>
	/// <returns>Whether the children controls of the container need recalculation.</returns>
	API_PROPERTY() bool IsDirty() const { return _dirty; }
	/// <summary>
	/// Marks the layout's stored values of child control slot sizes and positions as dirty, making a recalculation
	/// necessary for next frame. Call if some value changed in a control that might require changes in the layout.
	/// Any change requires new layout calculation, but not necessarily size calculations.
	/// </summary>
	/// <param name="sizeType">Which sizes need recalculation. This can be Hint, Min, Max or All.</param>
	API_FUNCTION() virtual void MakeDirty(FudgetSizeType sizeType);

	/// <summary>
	/// Returns the calculated preferred size of the layout with all the managed controls laid out at the ideal size.
	/// This might require recalculation if the preferred size of any control changed or a new control was added.
	/// </summary>
	/// <returns>The layout's preferred size</returns>
	API_PROPERTY() Float2 GetHintSize() const;

	/// <summary>
	/// Returns the calculated minimum size of the layout with all the managed controls laid out with their smallest size.
	/// This might require recalculation if the minimum size of any control changed or a new control was added.
	/// </summary>
	/// <returns>The layout's minimum size</returns>
	API_PROPERTY() Float2 GetMinSize() const;

	/// <summary>
	/// Returns the calculated maximum size of the layout with all the managed controls laid out with their largest size.
	/// This might require recalculation if the maximum size of any control changed or a new control was added.
	/// </summary>
	/// <returns>The layout's maximum size</returns>
	API_PROPERTY() Float2 GetMaxSize() const;

	/// <summary>
	/// TODO
	/// </summary>
	void OnDeleteObject() override;

	/// <summary>
	/// Calculates the child controls position and size on the owner container if necessary or requested.
	/// </summary>
	/// <param name="forced">Whether to recalculate layouts even if the layout is not dirty.</param>
	API_FUNCTION() virtual void LayoutChildren(bool forced) = 0;
protected:
	/// <summary>
	/// Directly sets a control's position and size on the owner container.
	/// </summary>
	/// <param name="index">The index of the control</param>
	/// <param name="pos">The calculated position of the control</param>
	/// <param name="size">The calculated size of the control</param>
	API_FUNCTION() virtual void SetControlDimensions(int index, Float2 pos, Float2 size);
	/// <summary>
	/// Returns the slot, which represents the attributes of a control depending on what values a layout needs.
	/// The returned value is a derived class with a type that depends on the layout.
	/// </summary>
	/// <param name="index">The index of the control to make the slot for</param>
	/// <returns>The slot for layouting attributes or null if the index is invalid</returns>
	API_FUNCTION() FudgetLayoutSlot* GetSlot(int index) const;
	/// <summary>
	/// Called during destruction or when the layout is removed from the owner container to delete the allocated
	/// slots.
	/// </summary>
	API_FUNCTION() virtual void CleanUp();
	/// <summary>
	/// Adds or recreates the slots for the controls of the owner container. Called when the owner container changed.
	/// </summary>
	API_FUNCTION() virtual void FillSlots();

	/// <summary>
	/// Updates the dirty flag to show that the layout doesn't need recalculation. Doesn't influence the recalculation
	/// of sizes that were not needed during the last layout.
	/// </summary>
	API_FUNCTION() void ClearedDirt();

	/// <summary>
	/// Calculates size of the layout that was requested or their cached value if recalculation isn't necessary.
	/// </summary>
	/// <param name="type">The size to return which can be Hint, Min, or Max</param>
	/// <returns></returns>
	API_FUNCTION() virtual Float2 GetRequestedSize(FudgetSizeType type) const = 0;
	/// <summary>
	/// Creates a slot which represents properties of a single child control on the owner container. The function
	/// in derived classes should return an object of the proper type fitting that layout.
	/// </summary>
	/// <param name="control">The control that will be inserted into the slot</param>
	/// <returns>The created object holding layouting properties of the control</returns>
	virtual FudgetLayoutSlot* CreateSlot(FudgetControl *control) = 0;

	/// <summary>
	/// Called by the owner container when a child control was added to it.
	/// </summary>
	/// <param name="control">The new control</param>
	/// <param name="index">The index the control was added or inserted at.</param>
	API_FUNCTION() virtual void ChildAdded(FudgetControl *control, int index);
	/// <summary>
	/// Called by the owner container when a child control was removed from it
	/// </summary>
	/// <param name="index">The index the control that was removed</param>
	API_FUNCTION() virtual void ChildRemoved(int index);
	/// <summary>
	/// Called by the owner container when a child control was moved to a new index
	/// </summary>
	/// <param name="from">The old index of the control</param>
	/// <param name="to">The new index of the control</param>
	API_FUNCTION() virtual void ChildMoved(int from, int to);

	FudgetLayout(const SpawnParams &params);

private:
	bool _dirty;

	FudgetContainer *_owner;
	Array<FudgetLayoutSlot*> _slots;

	mutable bool _hint_dirty;
	mutable bool _min_dirty;
	mutable bool _max_dirty;
	mutable Float2 _cached_hint;
	mutable Float2 _cached_min;
	mutable Float2 _cached_max;

	friend class FudgetContainer;
};
