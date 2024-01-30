#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Serialization/Serialization.h"

class FudgetControl;
class FudgetContainer;

enum class FudgetSizeType : uint8;
enum class FudgetDirtType : uint8;

/// <summary>
/// Horizontal alignment of a control in the slots or cells of layouts that can align controls horizontally in different ways.
/// </summary>
API_ENUM()
enum class FudgetHorzAlign
{
	/// <summary>
	/// Control should stick to the left side of the available space. If the slot is too small, the control will fill the area
	/// and this setting wil act the same as if it was Fill.
	/// </summary>
	Left,
	/// <summary>
	/// Control should be centered horizontally in the available space. If the slot is too small, the control will fill the
	/// area and this setting wil act the same as if it was Fill.
	/// </summary>
	Center,
	/// <summary>
	/// Control should stick to the right side of the available space. If the slot is too small, the control will fill the area
	/// and this setting wil act the same as if it was Fill.
	/// </summary>
	Right,
	/// <summary>
	/// Makes the control fill the available area horizontally, no matter its size.
	/// </summary>
	Fill,
	/// <summary>
	/// Control should stick to the left side of the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the right or be clipped.
	/// </summary>
	ClipLeft,
	/// <summary>
	/// Control should be centered horizontally in the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the sides or be clipped.
	/// </summary>
	ClipCenter,
	/// <summary>
	/// Control should stick to the right side of the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the left or be clipped.
	/// </summary>
	ClipRight
};

/// <summary>
/// Vertical alignment of a control in the slots or cells of layouts that can align controls vertically in different ways.
/// </summary>
API_ENUM()
enum class FudgetVertAlign
{
	/// <summary>
	/// Control should stick to the top side of the available space. If the slot is too small, the control will fill the area
	/// and this setting wil act the same as if it was Fill.
	/// </summary>
	Top,
	/// <summary>
	/// Control should be centered vertically in the available space. If the slot is too small, the control will fill the
	/// area and this setting wil act the same as if it was Fill.
	/// </summary>
	Center,
	/// <summary>
	/// Control should stick to the bottom side of the available space. If the slot is too small, the control will fill the area
	/// and this setting wil act the same as if it was Fill.
	/// </summary>
	Bottom,
	/// <summary>
	/// Makes the control fill the available area vertically, no matter its size.
	/// </summary>
	Fill,
	/// <summary>
	/// Control should stick to the top side of the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the bottom or be clipped.
	/// </summary>
	ClipTop,
	/// <summary>
	/// Control should be centered vertically in the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the sides or be clipped.
	/// </summary>
	ClipCenter,
	/// <summary>
	/// Control should stick to the bottom side of the available space. If the slot is too small, the control will keep its minimal
	/// size, and either stick out to the top or be clipped.
	/// </summary>
	ClipBottom
};

/// <summary>
/// Orientation of layouts that that can be either horizontal or vertical.
/// </summary>
API_ENUM()
enum class FudgetOrientation
{
	/// <summary>
	/// The layout will orient itself horizontally. For list layouts this means items are placed from left to right
	/// </summary>
	Horizontal,
	/// <summary>
	/// The layout will orient itself vertically. For list layouts this means items are placed from top to bottom
	/// </summary>
	Vertical
};

/// <summary>
/// Flags that indicate what change will result in dirty layouts and sizes.
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetLayoutFlag : uint16
{
	/// <summary>
	/// The empty flag with no effect
	/// </summary>
	None = 0,
	/// <summary>
	/// Layout calculation is necessary if the owner container was resized.
	/// </summary>
	LayoutOnContainerResize = 1 << 0,
	/// <summary>
	/// Layout calculation is necessary if the owner container's position changed.
	/// </summary>
	LayoutOnContainerReposition = 1 << 1,
	/// <summary>
	/// Layout calculation is necessary if a child control was resized.
	/// </summary>
	LayoutOnContentResize = 1 << 2,
	/// <summary>
	/// Layout calculation is necessary if a child control's position changed.
	/// </summary>
	LayoutOnContentReposition = 1 << 3,
	/// <summary>
	/// Size calculation is necessary if the owner container ws resized.
	/// </summary>
	ResizeOnContainerResize = 1 << 4,
	/// <summary>
	/// Size calculation is necessary if the owner container's position changed.
	/// </summary>
	ResizeOnContainerReposition = 1 << 5,
	/// <summary>
	/// Size calculation is necessary if a child control was resized.
	/// </summary>
	ResizeOnContentResize = 1 << 6,
	/// <summary>
	/// Size calculation is necessary if a child control's position changed.
	/// </summary>
	ResizeOnContentReposition = 1 << 7,

	/// <summary>
	/// Layout calculation is necessary if a child control's index changed.
	/// </summary>
	LayoutOnContentIndexChange = 1 << 8,
	/// <summary>
	/// Size calculation is necessary if a child control's index changed.
	/// </summary>
	ResizeOnContentIndexChange = 1 << 9,
};
DECLARE_ENUM_OPERATORS(FudgetLayoutFlag);


/// <summary>
/// Base class for "slots" in a layout that can be used to assign properties for controls for layouting
/// </summary>
API_CLASS(NoSpawn)
class FUDGETS_API FudgetLayoutSlot : public ScriptingObject
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayoutSlot);

	FudgetLayoutSlot(FudgetControl *control);

	/// <summary>
	/// Fetches the control that is positioned and sized by the values in this slot
	/// </summary>
	/// <returns>The control in this slot</returns>
	API_PROPERTY() FudgetControl* GetControl() const { return _control; }

	/// <summary>
	/// The control placed in the slot. The attributes affect its position and size 
	/// </summary>
	API_FIELD() FudgetControl *_control;
	/// <summary>
	/// The calculated hint size for caching
	/// </summary>
	API_FIELD() Float2 _hint_size;
	/// <summary>
	/// The calculated min size for caching
	/// </summary>
	API_FIELD() Float2 _min_size;
	/// <summary>
	/// The calculated max size for caching
	/// </summary>
	API_FIELD() Float2 _max_size;
};


/// <summary>
/// Base class for all layouts that can be assigned to container controls. Layouts don't directly contain
/// controls, instead they access the controls of the container to set sizes and positions. To create a
/// new layout, see the implementation of StackLayout as a minimal example and for more explanation.
/// </summary>
API_CLASS(Abstract, NoSpawn)
class FUDGETS_API FudgetLayout : public ScriptingObject, public ISerializable
{
	using Base = ScriptingObject;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetLayout);
public:
	FudgetLayout();
	FudgetLayout(FudgetLayoutFlag flags);
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
	/// Dirty layout means that due to changes of the owner container or its children, the layout of
	/// the child controls need to be recalculated. This is done on the next frame.
	/// </summary>
	/// <returns>Whether layout recalculation is necessary</returns>
	API_PROPERTY() bool IsLayoutDirty() const { return _layout_dirty; }

	/// <summary>
	/// Dirty size means that due to changes of the owner container or its children, the stored size
	/// values are out of date and need to be recalculated. This is done on the next frame.
	/// </summary>
	/// <returns>Whether size recalculation is necessary</returns>
	API_PROPERTY() bool IsSizeDirty() const { return _size_dirty; }

	/// <summary>
	/// Notifies the layout that its owner container could have changed since the last layout or measurement
	/// calculations. This can mean that the layout itself needs to update the child controls inside the
	/// container. Layouts that don't depend on the specific change are free to ignore this call.
	/// </summary>
	/// <param name="dirt_flags">What changed in the container from the size, the position or both</param>
	/// <param name="content_changed">True if the changed was caused by a child control or layout</param>
	API_FUNCTION() virtual void MarkDirty(FudgetDirtType dirt_flags, bool content_changed);

	/// <summary>
	/// Sets the dirty flag for any change that happened during layout recalculation in a parent's layout. The
	/// layout is calculated from the top, so when this function is called, this layout will be calculated on the
	/// same frame.
	/// </summary>
	/// <param name="dirt_flags"></param>
	API_FUNCTION() virtual void MarkDirtyOnLayoutUpdate(FudgetDirtType dirt_flags);

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
	/// Calculates the child controls position and size on the owner container if necessary or requested.
	/// </summary>
	/// <param name="forced">Whether to recalculate layouts even if the layout is not dirty.</param>
	API_FUNCTION() void RequestLayoutChildren(bool forced);

	void Serialize(SerializeStream& stream, const void* otherObj) override;
	void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;
protected:
	/// <summary>
	/// Checks the index argument and determines if it is a valid index for accessing a slot on the layout
	/// </summary>
	/// <param name="index">The index to check</param>
	/// <returns>True on valid and false on invalid</returns>
	API_FUNCTION() bool GoodSlotIndex(int index) const;

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
	/// Returns one of the sizes of the layout. If the size data is dirty, it first calls RequestSize to recalculate
	/// them, otherwise no calculation takes place.
	/// </summary>
	/// <param name="type">Which size to return</param>
	/// <returns>The calculated size</returns>
	API_FUNCTION() Float2 GetRequestedSize(FudgetSizeType type) const;

	/// <summary>
	/// Calculates one of the size data, and then returns it. Always call GetRequestedSize() which first checks if
	/// previously calculated values are still valid, to avoid recalculation when unnecessary.
	/// </summary>
	/// <param name="type">Which size to calculate and return</param>
	/// <returns>The calculated size</returns>
	API_FUNCTION() virtual Float2 RequestSize(FudgetSizeType type) const = 0;

	/// <summary>
	/// Calculates the child controls position and size on the owner container.
	/// </summary>
	/// <returns>Whether the layout was successful and the dirty flag can be cleared</returns>
	API_FUNCTION() virtual bool LayoutChildren() = 0;

	/// <summary>
	/// Creates a slot which represents properties of a single child control on the owner container. The function
	/// in derived classes should return an object of the proper type fitting that layout.
	/// </summary>
	/// <param name="control">The control that will be inserted into the slot</param>
	/// <returns>The created object holding layouting properties of the control</returns>
	API_FUNCTION() virtual FudgetLayoutSlot* CreateSlot(FudgetControl *control) = 0;

	/// <summary>
	/// Called by the owner container when a child control was added to it. Checks which recalculations
	/// are necessary, marking them dirty. This can cause recalculations in other containers too.
	/// </summary>
	/// <param name="control">The new control</param>
	/// <param name="index">The index the control was added or inserted at.</param>
	API_FUNCTION() virtual void ChildAdded(FudgetControl *control, int index);

	/// <summary>
	/// Called by the owner container when a child control was removed.  Checks which recalculations
	/// are necessary, marking them dirty. This can cause recalculations in other containers too.
	/// </summary>
	/// <param name="index">The index the control that was removed</param>
	API_FUNCTION() virtual void ChildRemoved(int index);

	/// <summary>
	/// Called by the owner container when a child control was moved to a new index. Checks which recalculations
	/// are necessary, marking them dirty. This can cause recalculations in other containers too.
	/// </summary>
	/// <param name="from">The old index of the control</param>
	/// <param name="to">The new index of the control</param>
	API_FUNCTION() virtual void ChildMoved(int from, int to);

	/// <summary>
	/// Called by the owner container when all child controls were removed from it
	/// </summary>
	API_FUNCTION() virtual void AllDeleted();

	/// <summary>
	/// Determines if the control is allowed to update its position directly, instead of being positioned
	/// by its container or this layout. The call is invalid if the control doesn't belong to the container.
	/// The default implementation always returns false.
	/// </summary>
	/// <param name="control">The control requesting position change</param>
	/// <returns>Whether the control can change its position or not</returns>
	API_FUNCTION() bool IsControlPositioningPermitted(const FudgetControl *control) const;

	/// <summary>
	/// Describes what changes will cause a layout to mark itself dirty.
	/// </summary>
	/// <returns>The flags set for the layout</returns>
	API_PROPERTY() FudgetLayoutFlag GetLayoutFlags() const;

	/// <summary>
	/// Describes what changes will cause a layout to mark itself dirty.
	/// </summary>
	/// <param name="flags">The new flags</param>
	API_PROPERTY() void SetLayoutFlags(FudgetLayoutFlag flags);

	/// <summary>
	/// Matches the given flags with those of the layout, returning true only if all the flags were found.
	/// </summary>
	/// <param name="flags">The flags to look for</param>
	/// <returns>Whether the flags were all found on the layout</returns>
	API_FUNCTION() bool HasAllFlags(FudgetLayoutFlag flags) const;

	/// <summary>
	/// Matches the given flags with those of the layout, returning true if any of the flags was found.
	/// </summary>
	/// <param name="flags">The flags to look for</param>
	/// <returns>Whether at least one value was matching the layout flags</returns>
	API_FUNCTION() bool HasAnyFlag(FudgetLayoutFlag flags) const;


	FudgetLayout(const SpawnParams &params);
	FudgetLayout(const SpawnParams &params, FudgetLayoutFlag _flags);

private:
	/// <summary>
	/// Sets the container that holds the controls this layout can reposition or resize. For internal use, it doesn't
	/// notify the container of the changes. Use SetOwner for normal use.
	/// </summary>
	/// <param name="value">The new container this layout will be assined to</param>
	API_PROPERTY() void SetOwnerInternal(FudgetContainer *value);


	FudgetContainer *_owner;
	Array<FudgetLayoutSlot*> _slots;

	bool _layout_dirty;
	mutable bool _size_dirty;

	mutable Float2 _cached_hint;
	mutable Float2 _cached_min;
	mutable Float2 _cached_max;

	FudgetLayoutFlag _flags;

	bool _changing;

	friend class FudgetContainer;
};