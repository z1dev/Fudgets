#pragma once

#include "Control.h"

#include "Engine/Core/Collections/Array.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Log.h"

class FudgetLayout;
class Fudget;


enum class FudgetLayoutDirtyReason : uint8;

/// <summary>
/// Flags determining how a container measures its different sizes, or instead uses its layout's measurements
/// </summary>
API_ENUM(Attributes="Flags")
enum class FudgetSizeOverride
{
    /// <summary>
    /// The container's hint width is that of its layout if the layout provides it
    /// </summary>
    LayoutHintWidth = 1 << 0,
    /// <summary>
    /// The container's hint height is that of its layout if the layout provides it
    /// </summary>
    LayoutHintHeight = 1 << 1,
    /// <summary>
    /// The container's min width is that of its layout if the layout provides it
    /// </summary>
    LayoutMinWidth = 1 << 2,
    /// <summary>
    /// The container's min height is that of its layout if the layout provides it
    /// </summary>
    LayoutMinHeight = 1 << 3,
    /// <summary>
    /// The container's max width is that of its layout if the layout provides it
    /// </summary>
    LayoutMaxWidth = 1 << 4,
    /// <summary>
    /// The container's max height is that of its layout if the layout provides it
    /// </summary>
    LayoutMaxHeight = 1 << 5,
    /// <summary>
    /// The container's hint width is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutHintWidth.
    /// </summary>
    UnrestrictedHintWidth = LayoutHintWidth | 1 << 6,
    /// <summary>
    /// The container's hint height is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutHintHeight.
    /// </summary>
    UnrestrictedHintHeight = LayoutHintHeight | 1 << 7,
    /// <summary>
    /// The container's min width is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutMinWidth.
    /// </summary>
    UnrestrictedMinWidth = LayoutMinWidth | 1 << 8,
    /// <summary>
    /// The container's min height is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutMinHeight.
    /// </summary>
    UnrestrictedMinHeight = LayoutMinHeight | 1 << 9,
    /// <summary>
    /// The container's max width is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutMaxWidth.
    /// </summary>
    UnrestrictedMaxWidth = LayoutMaxWidth | 1 << 10,
    /// <summary>
    /// The container's max height is that of its layout if the layout provides it, unless the container
    /// is in a parent layout that doesn't set its child controls' size. Takes precedence over LayoutMaxHeight.
    /// </summary>
    UnrestrictedMaxHeight = LayoutMaxHeight | 1 << 11,

    /// <summary>
    /// The container's hint sizes are calculated by the layout if the layout provides it
    /// </summary>
    LayoutHint = LayoutHintWidth | LayoutHintHeight,
    /// <summary>
    /// The container's min sizes are calculated by the layout if the layout provides it
    /// </summary>
    LayoutMin = LayoutMinWidth | LayoutMinHeight,
    /// <summary>
    /// The container's max sizes are calculated by the layout if the layout provides it
    /// </summary>
    LayoutMax = LayoutMaxWidth | LayoutMaxHeight,
    /// <summary>
    /// The container's widths are calculated by the layout if the layout provides it
    /// </summary>
    LayoutWidth = LayoutHintWidth | LayoutMinWidth | LayoutMaxWidth,
    /// <summary>
    /// The container's heights are calculated by the layout if the layout provides it
    /// </summary>
    LayoutHeight = LayoutHintHeight | LayoutMinHeight | LayoutMaxHeight,

    /// <summary>
    /// The container's hint sizes are calculated by the layout if the layout provides it, apart from
    /// sizes that a parent container's layout doesn't set for its child controls
    /// </summary>
    UnrestrictedHint = UnrestrictedHintWidth | UnrestrictedHintHeight,
    /// <summary>
    /// The container's min sizes are calculated by the layout if the layout provides it, apart from
    /// sizes that a parent container's layout doesn't set for its child controls
    /// </summary>
    UnrestrictedMin = UnrestrictedMinWidth | UnrestrictedMinHeight,
    /// <summary>
    /// The container's max sizes are calculated by the layout if the layout provides it, apart from
    /// sizes that a parent container's layout doesn't set for its child controls
    /// </summary>
    UnrestrictedMax = UnrestrictedMaxWidth | UnrestrictedMaxHeight,
    /// <summary>
    /// The container's widths are calculated by the layout if the layout provides it, apart from
    /// sizes that a parent container's layout doesn't set for its child controls
    /// </summary>
    UnrestrictedWidth = UnrestrictedHintWidth | UnrestrictedMinWidth | UnrestrictedMaxWidth,
    /// <summary>
    /// The container's heights are calculated by the layout if the layout provides it, apart from
    /// sizes that a parent container's layout doesn't set for its child controls
    /// </summary>
    UnrestrictedHeight = UnrestrictedHintHeight | UnrestrictedMinHeight | UnrestrictedMaxHeight,

    /// <summary>
    /// Every size that the container's layout provides is calculated by the layout
    /// </summary>
    AllLayout = LayoutHint | LayoutMin | LayoutMax,

    /// <summary>
    /// Every size that the container's layout provides is calculated by the layout, apart from sizes
    /// that a parent container's layout doesn't set for its child controls. This is the default
    /// </summary>
    AllUnrestricted = UnrestrictedHint | UnrestrictedMin | UnrestrictedMax,
};
DECLARE_ENUM_OPERATORS(FudgetSizeOverride);

/// <summary>
/// Container class that can have child controls and a layout to position the controls
/// </summary>
API_CLASS()
class FUDGETS_API FudgetContainer : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetContainer);
public:
    ~FudgetContainer();

    template<typename T>
    FORCE_INLINE T* CreateChild()
    {
        T* child = New<T>(SpawnParams(Guid::New(), T::TypeInitializer));
        AddChild(child);
        return child;
    }

    template<typename T>
    FORCE_INLINE T* CreateLayout()
    {
        T* layout = New<T>(SpawnParams(Guid::New(), T::TypeInitializer));
        FudgetLayout *old_layout = SetLayout(layout);
        if (old_layout != nullptr)
            Delete(old_layout);
        return layout;
    }

    /// <summary>
    /// The color to use to fill the background of this container if fill_background is true
    /// </summary>
    API_FIELD() Color FillColor;

    /// <summary>
    /// Whether the fill_color is used to fill the background of this container. Mainly for testing
    /// </summary>
    API_FIELD() bool DrawFilledBackground;

    /// <summary>
    /// Inserts a control into the layout of this container. If an index is provided, the controls with the same
    /// or higher index are moved one index higher. The control with the highest index is drawn last.
    /// A negative value means the top above other controls.
    /// The control's drawing and events will be managed by the container. The control's lifetime is also controlled
    /// by the container, destroying the control when the container is destroyed.
    /// </summary>
    /// <param name="control">The control to insert</param>
    /// <param name="index">The requested index of the control</param>
    /// <returns>The new index position of the inserted control or -1 on failure.</returns>
    API_FUNCTION() virtual int AddChild(FudgetControl *control, int index = -1);

    /// <summary>
    /// Removes a child control from this container's layout, and changes the index of controls with higher indexes.
    /// </summary>
    /// <param name="control">The control to remove</param>
    /// <returns>The old index of the control</returns>
    API_FUNCTION() virtual int RemoveChild(FudgetControl *control);

    /// <summary>
    /// Removes a child control from this container's layout, and changes the index of controls with higher indexes.
    /// </summary>
    /// <param name="index">The index of the control to remove</param>
    /// <returns>The control removed from the layout</returns>
    API_FUNCTION() FudgetControl* RemoveChild(int index);

    /// <summary>
    /// Changes the index of a control at one index position to the other, moving all controls between to a new index.
    /// </summary>
    /// <param name="from">The starting index of the control</param>
    /// <param name="to">The index to move the control to</param>
    /// <returns>Returns whether the control's index was set to the target value</returns>
    API_FUNCTION() virtual bool MoveChildToIndex(int from, int to);

    /// <summary>
    /// Returns the control in the layout with the given index, if the index is valid.
    /// </summary>
    /// <param name="index">The index of the control</param>
    /// <returns>The control at index position or null on error</returns>
    API_FUNCTION() FudgetControl* ChildAt(int index) const;

    /// <summary>
    /// Gets the number of child controls directly inside the layout of this control
    /// </summary>
    /// <returns>The number of child controls</returns>
    API_PROPERTY() int GetChildCount() const;

    /// <summary>
    /// Gets the index of the control in the container's layout.
    /// </summary>
    /// <param name="control">The control to look for</param>
    /// <returns>The index if found, -1 if not</returns>
    API_FUNCTION() int ChildIndex(FudgetControl *control) const;

    /// <summary>
    /// Deletes all the controls added to the layout. Any reference to the controls becomes invalid.
    /// </summary>
    API_FUNCTION() void DeleteAll();

    /// <inheritdoc />
    void SetEnabled(bool value) override;

    /// <summary>
    /// Gets the preferred size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred size, this is the preferred size of the container itself.
    /// </summary>
    /// <returns>The container's preferred size with the current layout</returns>
    Float2 GetLayoutHintSize() const override;

    /// <summary>
    /// Gets the minimum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred minimum size, this is the minimum size set to the container itself.
    /// </summary>
    /// <returns>The container's minimum size with the current layout</returns>
    Float2 GetLayoutMinSize() const override;

    /// <summary>
    /// Gets the maximum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred maximum size, this is the maximum size set to the container itself.
    /// </summary>
    /// <returns>The container's maximum size with the current layout</returns>
    Float2 GetLayoutMaxSize() const override;

    /// <summary>
    /// The available space in the container for child controls, which excludes internal padding. Only valid if
    /// the size of the container is valid. 
    /// TODO: implement internal padding
    /// </summary>
    /// <returns></returns>
    API_PROPERTY() Float2 LayoutSpace() const;

    /// <summary>
    /// Notifies this container about a change that might require layout calculations. If a control initiated the
    /// change, it should be passed to the function, otherwise its sizes might not be measured. The control must
    /// be null for changes in self.
    /// </summary>
    /// <param name="dirt_flags">Cause of the layout change</param>
    /// <param name="control">Child control that initiated the change or null</param>
    API_FUNCTION() void MarkLayoutDirty(FudgetLayoutDirtyReason dirt_flags, FudgetControl *control = nullptr);

    /// <summary>
    /// Makes sure that this container and the child controls in this container have a valid size and position. This
    /// can cause parent layout calculations if needed.
    /// </summary>
    API_FUNCTION() void EnsureLayout();

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    bool OnMeasure(Float2 available, API_PARAM(Out) Float2 &wanted, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size) override;

    /// <inheritdoc />
    void DoFocusChanged(bool focused, FudgetControl *other) override;

    /// <inheritdoc />
    void DoShow() override;

    /// <inheritdoc />
    void DoHide() override;


        /// <inheritdoc />
    void ClearStyleCache(bool inherited = true) override;

    /// <summary>
    /// The current layout set for the container, or null if no layout was set.
    /// </summary>
    API_PROPERTY() FudgetLayout* GetLayout() const { return _dummy_layout ? nullptr : _layout; }

    /// <summary>
    /// Sets the given layout for the container. It's the responsibility of the user to delete the old layout returned
    /// from this function if it's not null. Trying to set the layout already in this container is an error and will
    /// always return null.
    /// </summary>
    /// <param name="value">The new layout for the container</param>
    /// <returns>Layout previously set for the container or null on error.</returns>
    API_FUNCTION() FudgetLayout* SetLayout(FudgetLayout *value);

    /// <summary>
    /// In the base FudgeControl, this would notify parent containers of changes, but changes are
    /// handled differently in this class, and this function does nothing.
    /// </summary>
    /// <param name="dirt_flags">Flags of what changed</param>
    void SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags) override final;

    /// <inheritdoc />
    void SetControlFlags(FudgetControlFlag flags) override;

    /// <summary>
    /// Determines if the control is allowed to update its position directly, instead of being positioned
    /// by this container or its layout. The call is invalid if the control doesn't belong to this container.
    /// </summary>
    /// <param name="control">The control requesting position change</param>
    /// <returns>Whether the control can change its position or not</returns>
    API_FUNCTION() bool IsControlPositionChangePermitted(const FudgetControl *control) const;

    /// <summary>
    /// Determines if setting the size hint of a control in this container should result in changing the control's size
    /// as well. This is usually false in a container with a layout that is responsible for the control dimensions.
    /// </summary>
    /// <param name="control">The control requesting size change</param>
    /// <returns>Whether the control can update its size or not</returns>
    API_FUNCTION() bool IsControlDirectSizeChangePermitted(const FudgetControl *control) const;

    /// <summary>
    /// Fills an array with all child controls belonging to the container.
    /// </summary>
    /// <param name="container">The parent container holding the controls to list</param>
    /// <param name="result">A list of all child controls in the order they are on their parents</param>
    API_FUNCTION() static void GetAllControls(const FudgetContainer *container, API_PARAM(Ref) Array<FudgetControl*> &result);

    /// <summary>
    /// Fills an array with all child controls belonging to this container.
    /// </summary>
    /// <param name="result">A list of all child controls in the order they are on their parents</param>
    API_FUNCTION() void GetAllControls(API_PARAM(Ref) Array<FudgetControl*> &result) const;

    // Input:

    /// <summary>
    /// Lists every control under the given position from bottom to top that has the matching flags.
    /// Containers call this function recursively to find all child controls that match as well.
    /// </summary>
    /// <param name="pos">Position relative to the container's top left corner</param>
    /// <param name="request_flags">Flags that controls must match with HasAnyFlag</param>
    /// <param name="reject_flags">Flags that shouldn't be found on the controls. This takes priority over requested flags.</param>
    /// <param name="block_flags">Stops checking child controls in the container which has one of these flags.</param>
    /// <param name="request_state">State flags of controls that must match with HasAnyFlag</param>
    /// <param name="reject_state">State flags that shouldn't be found on the controls. This takes priority over requested states.</param>
    /// <param name="block_state">Stops checking child controls in the container which has one of these states.</param>
    /// <param name="result">Receives the controls that match the flags</param>
    /// <returns>List of controls matching at least one request flag and none of the reject flags</returns>
    API_FUNCTION() virtual void ControlsAtPosition(Float2 pos, FudgetControlFlag request_flags, FudgetControlFlag reject_flags, FudgetControlFlag block_flags,
        FudgetControlState request_state, FudgetControlState reject_state, FudgetControlState block_state, API_PARAM(Ref) Array<FudgetControl*> &result);

    /// <summary>
    /// Determines which sizes are calculated by the layout and which ones are calculated by the container.
    /// </summary>
    API_PROPERTY() FudgetSizeOverride GetSizeOverrides() const { return _size_overrides; }

    /// <summary>
    /// Whether the container is providing its own hint size, not using the layout to calculate it.
    /// </summary>
    API_PROPERTY() bool IgnoresLayoutHintSize() const;

    /// <summary>
    /// Whether the container is providing its own min size, not using the layout to calculate it.
    /// </summary>
    API_PROPERTY() bool IgnoresLayoutMinSize() const;

    /// <summary>
    /// Whether the container is providing its own max size, not using the layout to calculate it.
    /// </summary>
    API_PROPERTY() bool IgnoresLayoutMaxSize() const;

    /// <summary>
    /// Whether the container is providing its own sizes, not using the layout to calculate them.
    /// </summary>
    API_PROPERTY() bool IgnoresLayoutSizes() const;

    /// <summary>
    /// Determines which sizes are calculated by the layout and which ones are calculated by the container.
    /// </summary>
    API_PROPERTY() void SetSizeOverrides(FudgetSizeOverride value);

    /// <summary>
    /// Checks if a size override is set in the container. If the value is a combination of flags, all need to be set.
    /// </summary>
    /// <param name="value">Size override flag or flags to check</param>
    /// <returns>Whether the flag is set for the container</returns>
    API_FUNCTION() bool HasSizeOverride(FudgetSizeOverride value) const;

    // Serialization

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

    /// <summary>
    /// Draws itself and then asks the child controls to draw themselves. Derived containers should instead override
    /// OnDraw for drawing themselves.
    /// </summary>
    void DoDraw() override;
protected:
    /// <inheritdoc />
    void DoInitialize() override;

    /// <inheritdoc />
    void DoStyleInitialize() override;

    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;

    /// <inheritdoc />
    void RequestLayout() override;

    /// <inheritdoc />
    void SetParentDisabled(bool value) override;

    /// <inheritdoc />
    void SetParentVisibility(bool value) override;

    /// <inheritdoc />
    void DoRootChanging(FudgetGUIRoot *new_root)  override;

    /// <inheritdoc />
    void DoRootChanged(FudgetGUIRoot *old_root) override;

    /// <inheritdoc />
    void DoParentStateChanged() override;

private:
    void SetParentDisabledRecursive();

    void SetParentVisibilityRecursive(bool visible);

    // Called from EnsureLayout into parent containers to find one that needs layouting. The passed value is the last
    // container in the chain that has a dirty layout. If the root is reached, the last_dirty container's
    // RequestLayout is called, unless last_dirty is null.
    void EnsureLayoutParent(FudgetContainer *last_dirty);

    /// <summary>
    /// Directly changes the position and size of the control. Only to be called by FudgetLayout. This derived
    /// version makes sure that the control's size change will cause the contents to recalculate when necessary.
    /// </summary>
    /// <param name="pos">The new position</param>
    /// <param name="size">The new size</param>
    void LayoutUpdate(Float2 pos, Float2 size) override;

    /// <summary>
    /// Creates the FudgetContainerLayout as the default layout in the container, when no other layout is set.
    /// This layout is never returned with GetLayout and should be transparent to the outside.
    /// </summary>
    void CreateDummyLayout();

    Array<FudgetControl*> _children;
    FudgetLayout *_layout;
    // Using a FudgetContainerLayout that lets its child controls determine their own position and size
    bool _dummy_layout;
    // Setting that determines which sizes are calculated by the layout and which ones are calculated by the container.
    FudgetSizeOverride _size_overrides;

    // Used locally to avoid double calling functions from child controls.
    bool _changing;
};
