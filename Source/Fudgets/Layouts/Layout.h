#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Serialization/Serialization.h"

class FudgetControl;
class FudgetContainer;

enum class FudgetSizeType : uint8;
enum class FudgetLayoutDirtyReason : uint8;

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
/// Flags that indicate what change will result in dirty layouts and sizes, and how the layout's size might
/// influence the size of its container. If no LayoutOn... and ResizeOn... flags are specified, the layout
/// allows the owner container's controls to size and position themselves.
/// </summary>
API_ENUM(Attributes = "Flags")
enum class FudgetLayoutFlag
{
    /// <summary>
    /// Empty flag
    /// </summary>
    None = 0,

    /// <summary>
    /// Layout calculation is necessary if the available space in the parent container changes
    /// </summary>
    LayoutOnContainerResize = 1 << 0,
    /// <summary>
    /// Layout calculation is necessary if a child control was resized.
    /// </summary>
    LayoutOnContentResize = 1 << 1,
    /// <summary>
    /// Layout calculation is necessary if the owner container's position changed.
    /// </summary>
    LayoutOnContainerReposition = 1 << 2,
    /// <summary>
    /// Layout calculation is necessary if a child control's position changed.
    /// </summary>
    LayoutOnContentReposition = 1 << 3,
    /// <summary>
    /// Layout calculation is necessary if the owner container's index changed in its parent.
    /// </summary>
    LayoutOnContainerIndexChange = 1 << 4,
    /// <summary>
    /// Layout calculation is necessary if a child control's index changed.
    /// </summary>
    LayoutOnContentIndexChange = 1 << 4,
    /// <summary>
    /// The sizes provided by the layout change if the available space in the parent container changes
    /// </summary>
    ResizeOnContainerResize = 1 << 5,
    /// <summary>
    /// The sizes provided by the layout change if a child control was resized.
    /// </summary>
    ResizeOnContentResize = 1 << 6,
    /// <summary>
    /// The sizes provided by the layout change if the owner container's position changed.
    /// </summary>
    ResizeOnContainerReposition = 1 << 7,
    /// <summary>
    /// The sizes provided by the layout change if a child control's position changed.
    /// </summary>
    ResizeOnContentReposition = 1 << 8,
    /// <summary>
    /// The sizes provided by the layout change if the owner container's index changed in its parent.
    /// </summary>
    ResizeOnContainerIndexChange = 1 << 9,
    /// <summary>
    /// The sizes provided by the layout change if a child control's index changed.
    /// </summary>
    ResizeOnContentIndexChange = 1 << 9,

    /// <summary>
    /// Layout calculation is necessary for any change in the container
    /// </summary>
    LayoutOnContainerChange = LayoutOnContainerResize | LayoutOnContainerReposition | LayoutOnContainerIndexChange,
    /// <summary>
    /// Layout calculation is necessary for any change in the contents
    /// </summary>
    LayoutOnContentChange = LayoutOnContentResize | LayoutOnContentReposition | LayoutOnContentIndexChange,
    /// <summary>
    /// The sizes provided by the layout change for any change in the container
    /// </summary>
    ResizeOnContainerChange = ResizeOnContainerResize | ResizeOnContainerReposition | ResizeOnContainerIndexChange,
    /// <summary>
    /// The sizes provided by the layout change for any change in the contents
    /// </summary>
    ResizeOnContentChange = ResizeOnContentResize | ResizeOnContentReposition | ResizeOnContentIndexChange,

    /// <summary>
    /// The layout has a valid hint size width that containers can use. If this is not set,
    /// containers need to provide their own hint width.
    /// </summary>
    CanProvideHintSizeWidth = 1 << 10,
    /// <summary>
    /// The layout has a valid hint size height that containers can use. If this is not set,
    /// containers need to provide their own hint height.
    /// </summary>
    CanProvideHintSizeHeight = 1 << 11,

    /// <summary>
    /// The layout has valid hint sizes that containers can use.
    /// </summary>
    CanProvideHintSize = CanProvideHintSizeWidth | CanProvideHintSizeHeight,

    /// <summary>
    /// The layout has a valid min size width that containers can use. If this is not set,
    /// containers need to provide their own min width.
    /// </summary>
    CanProvideMinSizeWidth = 1 << 12,
    /// <summary>
    /// The layout has a valid min size height that containers can use. If this is not set,
    /// containers need to provide their own min height.
    /// </summary>
    CanProvideMinSizeHeight = 1 << 13,

    /// <summary>
    /// The layout has valid min sizes that containers can use.
    /// </summary>
    CanProvideMinSize = CanProvideMinSizeWidth | CanProvideMinSizeHeight,

    /// <summary>
    /// The layout has a valid max size width that containers can use. If this is not set,
    /// containers need to provide their own max width.
    /// </summary>
    CanProvideMaxSizeWidth = 1 << 14,
    /// <summary>
    /// The layout has a valid max size height that containers can use. If this is not set,
    /// containers need to provide their own max height.
    /// </summary>
    CanProvideMaxSizeHeight = 1 << 15,

    /// <summary>
    /// The layout has valid max sizes that containers can use.
    /// </summary>
    CanProvideMaxSize = CanProvideMaxSizeWidth | CanProvideMaxSizeHeight,

    /// <summary>
    /// The layout has a valid size of each type that containers can use.
    /// </summary>
    CanProvideSizes = CanProvideHintSize | CanProvideMinSize | CanProvideMaxSize,

    /// <summary>
    /// The layout has valid hint, min and max width size
    /// </summary>
    CanProvideSizeWidth = CanProvideHintSizeWidth | CanProvideMinSizeWidth | CanProvideMaxSizeWidth,

    /// <summary>
    /// The layout has valid hint, min and max height size
    /// </summary>
    CanProvideSizeHeight = CanProvideHintSizeHeight | CanProvideMinSizeHeight | CanProvideMaxSizeHeight,

    /// <summary>
    /// Set on the layout before assigning it to an owner container to reset its flags from GetInitFlags.
    /// Makes the flags reset the next time the layout owner changes to not null
    /// </summary>
    ResetFlags
};
DECLARE_ENUM_OPERATORS(FudgetLayoutFlag);



/// <summary>
/// Base class for "slots" in a layout that can be used to assign properties for controls for layouting
/// </summary>
API_CLASS()
class FUDGETS_API FudgetLayoutSlot : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetLayoutSlot);

public:
    /// <summary>
    /// Fetches the control that is positioned and sized by the values in this slot
    /// </summary>
    /// <returns>The control in this slot</returns>
    API_PROPERTY(Attributes="HideInEditor") FudgetControl* GetControl() const { return _control; }

    /// <summary>
    /// The control placed in the slot. The attributes affect its position and size 
    /// </summary>
    API_FIELD(Attributes = "HideInEditor") FudgetControl *_control;

    /// <summary>
    /// Size of the control at the start of layouting. Used for determining if the control needs to update its
    /// own layout after the layout is calculated. Controls with the SizeDependsOnSpace flag are asked to
    /// calculate their sizes again if the new size doesn't match the old one.
    /// </summary>
    API_FIELD(Attributes="HideInEditor, NoSerialize") Float2 _old_size;

    /// <summary>
    /// Same as the control flag. If the available space changes, the control's sizes might change as well. Can be
    /// true for a container that holds a control with this flag or has a layout with such behavior.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") bool _size_from_space;

    /// <summary>
    /// Dimensions the control wishes to have, provided a space it can occupy. Usually calculated from the hint size
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 _wanted_size;

    /// <summary>
    /// Minimum dimensions the control needs
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 _min_size;

    /// <summary>
    /// Largest size the control wants
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 _max_size;

};


/// <summary>
/// Base class for all layouts that can be assigned to container controls. Layouts don't directly contain
/// controls, instead they access the controls of the container to set sizes and positions. To create a
/// new layout, see the implementation of StackLayout as a minimal example and for more explanation.
/// </summary>
API_CLASS(Abstract)
class FUDGETS_API FudgetLayout : public ScriptingObject, public ISerializable
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetLayout);
public:
    ~FudgetLayout();

    /// <summary>
    /// Gets the container that holds the controls this layout can reposition or resize
    /// </summary>
    /// <returns>The container the layout is attached to</returns>
    API_PROPERTY(Attributes="HideInEditor") FudgetContainer* GetOwner() const { return _owner; }

    /// <summary>
    /// Sets the container that holds the controls this layout can reposition or resize
    /// </summary>
    /// <param name="value">The new container this layout will be assined to</param>
    API_PROPERTY(Attributes="HideInEditor") void SetOwner(FudgetContainer *value);

    /// <summary>
    /// Dirty layout means that due to changes of the owner container or its children, the layout of
    /// the child controls need to be recalculated. This is done on the next frame.
    /// </summary>
    /// <returns>Whether layout recalculation is necessary</returns>
    API_PROPERTY(Attributes="HideInEditor") bool IsLayoutDirty() const { return _layout_dirty; }

    /// <summary>
    ///// Dirty size means that due to changes of the owner container or its children, the stored size
    ///// values are out of date and need to be recalculated. This is done on the next frame.
    ///// </summary>
    ///// <returns>Whether size recalculation is necessary</returns>
    //API_PROPERTY(Attributes="HideInEditor") bool IsSizeDirty() const { return _size_dirty; }

    /// <summary>
    /// Notifies the layout that either its owner container, or a child control changed since the last layout
    /// or size measurement calculation. This can mean that the layout itself needs to update the child controls
    /// inside the container, or that parents need to do layout calculations up the tree. Layouts that don't
    /// depend on the specific change will not be marked dirty.
    /// </summary>
    /// <param name="dirt_reason">What changed in the container or contents</param>
    API_FUNCTION() virtual void MarkDirty(FudgetLayoutDirtyReason dirt_reason);

    /// <summary>
    /// Sets the dirty flag for any change that happened during layout recalculation in a parent's layout. The
    /// layout is calculated from the top, so when this function is called, this layout will be calculated in the
    /// same frame. Marking a layout dirty this way will not influence the parents. The Container flag doesn't need
    /// to be set in the dirt reason, because it is always effective
    /// </summary>
    /// <param name="dirt_reason">What changed in the container</param>
    API_FUNCTION() virtual void MarkDirtyOnLayoutUpdate(FudgetLayoutDirtyReason dirt_reason);

    /// <summary>
    /// Returns the calculated preferred size of the layout with all the managed controls laid out at the ideal size.
    /// This might require recalculation if the preferred size of any control changed or a new control was added.
    /// </summary>
    /// <returns>The layout's preferred size</returns>
    API_PROPERTY(Attributes="HideInEditor") Float2 GetHintSize();

    /// <summary>
    /// Returns the calculated minimum size of the layout with all the managed controls laid out with their smallest size.
    /// This might require recalculation if the minimum size of any control changed or a new control was added.
    /// </summary>
    /// <returns>The layout's minimum size</returns>
    API_PROPERTY(Attributes="HideInEditor") Float2 GetMinSize();

    /// <summary>
    /// Returns the calculated maximum size of the layout with all the managed controls laid out with their largest size.
    /// This might require recalculation if the maximum size of any control changed or a new control was added.
    /// </summary>
    /// <returns>The layout's maximum size</returns>
    API_PROPERTY(Attributes="HideInEditor") Float2 GetMaxSize();

    /// <summary>
    /// Calculates the child controls position and size on the owner container if necessary or requested.
    /// </summary>
    /// <param name="forced">Whether to recalculate layouts even if the layout is not dirty.</param>
    API_FUNCTION() void RequestLayoutChildren(bool forced);

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;
protected:
    /// <summary>
    /// Call in derived layout classes to cache the size after Measure.
    /// </summary>
    /// <param name="value">Updated size to store</param>
    API_FUNCTION() void CacheMeasureSpace(Float2 value) { _cached_space = value; }
    /// <summary>
    /// Call in derived layout classes to cache the size after Measure. This value will be used as the
    /// size until new measurements are made
    /// </summary>
    /// <param name="value">Updated size to store</param>
    API_FUNCTION() void CacheHintSize(Float2 value) { _cached_hint = value; }
    /// Call in derived layout classes to cache the size after Measure. This value will be used as the
    /// size until new measurements are made
    /// </summary>
    /// <param name="value">Updated size to store</param>
    API_FUNCTION() void CacheMinSize(Float2 value) { _cached_min = value; }
    /// Call in derived layout classes to cache the size after Measure. This value will be used as the
    /// size until new measurements are made
    /// </summary>
    /// <param name="value">Updated size to store</param>
    API_FUNCTION() void CacheMaxSize(Float2 value) { _cached_max = value; }

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
    /// Calculates and sets the child controls' position and size on the owner container, using the available space, and
    /// precalculated sizes in slots.
    /// </summary>
    /// <returns>Whether the layout was successful and the layout dirty flag can be cleared</returns>
    API_FUNCTION() virtual bool LayoutChildren() { return false; }

    /// <summary>
    /// Calculates the size of the layout contents based on a value for available width and height. If one of the
    /// available dimensions is negative, then the layout is not restricted in space in that dimension.
    /// </summary>
    /// <param name="avaliable">The available space for the layout contents or unrestricted when negative</param>
    /// <param name="wanted_size">The size requested by the layout contents. This might be larger than the available space.</param>
    /// <param name="min_size">The minimum size requied by the layout. Should be constant unless the contents change</param>
    /// <param name="min_size">The maximum size requied by the layout. Should be constant unless the contents change</param>
    /// <returns>Whether the layout's size depends on available space, or manages a control that influences this behavior</returns>
    API_FUNCTION() virtual bool Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) { ASSERT(false); }

    /// <summary>
    /// Creates a slot which represents properties of a single child control on the owner container. The function
    /// in derived classes should return an object of the proper type fitting that layout.
    /// </summary>
    /// <param name="control">The control that will be inserted into the slot</param>
    /// <returns>The created object holding layouting properties of the control</returns>
    API_FUNCTION() virtual FudgetLayoutSlot* CreateSlot(FudgetControl *control);

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
    API_FUNCTION() virtual bool IsControlPositionChangePermitted(const FudgetControl *control) const { return false; }
    /// <summary>
    /// Determines if setting the size hint of a control in its container should result in changing the control's size
    /// as well. This is usually false in a layout that is responsible for the control dimensions.
    /// The default implementation always returns false.
    /// </summary>
    /// <param name="control">The control requesting size change</param>
    /// <returns>Whether the control can update its size or not</returns>
    API_FUNCTION() virtual bool IsControlDirectSizeChangePermitted(const FudgetControl *control) const { return false; }

    /// <summary>
    /// Describes what changes will cause a layout to mark itself dirty.
    /// </summary>
    /// <returns>The flags set for the layout</returns>
    API_PROPERTY(Attributes="HideInEditor") FudgetLayoutFlag GetLayoutFlags() const;

    /// <summary>
    /// Describes what changes will cause a layout to mark itself dirty.
    /// </summary>
    /// <param name="flags">The new flags</param>
    API_PROPERTY(Attributes="HideInEditor") void SetLayoutFlags(FudgetLayoutFlag flags);

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

    /// <summary>
    /// Called when the layout is added to a container owner, to set some basic behavior for the layout. It can be modified individually
    /// for layouts with SetLayoutFlags, but it might be reset each time the owner container changes.
    /// Include the result of the base layout's GetInitFlags if it's needed for correct behavior.
    /// </summary>
    /// <returns>Layout flags that should be set to the layout.</returns>
    API_FUNCTION() virtual FudgetLayoutFlag GetInitFlags() const { return FudgetLayoutFlag::None; }
private:
    /// <summary>
    /// Sets the container that holds the controls managed by this layout. For internal use, it doesn't
    /// notify the container of the changes. Use SetOwner for normal use.
    /// </summary>
    /// <param name="value">The new container this layout will be assined to</param>
    API_PROPERTY(Attributes="HideInEditor") void SetOwnerInternal(FudgetContainer *value);

    FudgetContainer *_owner;
    Array<FudgetLayoutSlot*> _slots;

    bool _layout_dirty;
    //bool _size_dirty;

    // The available size given by the owner container used for the cached measurements. The dirty flag
    // doesn't need to be true if this is different in a new request and the LayoutOnContainerResize or
    // ResizeOnContainerResize flags are set.
    Float2 _cached_space;

    // Calculated hint size in the last call to Measure
    Float2 _cached_hint;
    // Calculated min size in the last call to Measure
    Float2 _cached_min;
    // Calculated max size in the last call to Measure
    Float2 _cached_max;

    // Flags determining what can cause the layout to become dirty
    FudgetLayoutFlag _flags;

    bool _changing;

    friend class FudgetContainer;
};