#pragma once

#include "Control.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Input/Enums.h"
#include "Engine/Core/Log.h"

class FudgetLayout;
class Fudget;


enum class FudgetDirtType : uint8;


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
        T* child = New<T>(SpawnParams(Guid::New(), TypeInitializer));
        AddChild(child);
        return child;
    }

    template<typename T>
    FORCE_INLINE T* CreateLayout()
    {
        if (GetGUIRoot() == this)
        {
            LOG(Error, "Adding layout directly to the root is not supported. Add a child container or FudgetAssetRoot and set its hint size to the size of this root.");
            return nullptr;
        }

        T* layout = New<T>(SpawnParams(Guid::New(), TypeInitializer));
        SetLayout(layout);
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
    Float2 GetHintSize() const override;

    /// <inheritdoc />
    void SetHintSize(Float2 value) override;

    /// <summary>
    /// Gets the minimum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred minimum size, this is the minimum size set to the container itself.
    /// </summary>
    /// <returns>The container's minimum size with the current layout</returns>
    Float2 GetMinSize() const override;

    /// <inheritdoc />
    void SetMinSize(Float2 value) override;

    /// <summary>
    /// Gets the maximum size of this container with its contents depending on the layout. If the layout
    /// doesn't have a preferred maximum size, this is the maximum size set to the container itself.
    /// </summary>
    /// <returns>The container's maximum size with the current layout</returns>
    Float2 GetMaxSize() const override;

    /// <inheritdoc />
    void SetMaxSize(Float2 value) override;

    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or the preferred size is
    /// based on the container's hint size. If the container has a layout that can't provide its own hint size
    /// width, this setting will be ignored, and the container will have to rely on its own hint size.
    /// </summary>
    /// <returns>Using the layout's width or not</returns>
    API_PROPERTY() bool GetUsingLayoutHintWidth() const { return _hint_width_from_layout; }

    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or the preferred size is
    /// based on the container's hint size. If the container has a layout that can't provide its own hint size
    /// width, this setting will be ignored, and the container will have to rely on its own hint size.
    /// </summary>
    /// <param name="value">Use the layout's width or not</param>
    API_PROPERTY() void SetUsingLayoutHintWidth(bool value);

    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or the preferred size is
    /// based on the container's hint size. If the container has a layout that can't provide its own hint size
    /// height, this setting will be ignored, and the container will have to rely on its own hint size.
    /// </summary>
    /// <returns>Using the layout's height or not</returns>
    API_PROPERTY() bool GetUsingLayoutHintHeight() const { return _hint_height_from_layout; }

    /// <summary>
    /// Determines if the height of the container is calculated based on the layout or the preferred size is
    /// based on the container's hint size. If the container has a layout that can't provide its own hint size
    /// height, this setting will be ignored, and the container will have to rely on its own hint size.
    /// </summary>
    /// <param name="value">Use the layout's height or not</param>
    API_PROPERTY() void SetUsingLayoutHintHeight(bool value);

    /// <summary>
    /// Determines if the minimum width of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min width or not</returns>
    API_PROPERTY() bool GetUsingLayoutMinWidth() const { return _min_width_from_layout; }

    /// <summary>
    /// Determines if the minimum width of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min width or not</returns>
    API_PROPERTY() void SetUsingLayoutMinWidth(bool value);

    /// <summary>
    /// Determines if the maximum width of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min width or not</returns>
    API_PROPERTY() bool GetUsingLayoutMaxWidth() const { return _max_width_from_layout; }

    /// <summary>
    /// Determines if the maximum width of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min width or not</returns>
    API_PROPERTY() void SetUsingLayoutMaxWidth(bool value);

    /// <summary>
    /// Determines if the minimum height of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min height or not</returns>
    API_PROPERTY() bool GetUsingLayoutMinHeight() const { return _min_height_from_layout; }

    /// <summary>
    /// Determines if the minimum height of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min height or not</returns>
    API_PROPERTY() void SetUsingLayoutMinHeight(bool value);

    /// <summary>
    /// Determines if the maximum height of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min height or not</returns>
    API_PROPERTY() bool GetUsingLayoutMaxHeight() const { return _max_height_from_layout; }

    /// <summary>
    /// Determines if the maximum height of the container is calculated by the layout or it is set directly on
    /// the container
    /// </summary>
    /// <returns>Using the layout's min height or not</returns>
    API_PROPERTY() void SetUsingLayoutMaxHeight(bool value);

    /// <summary>
    /// Notifies the layout of this container that one or more of the stored sizes and control positions might
    /// need to be recalculated.
    /// </summary>
    /// <param name="dirt_flags">The size that needs recalculation</param>
    /// <param name="content_changed">True if the changed was caused by a child control or layout</param>
    /// <returns></returns>
    API_FUNCTION() void MarkLayoutDirty(FudgetDirtType dirt_flags, bool content_changed = false);

    /// <summary>
    /// Requests the immediate recalculation of the controls and their sizes inside the container's layout. Only those sizes are
    /// recalculated that are made dirty by calling MarkLayoutDirty directly or that have changed.
    /// In the container's implementation, if no layout is present, the controls are moved to their requested size and position.
    /// Compound controls can use this function to layout their children.
    /// </summary>
    API_FUNCTION() virtual void RequestLayout();

    /// <inheritdoc />
    void OnDraw() override;

    /// <inheritdoc />
    void OnFocusChanged(bool focused, FudgetControl *other) override;

        /// <inheritdoc />
    void ClearStyleCache(bool inherited = true) override;

    /// <summary>
    /// Returns the current layout set for the container
    /// </summary>
    API_PROPERTY() FudgetLayout* GetLayout() const { return _layout; }

    /// <summary>
    /// Sets the given layout for the container. It's the responsibility of the user to get the current layout beforehand and
    /// delete it when this function is done.
    /// </summary>
    /// <param name="value">The new layout for the container</param>
    API_PROPERTY() void SetLayout(FudgetLayout *value);

    /// <summary>
    /// In the base FudgeControl, this would notify parent containers of changes, but changes are
    /// handled differently in this class, and this function does nothing.
    /// </summary>
    /// <param name="dirt_flags">Flags of what changed</param>
    void SizeOrPosModified(FudgetDirtType dirt_flags) override final;

    /// <inheritdoc />
    void SetControlFlags(FudgetControlFlags flags) override;

    /// <summary>
    /// Determines if the control is allowed to update its position directly, instead of being positioned
    /// by this container or its layout. The call is invalid if the control doesn't belong to this container.
    /// </summary>
    /// <param name="control">The control requesting position change</param>
    /// <returns>Whether the control can change its position or not</returns>
    API_FUNCTION() bool IsControlPositioningPermitted(const FudgetControl *control) const;

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
    /// <param name="request">Flags that the control must match with HasAnyFlag</param>
    /// <param name="reject">Flags that shouldn't be found on the controls. This takes priority over requested flags.</param>
    /// <param name="block">Stops checking child controls in the container which has one of these flags.</param>
    /// <param name="result">Receives the controls that match the flags</param>
    /// <returns>List of controls matching at least one request flag and none of the reject flags</returns>
    API_FUNCTION() virtual void ControlsAtPosition(Float2 pos, FudgetControlFlags request, FudgetControlFlags reject, FudgetControlFlags block, API_PARAM(Ref) Array<FudgetControl*> &result);

    // Serialization

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;

    /// <summary>
    /// Requests layouting, draws itself and then asks the child controls to draw themselves. Derived containers
    /// should instead override OnDraw for drawing themselves.
    /// </summary>
    void Draw() override;

protected:
    /// <inheritdoc />
    void Initialize() override;

    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

    /// <inheritdoc />
    void SetParentDisabled(bool value) override;

private:
    void SetParentDisabledRecursive();

    /// <summary>
    /// Directly changes the position and size of the control. Only to be called by FudgetLayout. This derived
    /// version makes sure that the control's size change will cause the contents to recalculate when necessary.
    /// </summary>
    /// <param name="pos">The new position</param>
    /// <param name="size">The new size</param>
    void LayoutUpdate(Float2 pos, Float2 size) override;

    Array<FudgetControl*> _children;
    FudgetLayout *_layout;

    bool _hint_width_from_layout;
    bool _hint_height_from_layout;

    bool _min_width_from_layout;
    bool _max_width_from_layout;
    bool _min_height_from_layout;
    bool _max_height_from_layout;

    // Used locally to avoid double calling functions from child controls.
    bool _changing;
};
