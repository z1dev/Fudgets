#pragma once

#include "Layout.h"
#include "../MarginStructs.h"


/// <summary>
/// Rule applied to slots how to distribute the available space in the layout
/// </summary>
API_ENUM()
enum class FudgetDistributedSizingRule
{
    /// <summary>
    /// Slot wants to take up as much space as it can, even if it means Shrink slots will be forced to be
    /// their minimal size. If multiple slots have Expanding specfied, the remaining space will be
    /// distributed based on their weight.
    /// </summary>
    Expanding,
    /// <summary>
    /// Slot wants to take up as much space as it can, but only if there is space left. If there are
    /// Expanding slots, the slot will behave like an Expanding slot and the remaining space will be
    /// distributed among them based on their weight. With no Expanding slots, this rule is equivalent
    /// to GrowExact.
    /// </summary>
    GrowExpanding,
    /// <summary>
    /// Slot wants to take up as much space as it can, but only if there is space left. If there are
    /// Expanding slots, the slot will behave like an Exact slot and will want to keep its contents'
    /// measured size. If there are multiple Grow*** slots and there is no Expanding slot, the remaining
    /// space will be distributed based on their weight. With no Expanding slots, this rule is equivalent
    /// to GrowExpanding.
    /// </summary>
    GrowExact,
    /// <summary>
    /// Slot wants to be its measured size if there is enough space. Grow or Expanding slots don't influence
    /// its size. When there are no Grow or Expanding slots, the remaining space is distributed among Exact
    /// slots based on their weight.
    /// </summary>
    Exact,
    /// <summary>
    /// Slot wants to be the size it measured for itself, but if there are Expanding slots, it will be the
    /// smallest size it needs. When there are only Shrink slots, the remaining space is distributed among
    /// those based on their weight.
    /// </summary>
    Shrink,
    /// <summary>
    /// Slot wants to be as small as its minimum size if there are non-Minimal slots too. When there are only
    /// Minimal slots, the remaining space is distributed among those based on their weight.
    /// </summary>
    Minimal,
};

/// <summary>
/// Rule applied to slots to decide how they shrink when there is not enough space in the layout
/// </summary>
API_ENUM()
enum class FudgetDistributedShrinkingRule
{
    /// <summary>
    /// Slot can shrink below its wanted size until it reaches its minimum size. Slots with this rule will
    /// shrink to their minimum size before slots with the LateShrink shrinking rule start changing their size.
    /// Has no effect when the sizing rule of a slot forces it to be its minimum size.
    /// </summary>
    CanShrink,
    /// <summary>
    /// Slot keeps its wanted size while there are other slots that can shrink. If every shrinking slot
    /// reached its minimum size, this slot will shrink together with other LateShrink slots proportionally.
    /// Has no effect when the sizing rule of a slot forces it to be its minimum size.
    /// </summary>
    LateShrink,
    /// <summary>
    /// Slot behaves like a CanShrink slot while having a minimum size of zero independent of its contents.
    /// </summary>
    IgnoreMinimum,
    /// <summary>
    /// Slots of this rule can't shrink below their wanted size.
    /// Has no effect when the sizing rule of a slot forces it to be its minimum size.
    /// </summary>
    KeepSize,
};

/// <summary>
/// Derived slot holding attributes necessary for layouting for the list layout
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListLayoutSlot : public FudgetLayoutSlot
{
    using Base = FudgetLayoutSlot;
    DECLARE_SCRIPTING_TYPE(FudgetListLayoutSlot);
    API_AUTO_SERIALIZATION();

public:
    /// <summary>
    /// Horizontal alignment of control in its column or row
    /// </summary>
    API_FIELD() FudgetLayoutHorzAlign _horz_align;

    /// <summary>
    /// Vertical alignment of control in its column or row
    /// </summary>
    API_FIELD() FudgetLayoutVertAlign _vert_align;

    /// <summary>
    /// Padding around inside the slot around the control
    /// </summary>
    API_FIELD() FudgetPadding _padding;

    /// <summary>
    /// Rule determining how the horizontal or vertical size of the slot is calculated.
    /// </summary>
    API_FIELD() FudgetDistributedSizingRule _sizing_rule;

    /// <summary>
    /// Rule determining how the slot might shrink below its wanted size if there is not enough space.
    /// </summary>
    API_FIELD() FudgetDistributedShrinkingRule _shrinking_rule;

    /// <summary>
    /// Weight of the slot, determining how much free space it uses relative to other slots if there is space
    /// to distribute or excess to remove. The weight must be positive.
    /// </summary>
    API_FIELD() Float2 _weight;

};

/// <summary>
/// A layout that aligns the owner container's children horizontally or vertically like a list of items
/// </summary>
API_CLASS()
class FUDGETS_API FudgetListLayout : public FudgetLayout
{
    using Base = FudgetLayout;
    DECLARE_SCRIPTING_TYPE(FudgetListLayout);
public:

    /// <summary>
    /// Gets the layout orientation of controls on the owner. Depending on the value, they are either
    /// placed next to each other from left to right or from top to bottom
    /// </summary>
    /// <returns>The layout direction for positioning child controls</returns>
    API_PROPERTY() FudgetOrientation GetOrientation() { return _ori; }
    /// <summary>
    /// Sets the layout orientation of controls on the owner. Depending on the value, they are either
    /// placed next to each other from left to right or from top to bottom
    /// </summary>
    /// <param name="value">The layout direction for positioning child controls</param>
    API_PROPERTY() void SetOrientation(FudgetOrientation value);

    /// <summary>
    /// Gets a control's horizontal alignment given its index in the owner container. The horizontal alignment
    /// determines how the control is placed in its designated slot.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The control's horizontal alignment</returns>
    API_FUNCTION() FudgetLayoutHorzAlign GetSlotHorizontalAlignment(int index) const;

    /// <summary>
    /// Sets a control's horizontal alignment given its index in the owner container. The horizontal alignment
    /// determines how the control is placed in its designated slot.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The control's new horizontal alignment</param>
    API_FUNCTION() void SetSlotHorizontalAlignment(int index, FudgetLayoutHorzAlign value);

    /// <summary>
    /// Gets a control's vertical alignment given its index in the owner container. The vertical alignment
    /// determines how the control is placed in its designated slot.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The control's vertical alignment</returns>
    API_FUNCTION() FudgetLayoutVertAlign GetSlotVerticalAlignment(int index) const;

    /// <summary>
    /// Sets a control's vertical alignment given its index in the owner container. The vertical alignment
    /// determines how the control is placed in its designated slot.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The control's new vertical alignment</param>
    API_FUNCTION() void SetSlotVerticalAlignment(int index, FudgetLayoutVertAlign value);

    /// <summary>
    /// Gets the padding of a control in its slot. The padding with the control together counts as the
    /// content of a slot, and it is calculated as an inner border around the control
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The padding values for the sides</returns>
    API_FUNCTION() FudgetPadding& GetSlotPadding(int index) const;

    /// <summary>
    /// Sets the padding of a control in its slot. The padding with the control together counts as the
    /// content of a slot, and it is calculated as an inner border around the control
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The padding values for the sides</param>
    API_FUNCTION() void SetSlotPadding(int index, FudgetPadding value);

    /// <summary>
    /// Gets the sizing rule of a control slot. The sizing rule determines how a slot expands.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The sizing rule</returns>
    API_FUNCTION() FudgetDistributedSizingRule GetSlotSizingRule(int index) const;

    /// <summary>
    /// Sets the sizing rule of a control slot. The sizing rule determines how a slot expands.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The sizing rule</param>
    API_FUNCTION() void SetSlotSizingRule(int index, FudgetDistributedSizingRule value);
    /// <summary>
    /// Gets the shrinking rule of a control slot. The shrinking rule determines whether a slot shrinks
    /// before other slots reach their minimum size.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The shrinking rule</returns>
    API_FUNCTION() FudgetDistributedShrinkingRule GetSlotShrinkingRule(int index) const;
    /// <summary>
    /// Sets the shrinking rule of a control slot. The shrinking rule determines whether a slot shrinks
    /// before other slots reach their minimum size.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The shrinking rule</param>
    API_FUNCTION() void SetSlotShrinkingRule(int index, FudgetDistributedShrinkingRule value);
    /// <summary>
    /// Gets the weight a control slot. The slot weight determines the relative size of a slot when
    /// multiple slots can expand or shrink. Its effect depends on the sizing and shrinking rules.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <returns>The slot weight</returns>
    API_FUNCTION() Float2 GetSlotWeight(int index) const;
    /// <summary>
    /// Sets the weight a control slot. The slot weight determines the relative size of a slot when
    /// multiple slots can expand or shrink. Its effect depends on the sizing and shrinking rules.
    /// </summary>
    /// <param name="index">The control's index in its container</param>
    /// <param name="value">The slot weight</param>
    API_FUNCTION() void SetSlotWeight(int index, Float2 value);

protected:
    /// <inheritdoc />
    void PreLayoutChildren(Int2 space, FudgetContainer *owner, int count) override;
    /// <inheritdoc />
    void LayoutChildren(Int2 space, FudgetContainer *owner, int count) override;

    /// <inheritdoc />
    FudgetLayoutSlot* CreateSlot(FudgetControl *control) override;

    /// <summary>
    /// Replaces the GetSlot function of Layout to return the derived slot type
    /// </summary>
    /// <param name="index">The index of the control to make the slot for</param>
    /// <returns>The slot for layouting attributes or null if the index is invalid</returns>
    API_FUNCTION(new) FudgetListLayoutSlot* GetSlot(int index) const;

    /// <inheritdoc />
    FudgetLayoutFlag GetInitFlags() const override;

    /// <inheritdoc />
    void PlaceControlInSlotRectangle(int index) override;

private:
    // Used during layouting to check if the sizing rule allows its slot to expand. It uses data provided from PreLayoutChildren
    bool IsExpandingRule(FudgetDistributedSizingRule rule) const;

    template<typename R, typename A>
    FORCE_INLINE R Relevant(A value) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return value.X;
        return value.Y;
    }

    FORCE_INLINE int Relevant(Int2 value) const
    {
        return Relevant<int>(value);
    }

    FORCE_INLINE float Relevant(Float2 value) const
    {
        return Relevant<float>(value);
    }

    template<typename R, typename A>
    FORCE_INLINE R& RelevantRef(A &value) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return value.X;
        return value.Y;
    }

    FORCE_INLINE int& RelevantRef(Int2 value) const
    {
        return RelevantRef<int>(value);
    }

    FORCE_INLINE float& RelevantRef(Float2 value) const
    {
        return RelevantRef<float>(value);
    }

    template<typename R, typename A>
    FORCE_INLINE R Opposite(A value) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return value.Y;
        return value.X;
    }

    FORCE_INLINE int Opposite(Int2 value) const
    {
        return Opposite<int>(value);
    }

    FORCE_INLINE float Opposite(Float2 value) const
    {
        return Opposite<float>(value);
    }

    template<typename R, typename A>
    FORCE_INLINE R& OppositeRef(A &value) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return value.Y;
        return value.X;
    }

    FORCE_INLINE int& OppositeRef(Int2 value) const
    {
        return OppositeRef<int>(value);
    }

    FORCE_INLINE float& OppositeRef(Float2 value) const
    {
        return OppositeRef<float>(value);
    }

    FORCE_INLINE int RelevantPad(const FudgetPadding &padding) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return padding.Left + padding.Right;
        return padding.Top + padding.Bottom;
    }
    FORCE_INLINE int OppositePad(const FudgetPadding &padding) const
    {
        if (_ori == FudgetOrientation::Horizontal)
            return padding.Top + padding.Bottom;
        return padding.Left + padding.Right;
    }

    FudgetOrientation _ori;
    //bool _stretched;

    // Temporary values used during layout frame:

    // Available space with all slot paddings subtracted 
    int _no_padding_space;

    bool _has_expanding;
    bool _has_grow_expanding;
    bool _has_grow_exact;
    bool _has_exact;
    bool _has_shrink;
    bool _has_minimal;

    bool _space_dependent;
};
