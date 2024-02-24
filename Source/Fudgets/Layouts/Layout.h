#pragma once

#include "Engine/Scripting/ScriptingObject.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Serialization/Serialization.h"
#include "Engine/Core/Math/Rectangle.h"

class FudgetControl;
class FudgetContainer;

enum class FudgetSizeType : uint8;
enum class FudgetLayoutDirtyReason : uint8;

/// <summary>
/// Horizontal alignment of a control in the slots or cells of layouts that can align controls horizontally in different ways.
/// </summary>
API_ENUM()
enum class FudgetLayoutHorzAlign
{
    /// <summary>
    /// The control should stick to the left side of the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Left,
    /// <summary>
    /// The control should be centered in the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Center,
    /// <summary>
    /// The control should stick to the right side of the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Right,
    /// <summary>
    /// The control should stick to the left side of the layout slot. If the slot is bigger, the control will be resized but
    /// won't be larger than its maximum size. If the slot is too small, the control will fill the area and this setting wil
    /// act the same as if it was Fill.
    /// </summary>
    LeftGrow,
    /// <summary>
    /// The control should be centered in the layout slot. If the slot is bigger, the control will be resized but won't
    /// be larger than its maximum size. If the slot is too small, the control will fill the area and this setting will act the
    /// same as if it was Fill.
    /// </summary>
    CenterGrow,
    /// <summary>
    /// The control should stick to the right side of the layout slot. If the slot is bigger, the control will be resized but
    /// won't be larger than its maximum size. If the slot is too small, the control will fill the area and this setting will act
    /// the same as if it was Fill.
    /// </summary>
    RightGrow,
    /// <summary>
    /// The control should fill the available space in its slot horizontally.
    /// </summary>
    Fill,
    /// <summary>
    /// The control should stick to the left side of the layout slot. If the slot is too small, the control will keep its minimal
    /// size, and either stick out to the right or be clipped.
    /// </summary>
    ClipLeft,
    /// <summary>
    /// The control should be centered horizontally in the layout slot. If the slot is too small, the control will keep its minimal
    /// size, and either stick out to the sides or be clipped.
    /// </summary>
    ClipCenter,
    /// <summary>
    /// The control should stick to the right side of the layout slot. If the slot is too small, the control will keep its minimal
    /// size, and either stick out to the left or be clipped.
    /// </summary>
    ClipRight
};

/// <summary>
/// Vertical alignment of a control in the slots or cells of layouts that can align controls vertically in different ways.
/// </summary>
API_ENUM()
enum class FudgetLayoutVertAlign
{
    /// <summary>
    /// The control should stick to the top of the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Top,
    /// <summary>
    /// The control should be centered in the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Center,
    /// <summary>
    /// The control should stick to the bottom of the layout slot. If the slot is bigger, the control will keep its hint size.
    /// If the slot is too small, the control will fill the area and this setting will act the same as if it was Fill.
    /// </summary>
    Bottom,
    /// <summary>
    /// The control should stick to the top of the layout slot. If the slot is bigger, the control will be resized but
    /// won't be larger than its maximum size. If the slot is too small, the control will fill the area and this setting wil
    /// act the same as if it was Fill.
    /// </summary>
    TopGrow,
    /// <summary>
    /// The control should be centered in the layout slot. If the slot is bigger, the control will be resized but won't
    /// be larger than its maximum size. If the slot is too small, the control will fill the area and this setting will act the
    /// same as if it was Fill.
    /// </summary>
    CenterGrow,
    /// <summary>
    /// The control should stick to the bottom of the layout slot. If the slot is bigger, the control will be resized but
    /// won't be larger than its maximum size. If the slot is too small, the control will fill the area and this setting will act
    /// the same as if it was Fill.
    /// </summary>
    BottomGrow,
    /// <summary>
    /// The control should fill the available space in its slot horizontally.
    /// </summary>
    Fill,
    /// <summary>
    /// The control should stick to the top of the layout slot. If the slot is too small, the control will keep its minimal
    /// size, and either stick out to the bottom or be clipped.
    /// </summary>
    ClipTop,
    /// <summary>
    /// The control should be centered horizontally in the layout slot. If the slot is too small, the control will keep its minimal
    /// size, and either stick out to the sides or be clipped.
    /// </summary>
    ClipCenter,
    /// <summary>
    /// The control should stick to the bottom of the layout slot. If the slot is too small, the control will keep its minimal
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
/// influence the size of its container.
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
    /// Layout calculation is necessary if a child control's position changed. This can be used
    /// in layouts to mark themselves dirty even if child controls can't manually change position,
    /// when a slot setting requires control position change.
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
/// Measurements of a control in a layout, which depends on the available space for some controls
/// </summary>
API_STRUCT()
struct FUDGETS_API FudgetLayoutSizeCache
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(FudgetLayoutSizeCache);

    FudgetLayoutSizeCache() : IsValid(false), Space(MAX_float - 1.0f), Size(Float2::Zero), Min(Float2::Zero), Max(Float2::Zero), SizeFromSpace(false) {}
    FudgetLayoutSizeCache(Float2 space) : IsValid(true), Space(space), Size(Float2::Zero), Min(Float2::Zero), Max(Float2::Zero), SizeFromSpace(false) {}
    FudgetLayoutSizeCache(Float2 space, Float2 size, Float2 min, Float2 max, bool size_from_space = false) : IsValid(true), Space(space), Size(size), Min(min), Max(max), SizeFromSpace(size_from_space) {}

    /// <summary>
    /// Whether the data in this cache is valid and hasn't been invalidated.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") bool IsValid;
    /// <summary>
    /// Space provided in measure used to calculate the wanted sizes
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 Space;
    /// <summary>
    /// The optimal size for a control.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 Size;
    /// <summary>
    /// The minimum size requested by a control.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 Min;
    /// <summary>
    /// The maximum size requested by a control.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Float2 Max;

    /// <summary>
    /// Same as the control flag. If the available space changes, the control's sizes might change as well. Can be
    /// true for a container that holds a control with this flag or has a layout with such behavior.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") bool SizeFromSpace;
};

/// <summary>
/// Base class for "slots" in a layout that can be used to assign properties for controls for layouting
/// </summary>
API_CLASS()
class FUDGETS_API FudgetLayoutSlot : public ScriptingObject
{
    using Base = ScriptingObject;
    DECLARE_SCRIPTING_TYPE(FudgetLayoutSlot);

public:
    ///// <summary>
    ///// Fetches the control that is positioned and sized by the values in this slot
    ///// </summary>
    ///// <returns>The control in this slot</returns>
    //API_PROPERTY(Attributes="HideInEditor") FudgetControl* GetControl() const { return _control; }

    /// <summary>
    /// The control placed in the slot. The attributes affect its position and size 
    /// </summary>
    API_FIELD(Attributes = "HideInEditor") FudgetControl *Control;

    /// <summary>
    /// Size of the control at the start of layouting. Used for determining if the control needs to update its
    /// own layout after the layout is calculated. Controls with the SizeDependsOnSpace flag are asked to
    /// calculate their sizes again if the new size doesn't match the old one.
    /// </summary>
    API_FIELD(Attributes="HideInEditor, NoSerialize") FudgetLayoutSizeCache OldSizes;

    /// <summary>
    /// Dimensions of a control when it can take all the space it requested.
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") FudgetLayoutSizeCache UnrestrictedSizes;
    /// <summary>
    /// Dimensions the control wishes to have, provided a space it can occupy. Usually calculated from the hint size
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") FudgetLayoutSizeCache Sizes;


    /// <summary>
    /// Temporary value used during layout calculation
    /// </summary>
    API_FIELD(Attributes = "HideInEditor, NoSerialize") Rectangle ComputedBounds;

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
    /// Notifies the layout that either its owner container, or a child control changed since the last layout
    /// or size measurement calculation, and the change affects the managed controls' size or position within the
    /// layout.
    /// Call MarkDirty directly if a change in the layout or one of its slots only affects the managed controls.
    /// Call MarkOwnerDirty, if a change affects the owner container's size. For example a control's alignment
    /// in its slot doesn't change the owner's size, while adding padding to it might.
    /// </summary>
    /// <param name="dirt_reason">What changed in the container or contents</param>
    /// <returns>Whether the size of the layout could change after a layout calculation</returns>
    API_FUNCTION() virtual bool MarkDirty(FudgetLayoutDirtyReason dirt_reason);

    /// <summary>
    /// Notifies the layout that the size of one of its child controls changed and it needs to be measured.
    /// This does not mark the layout dirty and won't result in updating the control sizes unless MarkDirty
    /// is also called.
    /// </summary>
    /// <param name="index">Index of the control in the owner container</param>
    API_FUNCTION() virtual void MarkControlSizesDirty(int index);

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
    /// The return value is invalid for dirty layouts.
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
    /// Whether the layout or any of its child controls can change their requested sizes depending on the available space.
    /// </summary>
    API_PROPERTY(Attributes = "HideInEditor") bool SizeDependsOnSpace() const;

    /// <summary>
    /// Calculates the child controls position and size on the owner container if necessary or requested.
    /// </summary>
    /// <param name="forced">Whether to recalculate layouts even if the layout is not dirty.</param>
    API_FUNCTION() void RequestLayoutChildren(bool forced);

    /// <summary>
    /// Use in measurement and layout functions to determine if the space for the layout is unrestricted or not.
    /// </summary>
    /// <param name="space">The size of the available space to check</param>
    /// <returns>Whether the layout can size and place its children without being limited in space</returns>
    API_FUNCTION() FORCE_INLINE static bool IsUnrestrictedSpace(Float2 space) { return space.X < 0.f || space.Y < 0.f; }

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
    /// Called once before starting the layouting calculations. Layouts can use this function to pre-calculate values.
    /// For example the available space could be stored after excluding all padding sizes, which wouldn't change between
    /// steps of layout calculation.
    /// The default implementation resets the computed sizes of all slots. Call the base implementation if this is
    /// desirable.
    /// </summary>
    /// <param name="space">The available space for the layout contents. Check with IsUnrestrictedSpace to see if this
    /// value is valid</param>
    API_FUNCTION() virtual void PreLayoutChildren(Float2 space);

    /// <summary>
    /// Calculates the bounds of the child controls in the owner container, using the available space and slot size
    /// measurements. Call SetMeasuredSizes at least once to provide the parent layouts the appropriate sizes of this
    /// layout.
    /// </summary>
    /// <param name="space">The available space for the layout contents. Check with IsUnrestrictedSpace to see if this
    /// value is valid</param>
    API_FUNCTION() virtual void LayoutChildren(Float2 space);

    /// <summary>
    /// Calculates and sets the child controls' position and size on the owner container, using the available space, and
    /// precalculated sizes in slots. Its behavior is undefined if the sizes are not already measured with DoMeasure.
    /// In derived classes override LayoutChildren instead.
    /// </summary>
    /// <param name="space">The available space for the layout contents. Check with IsUnrestrictedSpace to see if this
    /// value is valid</param>
    API_FUNCTION() virtual void DoLayoutChildren(Float2 space);

    /// <summary>
    /// Measures the control in the slot if it doesn't have valid measurements for the passed available space. Otherwise returns
    /// the cached values. Call this in LayoutChildren instead of directly accessing the slot's control, calling its OnMeasure.
    /// </summary>
    /// <param name="index">Slot index</param>
    /// <param name="available">The available space for the layout contents. Negative values mean unrestricted space</param>
    /// <param name="result">Receives the sizes requested by the control in the slot.</param>
    API_FUNCTION() virtual bool MeasureSlot(int index, Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &wanted_min, API_PARAM(Out) Float2 &wanted_max);

    /// <summary>
    /// Call inside LayoutChildren when the layout calculated its size requirements. If the available space is unrestricted
    /// (checked by IsUnrestrictedSpace), only the layout sizes should be calculated and updating any slot layout data
    /// should be avoided. Care must be taken not to cause float overflow when calculating any of the sizes.
    /// </summary>
    /// <param name="sizes">The available space and sizes for child controls. Every member needs to be filled</param>
    API_FUNCTION() void SetMeasuredSizes(const FudgetLayoutSizeCache &sizes);

    /// <summary>
    /// Called after the end of layout calculation to place the controls in their slot. The slot size should already be calculated and saved
    /// in the ComputedBounds member of the slots. By default the layout moves the controls to fill their calculated bounds, but derived
    /// classes can fit controls in the calculated bounds in different ways, for example to align smaller controls to one side of their slot.
    /// </summary>
    /// <param name="index"></param>
    API_FUNCTION() virtual void PlaceControlInSlotRectangle(int index);

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
    /// Call MarkOwnerDirty, if a change in the layout or one of its slots affects the owner container's size. Call
    /// MarkDirty directly if a change only affects the managed controls. For example a control's alignment in its
    /// slot doesn't change the owner's size, while adding padding to it might.
    /// </summary>
    API_FUNCTION() virtual void MarkOwnerDirty();

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

    /// <summary>
    /// The child control positions and possibly sizes need to be recalculated, due to a change
    /// </summary>
    bool _layout_dirty;

    /// <summary>
    /// Cached measurements of the layout contents
    /// </summary>
    FudgetLayoutSizeCache _sizes;
    /// <summary>
    /// Cached measurements of the layout contents when the available space is unlimited
    /// </summary>
    FudgetLayoutSizeCache _unrestricted_sizes;

    // Flags determining what can cause the layout to become dirty
    FudgetLayoutFlag _flags;

    bool _changing;

    friend class FudgetContainer;
};