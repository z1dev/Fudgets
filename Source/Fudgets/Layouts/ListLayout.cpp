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
    _ori = value;
    MarkDirty(FudgetLayoutDirtyReason::All | FudgetLayoutDirtyReason::Container);
}

//void FudgetListLayout::SetStretched(bool value)
//{
//    _stretched = value;
//    MarkDirty(FudgetLayoutDirtyReason::All, true);
//}

FudgetLayoutHorzAlign FudgetListLayout::GetSlotHorizontalAlignment(int index) const
{
    if (!GoodSlotIndex(index))
        return FudgetLayoutHorzAlign::Left;

    return GetSlot(index)->_horz_align;
}

void FudgetListLayout::SetSlotHorizontalAlignment(int index, FudgetLayoutHorzAlign value)
{
    if (!GoodSlotIndex(index))
        return;

    GetSlot(index)->_horz_align = value;
}

FudgetLayoutVertAlign FudgetListLayout::GetSlotVerticalAlignment(int index) const
{
    if (!GoodSlotIndex(index))
        return FudgetLayoutVertAlign::Top;

    return GetSlot(index)->_vert_align;
}

void FudgetListLayout::SetSlotVerticalAlignment(int index, FudgetLayoutVertAlign value)
{
    if (!GoodSlotIndex(index))
        return;

    GetSlot(index)->_vert_align = value;
}

//bool FudgetListLayout::GetSlotLimitsEnforced(int index) const
//{
//    if (!GoodSlotIndex(index))
//        return false;
//
//    return GetSlot(index)->_enforce_limits;
//}
//
//void FudgetListLayout::SetSlotLimitsEnforced(int index, bool value)
//{
//    if (!GoodSlotIndex(index))
//        return;
//
//    GetSlot(index)->_enforce_limits = value;
//}

FudgetPadding& FudgetListLayout::GetSlotPadding(int index) const
{
    ASSERT(GoodSlotIndex(index));

    return GetSlot(index)->_padding;
}

void FudgetListLayout::SetSlotPadding(int index, FudgetPadding value)
{
    if (!GoodSlotIndex(index))
        return;

    GetSlot(index)->_padding = value;
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
        _space_dependent |= slot->UnrestrictedSizes.SizeFromSpace;

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

    SetMeasuredSizes(space, layout_wanted, layout_min, layout_max, _space_dependent);

    if (unrestricted)
        return;


    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);

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

        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
            RelevantRef(shrink_sizes[ix]) = RelevantRef(wanted_sizes[ix]);
    }

    for (int ix = 0; ix < count; ++ix)
    {
        float size = Relevant(wanted_sizes[ix]);
        float ratio = _no_padding_space > 0.f ? size / _no_padding_space : 0.f;
        ratio_sum += ratio;
        ratios.Add(ratio);
    }

    // The size remaining after all the wanted sizes are subtracted from the available space. This
    // might be a negative value if the slots need more space.
    float remaining = _no_padding_space;

    for (int ix = 0; ix < count; ++ix)
    {
        float size = Math::Min(Relevant(max_sizes[ix]), ratio_sum != 0.f ? (ratios[ix] / ratio_sum * Relevant(wanted_sizes[ix])) : 0.f);
        sizes.Add(size);
        remaining -= size;

    }

    if (remaining >= 0.f)
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
    else if (remaining + _no_padding_space <= 0.f)
    {
        for (int ix = 0; ix < count; ++ix)
            sizes[ix] = 0.f;
    }
    else
    {
        float shrink_sum = 0.f;
        float exact_shrink_sum = 0.f;
        float shrink_weights = 0.f;
        float exact_shrink_weights = 0.f;
        float min_sum = 0.f;
        int shrink_cnt = 0;
        int exact_shrink_cnt = 0;
        for (int ix = 0; ix < count; ++ix)
        {
            FudgetListLayoutSlot *slot = GetSlot(ix);
            if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::Exact)
            {
                exact_shrink_sum += Relevant(shrink_sizes[ix]);
                exact_shrink_weights += Relevant(slot->_weight);
                ++exact_shrink_cnt;
            }
            else
            {
                shrink_sum += Relevant(shrink_sizes[ix]);
                shrink_weights += Relevant(slot->_weight);
                ++shrink_cnt;
            }
            min_sum += Relevant(wanted_sizes[ix] - shrink_sizes[ix]);
        }

        if (shrink_sum + remaining >= 0.f)
        {
            // Only need to shrink the slots with the shrinking rule.
            for (int ix = 0; ix < count; ++ix)
            {
                FudgetListLayoutSlot *slot = GetSlot(ix);
                if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::Exact)
                    continue;
                if (shrink_weights != 0.f)
                    sizes[ix] += remaining * Relevant(slot->_weight) / shrink_weights;
                else if (shrink_sum != 0.f)
                    sizes[ix] += remaining * Relevant(shrink_sizes[ix]) / shrink_sum;
                else
                    sizes[ix] += remaining / shrink_cnt;
            }
        }
        else if (shrink_sum + exact_shrink_sum + remaining >= 0.f)
        {
            remaining += shrink_sum;
            // Shrinking slots all reached their minimum size, but the exact shrinking rule slots can
            // make up for it.
            // Only need to shrink the slots with the shrinking rule.
            for (int ix = 0; ix < count; ++ix)
            {
                FudgetListLayoutSlot *slot = GetSlot(ix);
                if (slot->_shrinking_rule != FudgetDistributedShrinkingRule::Exact)
                {
                    sizes[ix] += -Relevant(shrink_sizes[ix]);
                    continue;
                }
                if (exact_shrink_weights != 0.f)
                    sizes[ix] += remaining * Relevant(slot->_weight) / exact_shrink_weights;
                else if (exact_shrink_sum != 0.f)
                    sizes[ix] += remaining * Relevant(shrink_sizes[ix]) / exact_shrink_sum;
                else
                    sizes[ix] += remaining / exact_shrink_cnt;
            }
        }
        else
        {
            remaining += shrink_sum + exact_shrink_sum;
            // All slots must shrink below the minimum size based on their weight
            for (int ix = 0; ix < count; ++ix)
            {
                FudgetListLayoutSlot *slot = GetSlot(ix);
                if (exact_shrink_weights + shrink_weights != 0.f)
                    sizes[ix] += -Relevant(shrink_sizes[ix]) + remaining * Relevant(slot->_weight) / (exact_shrink_weights + shrink_weights);
                else if (min_sum != 0.f)
                    sizes[ix] += -Relevant(shrink_sizes[ix]) + remaining * Relevant(wanted_sizes[ix] - shrink_sizes[ix]) / min_sum;
                else
                    sizes[ix] += remaining / exact_shrink_cnt;
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


    //for (int ix = 0; ix < count; ++ix)
    //{
    //    auto slot = GetSlot(ix);
    //    float size = Math::Max(0.f, sizes[ix]);

    //    float size_x = _ori == FudgetOrientation::Horizontal ? size : Math::Max(space.X - OppositePad(slot->_padding), 0.0f);
    //    float size_y = _ori == FudgetOrientation::Vertical ? size : Math::Max(space.Y - OppositePad(slot->_padding), 0.0f);

    //    PlaceControlInSlotRectangle(ix, slot, Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(size_x, size_y));

    //    if (_ori == FudgetOrientation::Horizontal)
    //        pos.X += RelevantPad(slot->_padding) + size_x;
    //    else
    //        pos.Y += RelevantPad(slot->_padding) + size_y;
    //}


}

//bool FudgetListLayout::Measure(FudgetContainer *owner, int count, Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &wanted_min, API_PARAM(Out) Float2 &wanted_max)
//{
//    bool space_dependent = false;
//    wanted_size = Float2::Zero;
//    wanted_min = Float2::Zero;
//    wanted_max = Float2::Zero;
//    if (IsUnrestrictedSpace(available))
//    {
//        for (int ix = 0; ix < count; ++ix)
//        {
//            Float2 size = Float2::Zero;
//            Float2 min = Float2::Zero;
//            Float2 max = Float2::Zero;
//            space_dependent |= MeasureSlot(ix, available, size, min, max);
//
//            RelevantRef(wanted_size) = AddBigFloats(RelevantRef(wanted_size), Relevant(size));
//            OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(size));
//
//            RelevantRef(wanted_min) = AddBigFloats(RelevantRef(wanted_min), Relevant(min));
//            OppositeRef(wanted_min) = Math::Max(Opposite(wanted_min), Opposite(min));
//
//            RelevantRef(wanted_max) = AddBigFloats(Relevant(wanted_max), Relevant(max));
//            OppositeRef(wanted_max) = Math::Min(Opposite(wanted_max), Opposite(max));
//        }
//        return space_dependent;
//    }
//
//    bool has_expanding = false;
//    bool has_grow_expanding = false;
//    bool has_grow_exact = false;
//    bool has_exact = false;
//    bool has_shrink = false;
//    bool has_minimal = false;
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//
//        Float2 size = Float2::Zero;
//        Float2 min = Float2::Zero;
//        Float2 max = Float2::Zero;
//        space_dependent |= MeasureSlot(ix, -1.f, size, min, max);
//
//        slot->_wanted_size = size;
//        slot->_shrink_size = Float2(Math::Max(size.X - min.X, 0.f), Math::Max(size.Y - min.Y, 0.f));
//
//        has_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::Expanding;
//        has_grow_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding;
//        has_grow_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact;
//        has_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::Exact;
//        has_shrink |= slot->_sizing_rule == FudgetDistributedSizingRule::Shrink;
//        has_minimal |= slot->_sizing_rule == FudgetDistributedSizingRule::Minimal;
//    }
//    
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//
//        if (has_expanding || has_grow_exact || has_grow_expanding)
//        {
//            if (slot->_sizing_rule == FudgetDistributedSizingRule::Shrink)
//            {
//                RelevantRef(slot->_wanted_size) -= Relevant(slot->_shrink_size);
//                RelevantRef(slot->_shrink_size) = 0.f;
//            }
//        }
//        else if (has_expanding || has_grow_exact || has_grow_expanding || has_shrink)
//        {
//            if (slot->_sizing_rule == FudgetDistributedSizingRule::Minimal)
//            {
//                RelevantRef(slot->_wanted_size) -= Relevant(slot->_shrink_size);
//                RelevantRef(slot->_shrink_size) = 0.f;
//            }
//        }
//
//        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
//            RelevantRef(slot->_shrink_size) = RelevantRef(slot->_wanted_size);
//
//        RelevantRef(wanted_size) = AddBigFloats(RelevantRef(wanted_size), Relevant(slot->_wanted_size));
//        OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(slot->_wanted_size));
//
//        RelevantRef(wanted_min) = AddBigFloats(RelevantRef(wanted_min), Relevant(slot->_wanted_size - slot->_shrink_size));
//        OppositeRef(wanted_min) = Math::Max(Opposite(wanted_min), Opposite(slot->_wanted_size - slot->_shrink_size));
//
//        RelevantRef(wanted_max) = AddBigFloats(Relevant(wanted_max), Relevant(slot->UnrestrictedSizes.Max));
//        OppositeRef(wanted_max) = Math::Min(Opposite(wanted_max), Opposite(slot->UnrestrictedSizes.Max));
//    }
//
//
//    return space_dependent;
//}
//
//bool FudgetListLayout::RepeatMeasure(const Array<float> &sizes)
//{
//    auto owner = GetOwner();
//    int count = owner->GetChildCount();
//
//    bool size_changed = false;
//    Float2 space = owner->LayoutSpace();
//
//    bool has_expanding = false;
//    bool has_grow_expanding = false;
//    bool has_grow_exact = false;
//    bool has_exact = false;
//    bool has_shrink = false;
//    bool has_minimal = false;
//
//    Array<Float2> wanted_sizes(count);
//    Array<Float2> shrink_sizes(count);
//    Array<Float2> max_sizes(count);
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//
//        has_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::Expanding;
//        has_grow_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding;
//        has_grow_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact;
//        has_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::Exact;
//        has_shrink |= slot->_sizing_rule == FudgetDistributedSizingRule::Shrink;
//        has_minimal |= slot->_sizing_rule == FudgetDistributedSizingRule::Minimal;
//
//        if (!slot->_size_from_space)
//            continue;
//
//        Float2 size(sizes[ix]);
//        OppositeRef(size) = Opposite(space);
//
//        Float2 wanted_size = Float2::Zero;
//        Float2 wanted_min = Float2::Zero;
//        Float2 max_size = Float2::Zero;
//        slot->_control->OnMeasure(size, wanted_size, wanted_min, max_size);
//
//        wanted_sizes.Add(wanted_size);
//        shrink_sizes.Add(Float2(Math::Max(wanted_size.X - wanted_min.X, 0.f), Math::Max(wanted_size.Y - wanted_min.Y, 0.f)));
//        max_sizes.Add(max_size);
//    }
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        if (!slot->_size_from_space)
//            continue;
//
//        Float2 wanted_size = wanted_sizes[ix];
//        Float2 shrink_size = shrink_sizes[ix];
//        Float2 max_size = max_sizes[ix];
//
//        if (has_expanding || has_grow_exact || has_grow_expanding)
//        {
//            if (slot->_sizing_rule == FudgetDistributedSizingRule::Shrink)
//            {
//                RelevantRef(wanted_size) -= Relevant(shrink_size);
//                RelevantRef(shrink_size) = 0.f;
//            }
//        }
//        else if (has_expanding || has_grow_exact || has_grow_expanding || has_shrink)
//        {
//            if (slot->_sizing_rule == FudgetDistributedSizingRule::Minimal)
//            {
//                RelevantRef(wanted_size) -= Relevant(shrink_size);
//                RelevantRef(shrink_size) = 0.f;
//            }
//        }
//
//        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
//            RelevantRef(shrink_size) = RelevantRef(wanted_size);
//
//        if (!Math::NearEqual(wanted_size, slot->_wanted_size) || !Math::NearEqual(shrink_size, slot->_shrink_size) || !Math::NearEqual(max_size, slot->_max_size))
//        {
//            size_changed = true;
//            slot->_wanted_size = wanted_size;
//            slot->_shrink_size = shrink_size;
//            slot->_max_size = max_size;
//        }
//    }
//
//    Float2 wanted_size = Float2::Zero;
//    Float2 wanted_min = Float2::Zero;
//    Float2 max_size = Float2::Zero;
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        RelevantRef(wanted_size) += Relevant(slot->_wanted_size);
//        OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(slot->_wanted_size));
//
//        RelevantRef(wanted_min) += Relevant(slot->_wanted_size - slot->_shrink_size);
//        OppositeRef(wanted_min) = Math::Max(Opposite(wanted_min), Opposite(slot->_wanted_size - slot->_shrink_size));
//
//        RelevantRef(max_size) = AddBigFloats(Relevant(max_size), Relevant(slot->_max_size));
//        OppositeRef(max_size) = Math::Min(Opposite(max_size), Opposite(slot->_max_size));
//    }
//    CacheHintSize(wanted_size);
//    CacheMinSize(wanted_min);
//    CacheMaxSize(max_size);
//
//    return size_changed;
//}

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
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
        FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::LayoutOnContentIndexChange | FudgetLayoutFlag::CanProvideSizes |
        Base::GetInitFlags();
}

//Float2 FudgetListLayout::RequestSize(FudgetSizeType type) const
//{
//    auto owner = GetOwner();
//    if (owner == nullptr)
//        return Float2(0.0f);
//    int count = GetOwner()->GetChildCount();
//    if (count == 0.f)
//        return Float2(0.f);
//
//    Float2 result(0.0f);
//
//    for (int ix = 0, siz = count; ix < siz; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        Float2 pad = Float2(slot->_padding.Width(), slot->_padding.Height());
//        Float2 size = slot->_control->GetRequestedSize(type);
//        Float2 corrected_size = size;
//
//        if (type != FudgetSizeType::Max)
//        {
//            if (Relevant(corrected_size) < 0)
//                RelevantRef(corrected_size) = 0;
//            if (Opposite(corrected_size) < 0)
//                OppositeRef(corrected_size) = 0;
//        }
//        else
//        {
//            if (Relevant(corrected_size) < 0 || Relevant(corrected_size) > MaximumFloatLimit)
//                RelevantRef(corrected_size) = MaximumFloatLimit;
//            if (Opposite(corrected_size) < 0 || Opposite(corrected_size) > MaximumFloatLimit)
//                OppositeRef(corrected_size) = MaximumFloatLimit;
//        }
//
//        // Overflow checking replacement of RelevantRef(result) += Relevant(corrected_size)
//        RelevantRef(result) = AddBigFloats(Relevant(pad), AddBigFloats(Relevant(result), Relevant(corrected_size)));
//        OppositeRef(result) = Math::Max(Opposite(result), AddBigFloats(Opposite(pad), Opposite(corrected_size)));
//
//        switch (type)
//        {
//            case FudgetSizeType::Hint:
//                slot->_hint_size = corrected_size;
//                break;
//            case FudgetSizeType::Min:
//                slot->_min_size = corrected_size;
//                break;
//            case FudgetSizeType::Max:
//                slot->_max_size = corrected_size;
//                break;
//        }
//    }
//
//    return result;
//}

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

