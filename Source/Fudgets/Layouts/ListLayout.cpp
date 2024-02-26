#include "ListLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"



FudgetListLayoutSlot::FudgetListLayoutSlot(const SpawnParams &params) : Base(SpawnParams(Guid::New(), TypeInitializer)),
    _horz_align(FudgetLayoutHorzAlign::Left), _vert_align(FudgetLayoutVertAlign::Top), _sizing_rule(FudgetDistributedSizingRule::Exact),
    _shrinking_rule(FudgetDistributedShrinkingRule::CanShrink), _weight(1.f)
{
}

FudgetListLayout::FudgetListLayout(const SpawnParams &params) : Base(params), _ori(FudgetOrientation::Horizontal), _no_padding_space(false),
    _has_expanding(false),  _has_grow_expanding(false), _has_grow_exact(false), _has_exact(false), _has_shrink(false), _has_minimal(false)
{

}

void FudgetListLayout::SetOrientation(FudgetOrientation value)
{
    if (_ori == value)
        return;

    _ori = value;
    MarkOwnerDirty();
}

FudgetLayoutHorzAlign FudgetListLayout::GetSlotHorizontalAlignment(int index) const
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return FudgetLayoutHorzAlign::Left;

    return slot->_horz_align;
}

void FudgetListLayout::SetSlotHorizontalAlignment(int index, FudgetLayoutHorzAlign value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr || slot->_horz_align == value)
        return;

    int slot_group = (slot->_horz_align == FudgetLayoutHorzAlign::Left || slot->_horz_align == FudgetLayoutHorzAlign::Right || slot->_horz_align == FudgetLayoutHorzAlign::Center) ? 0 :
        (slot->_horz_align == FudgetLayoutHorzAlign::LeftGrow || slot->_horz_align == FudgetLayoutHorzAlign::RightGrow || slot->_horz_align == FudgetLayoutHorzAlign::CenterGrow) ? 1 :
        (slot->_horz_align == FudgetLayoutHorzAlign::ClipLeft || slot->_horz_align == FudgetLayoutHorzAlign::ClipRight || slot->_horz_align == FudgetLayoutHorzAlign::ClipCenter) ? 2 : 3;
    slot->_horz_align = value;
    int change_group = (value == FudgetLayoutHorzAlign::Left || value == FudgetLayoutHorzAlign::Right || value == FudgetLayoutHorzAlign::Center) ? 0 :
        (value == FudgetLayoutHorzAlign::LeftGrow || value == FudgetLayoutHorzAlign::RightGrow || value == FudgetLayoutHorzAlign::CenterGrow) ? 1 :
        (value == FudgetLayoutHorzAlign::ClipLeft || value == FudgetLayoutHorzAlign::ClipRight || value == FudgetLayoutHorzAlign::ClipCenter) ? 2 : 3;

    if (slot_group != change_group)
        MarkOwnerDirty();
    else
        MarkDirty(FudgetLayoutDirtyReason::Position | FudgetLayoutDirtyReason::Size);
}

FudgetLayoutVertAlign FudgetListLayout::GetSlotVerticalAlignment(int index) const
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return FudgetLayoutVertAlign::Top;

    return slot->_vert_align;
}

void FudgetListLayout::SetSlotVerticalAlignment(int index, FudgetLayoutVertAlign value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr || slot->_vert_align == value)
        return;

    int slot_group = (slot->_vert_align == FudgetLayoutVertAlign::Top || slot->_vert_align == FudgetLayoutVertAlign::Bottom || slot->_vert_align == FudgetLayoutVertAlign::Center) ? 0 :
        (slot->_vert_align == FudgetLayoutVertAlign::TopGrow || slot->_vert_align == FudgetLayoutVertAlign::BottomGrow || slot->_vert_align == FudgetLayoutVertAlign::CenterGrow) ? 1 :
        (slot->_vert_align == FudgetLayoutVertAlign::ClipTop || slot->_vert_align == FudgetLayoutVertAlign::ClipBottom || slot->_vert_align == FudgetLayoutVertAlign::ClipCenter) ? 2 : 3;
    slot->_vert_align = value;
    int change_group = (value == FudgetLayoutVertAlign::Top || value == FudgetLayoutVertAlign::Bottom || value == FudgetLayoutVertAlign::Center) ? 0 :
        (value == FudgetLayoutVertAlign::TopGrow || value == FudgetLayoutVertAlign::BottomGrow || value == FudgetLayoutVertAlign::CenterGrow) ? 1 :
        (value == FudgetLayoutVertAlign::ClipTop || value == FudgetLayoutVertAlign::ClipBottom || value == FudgetLayoutVertAlign::ClipCenter) ? 2 : 3;

    if (slot_group != change_group)
        MarkOwnerDirty();
    else
        MarkDirty(FudgetLayoutDirtyReason::Position | FudgetLayoutDirtyReason::Size);
}

FudgetPadding& FudgetListLayout::GetSlotPadding(int index) const
{
    ASSERT(GoodSlotIndex(index));

    return GetSlot(index)->_padding;
}

void FudgetListLayout::SetSlotPadding(int index, FudgetPadding value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr || slot->_padding == value)
        return;

    slot->_padding = value;
    MarkOwnerDirty();
}

FudgetDistributedSizingRule FudgetListLayout::GetSlotSizingRule(int index) const
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return FudgetDistributedSizingRule::Exact;

    return slot->_sizing_rule;
}

void FudgetListLayout::SetSlotSizingRule(int index, FudgetDistributedSizingRule value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr || slot->_sizing_rule == value)
        return;

    slot->_sizing_rule = value;
    MarkOwnerDirty();
}

FudgetDistributedShrinkingRule FudgetListLayout::GetSlotShrinkingRule(int index) const
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return FudgetDistributedShrinkingRule::CanShrink;

    return slot->_shrinking_rule;
}

void FudgetListLayout::SetSlotShrinkingRule(int index, FudgetDistributedShrinkingRule value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr || slot->_shrinking_rule == value)
        return;

    slot->_shrinking_rule = value;
    MarkOwnerDirty();
}

Float2 FudgetListLayout::GetSlotWeight(int index) const
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return Float2::Zero;

    return slot->_weight;
}

void FudgetListLayout::SetSlotWeight(int index, Float2 value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;

    value = Float2::Max(1.f, value);
    if (Float2::NearEqual(slot->_weight, value))
        return;

    slot->_weight = value;
    MarkOwnerDirty();
}

void FudgetListLayout::PreLayoutChildren(Float2 space)
{
    if (IsUnrestrictedSpace(space))
        return;

    auto owner = GetOwner();
    int count = owner->GetChildCount();

    // This will hold the available space with cell padding subtracted.
    _no_padding_space = Relevant(space);

    // Calculate the available space
    for (int ix = 0; ix < count; ++ix)
    {
        FudgetListLayoutSlot *slot = GetSlot(ix);
        _no_padding_space -= RelevantPad(slot->_padding);
    }

    // These are necessary to know which slot can grow or needs to shrink during measurements.

    _has_expanding = false;
    _has_grow_expanding = false;
    _has_grow_exact = false;
    _has_exact = false;
    _has_shrink = false;
    _has_minimal = false;

    _space_dependent = false;
    for (int ix = 0; ix < count; ++ix)
    {
        FudgetListLayoutSlot *slot = GetSlot(ix);
        _has_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::Expanding;
        _has_grow_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding;
        _has_grow_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact;
        _has_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::Exact;
        _has_shrink |= slot->_sizing_rule == FudgetDistributedSizingRule::Shrink;
        _has_minimal |= slot->_sizing_rule == FudgetDistributedSizingRule::Minimal;
        _space_dependent |= SlotSizeFromSpace(ix);

        // This layout needs to reset the computed bounds at the start, to know for every consecutive
        // iteration what to measure.
        slot->ComputedBounds.Size = Float2(-1.f);
    }
}

bool FudgetListLayout::IsExpandingRule(FudgetDistributedSizingRule rule) const
{
    return (_has_expanding && (rule == FudgetDistributedSizingRule::Expanding || rule == FudgetDistributedSizingRule::GrowExpanding)) ||
        (!_has_expanding && (_has_grow_expanding || _has_grow_exact) && (rule == FudgetDistributedSizingRule::GrowExpanding || rule == FudgetDistributedSizingRule::GrowExact)) ||
        (!_has_expanding && !_has_grow_expanding && !_has_grow_exact && _has_exact && rule == FudgetDistributedSizingRule::Exact) ||
        (!_has_expanding && !_has_grow_expanding && !_has_grow_exact && !_has_exact && _has_shrink && rule == FudgetDistributedSizingRule::Shrink) ||
        (!_has_expanding && !_has_grow_expanding && !_has_grow_exact && !_has_exact && !_has_shrink);
}

void FudgetListLayout::LayoutChildren(Float2 space)
{
    auto owner = GetOwner();
    int count = owner->GetChildCount();


    // Will hold the calculated size of each slot based on the ratio, without the leftover space.
    Array<float> sizes(count);

    // Will hold the ratio of each control relative to each other.
    Array<float> ratios(count);
    // Adding all the ratios up, to be used for the percent calculation
    float ratio_sum = 0.f;
    // Calculating the ratios and the ratio sum

    Array<Float2> wanted_sizes(count);
    Array<Float2> min_sizes(count);
    Array<Float2> max_sizes(count);
    Array<Float2> shrink_sizes(count);

    Float2 layout_wanted = Float2::Zero;
    Float2 layout_min = Float2::Zero;
    Float2 layout_max = Float2::Zero;

    bool unrestricted = IsUnrestrictedSpace(space);
    for (int ix = 0; ix < count; ++ix)
    {
        FudgetListLayoutSlot *slot = GetSlot(ix);

        Float2 wanted = Float2::Zero;
        Float2 min = Float2::Zero;
        Float2 max = Float2::Zero;
        MeasureSlot(ix, slot->ComputedBounds.Size, wanted, min, max);

        if (!unrestricted)
        {
            wanted_sizes.Add(wanted);
            min_sizes.Add(min);
            max_sizes.Add(max);
            shrink_sizes.Add(Float2(Math::Max(wanted.X - min.X, 0.f), Math::Max(wanted.Y - min.Y, 0.f)));
        }

        RelevantRef(layout_wanted) = AddBigFloats(Relevant(layout_wanted), Relevant(wanted) + RelevantPad(slot->_padding));
        OppositeRef(layout_wanted) = Math::Max(Opposite(layout_wanted), Opposite(wanted) + OppositePad(slot->_padding));
        RelevantRef(layout_min) = AddBigFloats(Relevant(layout_min), Relevant(min) + RelevantPad(slot->_padding));
        OppositeRef(layout_min) = Math::Max(Opposite(layout_min), Opposite(min) + OppositePad(slot->_padding));
        RelevantRef(layout_max) = AddBigFloats(Relevant(layout_max), Relevant(max) + RelevantPad(slot->_padding));
        OppositeRef(layout_max) = Math::Max(Opposite(layout_max), Opposite(max) + OppositePad(slot->_padding));
    }

    SetMeasuredSizes(FudgetLayoutSizeCache(space, layout_wanted, layout_min, layout_max, _space_dependent));

    if (unrestricted)
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);

        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
            RelevantRef(shrink_sizes[ix]) = RelevantRef(wanted_sizes[ix]);

        if (_has_expanding || _has_grow_exact || _has_grow_expanding)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Shrink)
            {
                RelevantRef(wanted_sizes[ix]) -= Relevant(shrink_sizes[ix]);
                RelevantRef(shrink_sizes[ix]) = 0.f;
            }
        }
        else if (_has_expanding || _has_grow_exact || _has_grow_expanding || _has_shrink)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Minimal)
            {
                RelevantRef(wanted_sizes[ix]) -= Relevant(shrink_sizes[ix]);
                RelevantRef(shrink_sizes[ix]) = 0.f;
            }
        }

        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::KeepSize)
            RelevantRef(shrink_sizes[ix]) = 0.f;
    }

    //// The size remaining after all the wanted sizes are subtracted from the available space. This
    //// might be a negative value if the slots need more space.
    float remaining = _no_padding_space;

    for (int ix = 0; ix < count; ++ix)
    {
        float size = Relevant(wanted_sizes[ix]);
        sizes.Add(size);
        remaining -= size;
    }



    if (_no_padding_space <= 0.f)
    {
        for (int ix = 0; ix < count; ++ix)
            sizes[ix] = 0.f;
    }
    else if (remaining >= 0.f)
    {
        float weight_sum = 0.f;
        int weighted_cnt = 0;
        int unweighted_cnt = 0;

        for (int ix = 0; ix < count; ++ix)
        {
            FudgetListLayoutSlot *slot = GetSlot(ix);

            if (IsExpandingRule(slot->_sizing_rule))
            {
                if (!Math::NearEqual(sizes[ix], Relevant(max_sizes[ix])))
                {
                    weight_sum += Math::Max(0.f, Relevant(slot->_weight));
                    ++weighted_cnt;
                }
                ++unweighted_cnt;
            }
        }

        bool sizes_set = false;
        for (int ix = 0; ix < count; ++ix)
        {
            FudgetListLayoutSlot *slot = GetSlot(ix);

            float grow_ratio = unweighted_cnt == 0.f ? 1.f / count : weight_sum == 0.f ? 1.f / unweighted_cnt : 0.f;
            if (unweighted_cnt != 0 && weight_sum > 0.f && IsExpandingRule(slot->_sizing_rule) && !Math::NearEqual(sizes[ix], Relevant(max_sizes[ix])))
            {
                grow_ratio = Relevant(slot->_weight) / weight_sum;
            }

            sizes[ix] += grow_ratio * remaining;
            if (grow_ratio != 0.f)
                sizes_set = true;
        }
        if (!sizes_set)
        {
            // No grow ratio set for anything.
            for (int ix = 0; ix < count; ++ix)
            {
                sizes[ix] += remaining / ix;
            }
        }
    }
    else
    {
        float shrink_sum = 0.f;
        float late_shrink_sum = 0.f;
        for (int ix = 0; ix < count; ++ix)
        {
            float size = Relevant(shrink_sizes[ix]);
            if (size <= 0.f)
                continue;
            auto slot = GetSlot(ix);
            if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::CanShrink || slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
                shrink_sum += size;
            if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::LateShrink)
                late_shrink_sum += size;
        }

        remaining = Math::Min(-remaining, shrink_sum + late_shrink_sum);
        for (int ix = 0; ix < count; ++ix)
        {
            float size = Relevant(shrink_sizes[ix]);
            if (size <= 0.f)
                continue;
            auto slot = GetSlot(ix);
            if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::CanShrink || slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
                sizes[ix] -= Math::Min(remaining, shrink_sum) * (size / shrink_sum);
        }
        remaining -= shrink_sum;
        if (remaining > 0.f && late_shrink_sum > 0.f && !Math::NearEqual(remaining, 0.f))
        {
            for (int ix = 0; ix < count; ++ix)
            {
                float size = Relevant(shrink_sizes[ix]);
                if (size <= 0.f)
                    continue;
                auto slot = GetSlot(ix);
                if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::LateShrink)
                    sizes[ix] -= Math::Min(remaining, late_shrink_sum) * (size / late_shrink_sum);
            }
        }
    }

    Float2 pos = Float2::Zero;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        float size = Math::Max(0.f, sizes[ix]);
        float size_x = _ori == FudgetOrientation::Horizontal ? size : Math::Max(space.X - OppositePad(slot->_padding), 0.0f);
        float size_y = _ori == FudgetOrientation::Vertical ? size : Math::Max(space.Y - OppositePad(slot->_padding), 0.0f);

        slot->ComputedBounds = Rectangle(Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(size_x, size_y));

        if (_ori == FudgetOrientation::Horizontal)
            pos.X += RelevantPad(slot->_padding) + size_x;
        else
            pos.Y += RelevantPad(slot->_padding) + size_y;
    }
}

FudgetLayoutSlot* FudgetListLayout::CreateSlot(FudgetControl *control)
{
    // This function is supposed to return a FudgetlayoutSlot object, but layouts can
    // provide their own derived value. To make use easier, you can define a new GetSlot()
    // function that will hide the original call. See below.

    FudgetLayoutSlot *slot = New<FudgetListLayoutSlot>(SpawnParams(Guid::New(), FudgetLayoutSlot::TypeInitializer));
    slot->Control = control;
    return slot;
}

FudgetListLayoutSlot* FudgetListLayout::GetSlot(int index) const
{
    // Since every slot that GetSlot(...) returns was created by its layout, it's safe to
    // cast without checking. This function hides the original.

    return (FudgetListLayoutSlot*)Base::GetSlot(index);
}

FudgetLayoutFlag FudgetListLayout::GetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::LayoutOnContentReposition |
        FudgetLayoutFlag::ResizeOnContainerResize | FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::LayoutOnContentIndexChange |
        FudgetLayoutFlag::CanProvideSizes | Base::GetInitFlags();
}

void FudgetListLayout::PlaceControlInSlotRectangle(int index)
{
    auto slot = GetSlot(index);

    Float2 computed_pos = slot->ComputedBounds.Location;
    Float2 computed_size = slot->ComputedBounds.Size;

    // Making sure it's calculated.
    GetHintSize();

    if (!Math::NearEqual(computed_size.X, slot->Sizes.Size.X))
    {
        float size_X = slot->Sizes.Size.X;
        if (computed_size.X > size_X &&
            (slot->_horz_align == FudgetLayoutHorzAlign::LeftGrow ||
            slot->_horz_align == FudgetLayoutHorzAlign::RightGrow ||
            slot->_horz_align == FudgetLayoutHorzAlign::CenterGrow))
        {
            size_X = Math::Min(computed_size.X, slot->Sizes.Max.X);
        }

        if (computed_size.X > size_X + 0.1e-3)
        {
            if (slot->_horz_align != FudgetLayoutHorzAlign::Fill)
            {
                float dif = computed_size.X - size_X;
                if (slot->_horz_align == FudgetLayoutHorzAlign::Right || slot->_horz_align == FudgetLayoutHorzAlign::ClipRight || slot->_horz_align == FudgetLayoutHorzAlign::RightGrow)
                    computed_pos.X += dif;
                else if (slot->_horz_align == FudgetLayoutHorzAlign::Center || slot->_horz_align == FudgetLayoutHorzAlign::ClipCenter || slot->_horz_align == FudgetLayoutHorzAlign::CenterGrow)
                    computed_pos.X += dif * 0.5f;
                computed_size.X = size_X;
            }
        }
        else if (slot->_horz_align == FudgetLayoutHorzAlign::ClipLeft)
        {
            computed_size.X = Math::Max(slot->Sizes.Min.X, computed_size.X);
        }
        else if (slot->_horz_align == FudgetLayoutHorzAlign::ClipRight)
        {
            float dif = Math::Max(slot->Sizes.Min.X, computed_size.X) - computed_size.X;
            computed_pos.X -= dif;
            computed_size.X += dif;
        }
        else if (slot->_horz_align == FudgetLayoutHorzAlign::ClipCenter)
        {
            float dif = Math::Max(slot->Sizes.Min.X, computed_size.X) - computed_size.X;
            computed_pos.X -= dif * 0.5f;
            computed_size.X += dif;
        }
    }

    if (!Math::NearEqual(computed_size.Y, slot->Sizes.Size.Y))
    {
        float size_Y = slot->Sizes.Size.Y;
        if (computed_size.Y > size_Y &&
            (slot->_vert_align == FudgetLayoutVertAlign::TopGrow ||
            slot->_vert_align == FudgetLayoutVertAlign::BottomGrow ||
            slot->_vert_align == FudgetLayoutVertAlign::CenterGrow))
        {
            size_Y = Math::Min(computed_size.Y, slot->Sizes.Max.Y);
        }

        if (computed_size.Y > size_Y + 0.1e-3)
        {
            if (slot->_vert_align != FudgetLayoutVertAlign::Fill)
            {
                float dif = computed_size.Y - size_Y;
                if (slot->_vert_align == FudgetLayoutVertAlign::Bottom || slot->_vert_align == FudgetLayoutVertAlign::ClipBottom || slot->_vert_align == FudgetLayoutVertAlign::BottomGrow)
                    computed_pos.Y += dif;
                else if (slot->_vert_align == FudgetLayoutVertAlign::Center || slot->_vert_align == FudgetLayoutVertAlign::ClipCenter || slot->_vert_align == FudgetLayoutVertAlign::CenterGrow)
                    computed_pos.Y += dif * 0.5f;
                computed_size.Y = size_Y;
            }
        }
        else if (slot->_vert_align == FudgetLayoutVertAlign::ClipTop)
        {
            computed_size.Y = Math::Max(slot->Sizes.Min.Y, computed_size.Y);
        }
        else if (slot->_vert_align == FudgetLayoutVertAlign::ClipBottom)
        {
            float dif = Math::Max(slot->Sizes.Min.Y, computed_size.Y) - computed_size.Y;
            computed_pos.Y -= dif;
            computed_size.Y += dif;
        }
        else if (slot->_vert_align == FudgetLayoutVertAlign::ClipCenter)
        {
            float dif = Math::Max(slot->Sizes.Min.Y, computed_size.Y) - computed_size.Y;
            computed_pos.Y -= dif * 0.5f;
            computed_size.Y += dif;
        }
    }

    SetControlDimensions(index, computed_pos, computed_size);
}

