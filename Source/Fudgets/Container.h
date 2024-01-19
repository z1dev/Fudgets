#pragma once

#include "Control.h"
#include "Engine/Core/Collections/Array.h"

class FudgetLayout;
class Fudget;


enum class FudgetDirtType : uint8;

// TODO: use these

/// <summary>
/// Vertical alignment of a layout inside a container if the container's size is independent of the layout
/// </summary>
API_ENUM()
enum class FudgetLayoutVertAlign
{
    /// <summary>
    /// The layout is attached to the top of the container.
    /// </summary>
    Top,
    /// <summary>
    /// The layout is attached to the bottom of the container.
    /// </summary>
    Bottom,
};

/// <summary>
/// Horizontal alignment of a layout inside a container if the container's size is independent of the layout
/// </summary>
API_ENUM()
enum class FudgetLayoutHorzAlign
{
    /// <summary>
    /// The layout is attached to the left of the container.
    /// </summary>
    Left,
    /// <summary>
    /// The layout is attached to the right of the container.
    /// </summary>
    Right,
};


/// <summary>
/// Container class that can have child controls and a layout to position the controls
/// </summary>
API_CLASS()
class FUDGETS_API FudgetContainer : public FudgetControl
{
    using Base = FudgetControl;
	DECLARE_SCRIPTING_TYPE(FudgetContainer);
public:
    FudgetContainer(Fudget *_ui_owner);

    ~FudgetContainer();

    template<typename T>
    FORCE_INLINE T* CreateChild()
    {
        T* child = New<T>(SpawnParams(Guid::New(), TypeInitializer));
        AddChild(child);
        return child;
    }

    template<typename T>
    FORCE_INLINE T* CreateLayout()
    {
        T* layout = New<T>(SpawnParams(Guid::New(), TypeInitializer));
        AddLayoutInternal(layout);
        return layout;
    }

    /// <summary>
    /// Inserts a control into the layout of this container. If an index is provided, the controls with same or higher
    /// index are moved into a higher index in the hierarchy. The control with the highest index is drawn last.
    /// Control's drawing and events will be managed by the container. The control's lifetime is also controlled
    /// by the container, destroying the control when the container is destroyed.
    /// </summary>
    /// <param name="control">The control to insert</param>
    /// <param name="index">The requested index of the control. -1 means add to end</param>
    /// <returns>The new index position of the inserted control or -1 on failure.</returns>
    API_FUNCTION() int AddChild(FudgetControl *control, int index = -1);

    /// <summary>
    /// Removes a child control from this container's layout, and changes the index of controls with higher indexes.
    /// </summary>
    /// <param name="control">The control to remove</param>
    /// <returns>The old index of the control</returns>
    API_FUNCTION() int RemoveChild(FudgetControl *control);

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
    API_FUNCTION() bool MoveChildToIndex(int from, int to);

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


    /// <summary>
    /// Gets the size that the container currently occupies in its parent's layout. This value can't be directly
    /// changed. Use SetHintSize to change it for normal containers. The root container inherits its size from
    /// the screen.
    /// </summary>
    /// <returns>The current size of the container.</returns>
    Float2 GetSize() const override;

    /// <summary>
    /// Gets the preferred size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred size, this is the preferred size of the container itself.
    /// </summary>
    /// <returns>The container's preferred size with the current layout</returns>
    Float2 GetHintSize() const override;
    /// <summary>
    /// Gets the minimum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred minimum size, this is the minimum size set to the container itself.
    /// </summary>
    /// <returns>The container's minimum size with the current layout</returns>
    Float2 GetMinSize() const override;
    /// <summary>
    /// Gets the maximum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred maximum size, this is the maximum size set to the container itself.
    /// </summary>
    /// <returns>The container's maximum size with the current layout</returns>
    Float2 GetMaxSize() const override;


    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or not.
    /// </summary>
    /// <returns>Whether the container has the height of the layout.</returns>
    API_PROPERTY() bool IsUsingLayoutHeight() const { return _height_from_layout; }

    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or not.
    /// </summary>
    /// <returns>Whether the container has the height of the layout.</returns>
    API_PROPERTY() void SetUsingLayoutHeight(bool value);

    /// <summary>
    /// Determines if the width of the container is calculated based on the layout or not.
    /// </summary>
    /// <returns>Whether the container has the width of the layout.</returns>
    API_PROPERTY() bool IsUsingLayoutWidth() const { return _width_from_layout; }

    /// <summary>
    /// Determines if the width of the container is calculated based on the layout or not.
    /// </summary>
    /// <returns>Whether the container has the width of the layout.</returns>
    API_PROPERTY() void SetUsingLayoutWidth(bool value);

    /// <summary>
    /// Notifies the layout that one or more of the stored sizes and control positions need to be recalculated. Hint, Min or Max
    /// sizes can be specified. Use All to make every size type dirty and None to only require control re-alignment.
    /// </summary>
    /// <param name="dirt_flags">The size that needs recalculation.</param>
    /// <returns></returns>
    API_PROPERTY() void MakeLayoutDirty(FudgetDirtType dirt_flags);
    /// <summary>
    /// Requests the immediate recalculation of the controls and their sizes inside the container's layout. Only those sizes are
    /// recalculated that are made dirty by calling MakeLayoutDirty directly or that have changed.
    /// </summary>
    API_FUNCTION() void RequestLayout();

    /// <summary>
    /// Called by the parent containers for all controls in the tree of controls to draw the control when rendering.
    /// </summary>
    /// <returns></returns>
    void Draw() override;

    API_FUNCTION(Internal) void AddLayoutInternal(FudgetLayout *layout);

    /// <summary>
    /// Notifies the parent to mark its layout as dirty, causing a calculation of child control sizes and
    /// positions. Depending on the size type, the local layout might need to recalculate as well. Any
    /// value other than Position will force a recalculation later.
    /// </summary>
    /// <param name="dirt_flags">Flags of what changed</param>
    void MakeParentLayoutDirty(FudgetDirtType dirt_flags) override;

private:
    Array<FudgetControl*> _children;
    FudgetLayout *_layout;

    bool _height_from_layout;
    bool _width_from_layout;

    // Used locally to avoid double calling functions from child controls.
    bool _changing;

    // True only for the container covering the screen as the root of the hierarchy
    Fudget *_root;
};