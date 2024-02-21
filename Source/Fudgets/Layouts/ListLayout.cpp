#include "ListLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"



FudgetListLayoutSlot::FudgetListLayoutSlot(const SpawnParams &params) : Base(SpawnParams(Guid::New(), TypeInitializer)),
    _horz_align(FudgetHorzAlign::Left), _vert_align(FudgetVertAlign::Top), _sizing_rule(FudgetDistributedSizingRule::Exact),
    _shrinking_rule(FudgetDistributedShrinkingRule::CanShrink), _weight(1.f), _grow_ratio(1.f), _shrink_size(0.f)
{
}

FudgetListLayout::FudgetListLayout(const SpawnParams &params) : Base(params), _ori(FudgetOrientation::Horizontal)/*, _stretched(true)*/
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

FudgetHorzAlign FudgetListLayout::GetSlotHorizontalAlignment(int index) const
{
    if (!GoodSlotIndex(index))
        return FudgetHorzAlign::Left;

    return GetSlot(index)->_horz_align;
}

void FudgetListLayout::SetSlotHorizontalAlignment(int index, FudgetHorzAlign value)
{
    if (!GoodSlotIndex(index))
        return;

    GetSlot(index)->_horz_align = value;
}

FudgetVertAlign FudgetListLayout::GetSlotVerticalAlignment(int index) const
{
    if (!GoodSlotIndex(index))
        return FudgetVertAlign::Top;

    return GetSlot(index)->_vert_align;
}

void FudgetListLayout::SetSlotVerticalAlignment(int index, FudgetVertAlign value)
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

bool FudgetListLayout::LayoutChildren()
{
    auto owner = GetOwner();
    if (owner == nullptr)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        return true;
    }

    int count = owner->GetChildCount();

    if (count == 0)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        return true;
    }

    Float2 space = owner->LayoutSpace();

    // This will hold the available space with cell padding subtracted.
    float no_padding = Relevant(space);

    // Calculate the available space
    for (int ix = 0; ix < count; ++ix)
    {
        FudgetListLayoutSlot *slot = GetSlot(ix);
        no_padding -= RelevantPad(slot->_padding);
    }

    // Will hold the calculated size of each slot based on the ratio, without the leftover space.
    Array<float> sizes(count);

    do
    {
        sizes.Clear();

        // Will hold the ratio of each control relative to each other.
        Array<float> ratios(count);
        // Adding all the ratios up, to be used for the percent calculation
        float ratio_sum = 0.f;
        // Calculating the ratios and the ratio sum
        for (int ix = 0; ix < count; ++ix)
        {
            FudgetListLayoutSlot *slot = GetSlot(ix);
            float size = Relevant(slot->_wanted_size);
            float ratio = no_padding > 0.f ? size / no_padding : 0.f;
            ratio_sum += ratio;
            ratios.Add(ratio);
        }

        // The size remaining after all the wanted sizes are subtracted from the available space. This
        // might be a negative value if the slots need more space.
        float remaining = no_padding;
        for (int ix = 0; ix < count; ++ix)
        {
            FudgetListLayoutSlot *slot = GetSlot(ix);
            float size = ratio_sum != 0.f ? (ratios[ix] / ratio_sum * Relevant(slot->_wanted_size)) : 0.f;
            sizes.Add(size);
            remaining -= size;
        }

        if (remaining >= 0.f)
        {
            bool sizes_set = false;
            for (int ix = 0; ix < count; ++ix)
            {
                FudgetListLayoutSlot *slot = GetSlot(ix);
                sizes[ix] += slot->_grow_ratio * remaining;
                if (slot->_grow_ratio != 0.f)
                    sizes_set = true;
            }
            if (!sizes_set)
            {
                // No grow ratio set for anything.
                for (int ix = 0; ix < count; ++ix)
                {
                    FudgetListLayoutSlot *slot = GetSlot(ix);
                    sizes[ix] += remaining / ix;
                }
            }
        }
        else if (remaining + no_padding <= 0.f)
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
                    exact_shrink_sum += Relevant(slot->_shrink_size);
                    exact_shrink_weights += Relevant(slot->_weight);
                    ++exact_shrink_cnt;
                }
                else
                {
                    shrink_sum += Relevant(slot->_shrink_size);
                    shrink_weights += Relevant(slot->_weight);
                    ++shrink_cnt;
                }
                min_sum += Relevant(slot->_wanted_size - slot->_shrink_size);
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
                        sizes[ix] += remaining * Relevant(slot->_shrink_size) / shrink_sum;
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
                        sizes[ix] += -Relevant(slot->_shrink_size);
                        continue;
                    }
                    if (exact_shrink_weights != 0.f)
                        sizes[ix] += remaining * Relevant(slot->_weight) / exact_shrink_weights;
                    else if (exact_shrink_sum != 0.f)
                        sizes[ix] += remaining * Relevant(slot->_shrink_size) / exact_shrink_sum;
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
                        sizes[ix] += -Relevant(slot->_shrink_size) + remaining * Relevant(slot->_weight) / (exact_shrink_weights + shrink_weights);
                    else if (min_sum != 0.f)
                        sizes[ix] += -Relevant(slot->_shrink_size) + remaining * Relevant(slot->_wanted_size - slot->_shrink_size) / min_sum;
                    else
                        sizes[ix] += remaining / exact_shrink_cnt;
                }
            }
        }
    } while (RepeatMeasure(sizes));
    CacheMeasureSpace(space);

    Float2 pos = Float2::Zero;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        float size = Math::Max(0.f, sizes[ix]);

        float size_x = _ori == FudgetOrientation::Horizontal ? size : Math::Max(space.X - OppositePad(slot->_padding), 0.0f);
        float size_y = _ori == FudgetOrientation::Vertical ? size : Math::Max(space.Y - OppositePad(slot->_padding), 0.0f);

        PlaceControlInSlotRectangle(ix, slot, Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(size_x, size_y));

        if (_ori == FudgetOrientation::Horizontal)
            pos.X += RelevantPad(slot->_padding) + size_x;
        else
            pos.Y += RelevantPad(slot->_padding) + size_y;
    }

    

    return true;

    //    for (int ix = 0; ix < count; ++ix)
    //    {
    //        auto slot = GetSlot(ix);
    //        float size = sizes[ix] < 0 ? no_padding_space * ratios[ix] : sizes[ix];

    //        float size_x = _ori == FudgetOrientation::Horizontal ? size : Math::Max(space.X - OppositePad(slot->_padding), 0.0f);
    //        float size_y = _ori == FudgetOrientation::Vertical ? size : Math::Max(space.Y - OppositePad(slot->_padding), 0.0f);

    //        PlaceControlInSlotRectangle(ix, slot, Float2(pos.X + slot->_padding.Left, pos.Y + slot->_padding.Top), Float2(size_x, size_y));

    //        if (_ori == FudgetOrientation::Horizontal)
    //            pos.X += RelevantPad(slot->_padding) + size_x;
    //        else
    //            pos.Y += RelevantPad(slot->_padding) + size_y;
    //    }
    //}
}

bool FudgetListLayout::Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
{
    auto owner = GetOwner();
    if (owner == nullptr)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        CacheMaxSize(Float2::Zero);
        return false;
    }

    int count = owner->GetChildCount();

    wanted_size = Float2::Zero;
    min_size = Float2::Zero;
    max_size = Float2::Zero;

    if (count == 0)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        CacheMaxSize(Float2::Zero);
        return false;
    }

    bool space_dependent = false;
    if (Relevant(available) < 0)
    {
        for (int ix = 0; ix < count; ++ix)
        {
            auto slot = GetSlot(ix);
            Float2 control_size = Float2::Zero;
            Float2 control_min = Float2::Zero;
            Float2 control_max = Float2::Zero;
            slot->_size_from_space = slot->_control->OnMeasure(available, control_size, control_min, control_max);
            space_dependent |= slot->_size_from_space;
            RelevantRef(wanted_size) += Relevant(control_size);
            OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(control_size));

            RelevantRef(min_size) += Relevant(control_min);
            OppositeRef(min_size) = Math::Max(Opposite(min_size), Opposite(control_min));

            RelevantRef(max_size) = AddBigFloats(Relevant(max_size), Relevant(control_max));
            OppositeRef(max_size) = Math::Min(Opposite(max_size), Opposite(control_max));
        }
        return space_dependent;
    }

    bool has_expanding = false;
    bool has_grow_expanding = false;
    bool has_grow_exact = false;
    bool has_exact = false;
    bool has_shrink = false;
    bool has_minimal = false;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        Float2 control_min = Float2::Zero;
        slot->_wanted_size = 0.f;
        slot->_max_size = 0.f;
        slot->_size_from_space = slot->_control->OnMeasure(Float2(-1.f), slot->_wanted_size, control_min, slot->_max_size);
        space_dependent |= slot->_size_from_space;

        slot->_shrink_size = Float2(Math::Max(slot->_wanted_size.X - control_min.X, 0.f), Math::Max(slot->_wanted_size.Y - control_min.Y, 0.f));


        has_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::Expanding;
        has_grow_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding;
        has_grow_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact;
        has_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::Exact;
        has_shrink |= slot->_sizing_rule == FudgetDistributedSizingRule::Shrink;
        has_minimal |= slot->_sizing_rule == FudgetDistributedSizingRule::Minimal;
    }
    
    float weight_sum = 0.f;
    int weighted_cnt = 0;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if ((has_expanding && (slot->_sizing_rule == FudgetDistributedSizingRule::Expanding || slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding)) ||
                (!has_expanding && (has_grow_expanding || has_grow_exact) && (slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding || slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact)) ||
                (!has_expanding && !has_grow_expanding && !has_grow_exact && has_exact && slot->_sizing_rule == FudgetDistributedSizingRule::Exact) ||
                (!has_expanding && !has_grow_expanding && !has_grow_exact && !has_exact && has_shrink && slot->_sizing_rule == FudgetDistributedSizingRule::Shrink) ||
                (!has_expanding && !has_grow_expanding && !has_grow_exact && !has_exact && !has_shrink))
        {
            weight_sum += Math::Max(0.f, Relevant(slot->_weight));
            ++weighted_cnt;
        }

        if (has_expanding || has_grow_exact || has_grow_expanding)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Shrink)
            {
                RelevantRef(slot->_wanted_size) -= Relevant(slot->_shrink_size);
                RelevantRef(slot->_shrink_size) = 0.f;
            }
        }
        else if (has_expanding || has_grow_exact || has_grow_expanding || has_shrink)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Minimal)
            {
                RelevantRef(slot->_wanted_size) -= Relevant(slot->_shrink_size);
                RelevantRef(slot->_shrink_size) = 0.f;
            }
        }

        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
            RelevantRef(slot->_shrink_size) = 0.f;

        if (!space_dependent)
        {
            RelevantRef(wanted_size) += Relevant(slot->_wanted_size);
            OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(slot->_wanted_size));

            RelevantRef(min_size) += Relevant(slot->_wanted_size - slot->_shrink_size);
            OppositeRef(min_size) = Math::Max(Opposite(min_size), Opposite(slot->_wanted_size - slot->_shrink_size));

            RelevantRef(max_size) = AddBigFloats(Relevant(max_size), Relevant(slot->_max_size));
            OppositeRef(max_size) = Math::Min(Opposite(max_size), Opposite(slot->_max_size));
        }
    }

    if (!space_dependent)
    {
        CacheHintSize(wanted_size);
        CacheMinSize(min_size);
    }

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if ((has_expanding && (slot->_sizing_rule == FudgetDistributedSizingRule::Expanding || slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding)) ||
            (!has_expanding && (has_grow_expanding || has_grow_exact) && (slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding || slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact)) ||
            (!has_expanding && !has_grow_expanding && !has_grow_exact && has_exact && slot->_sizing_rule == FudgetDistributedSizingRule::Exact) ||
            (!has_expanding && !has_grow_expanding && !has_grow_exact && !has_exact && has_shrink && slot->_sizing_rule == FudgetDistributedSizingRule::Shrink) ||
            (!has_expanding && !has_grow_expanding && !has_grow_exact && !has_exact && !has_shrink))
            slot->_grow_ratio = weight_sum != 0.f ? Relevant(slot->_weight) / weight_sum : 1.f / weighted_cnt;
    }

    if (space_dependent)
    {
        LayoutChildren();
        wanted_size = GetHintSize();
        min_size = GetMinSize();
        //for (int ix = 0; ix < count; ++ix)
        //{
        //    auto slot = GetSlot(ix);
        //    RelevantRef(wanted_size) += Relevant(slot->_wanted_size);
        //    OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(slot->_wanted_size));

        //    RelevantRef(min_size) += Relevant(slot->_wanted_size - slot->_shrink_size);
        //    OppositeRef(min_size) = Math::Max(Opposite(min_size), Opposite(slot->_wanted_size - slot->_shrink_size));
        //}
    }

    return space_dependent;
}

bool FudgetListLayout::RepeatMeasure(const Array<float> &sizes)
{
    auto owner = GetOwner();
    if (owner == nullptr)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        return false;
    }

    int count = owner->GetChildCount();
    ASSERT(sizes.Count() == count);

    if (count == 0)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        return false;
    }

    bool size_changed = false;
    Float2 space = owner->LayoutSpace();

    bool has_expanding = false;
    bool has_grow_expanding = false;
    bool has_grow_exact = false;
    bool has_exact = false;
    bool has_shrink = false;
    bool has_minimal = false;

    Array<Float2> wanted_sizes(count);
    Array<Float2> shrink_sizes(count);
    Array<Float2> max_sizes(count);

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);

        has_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::Expanding;
        has_grow_expanding |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExpanding;
        has_grow_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::GrowExact;
        has_exact |= slot->_sizing_rule == FudgetDistributedSizingRule::Exact;
        has_shrink |= slot->_sizing_rule == FudgetDistributedSizingRule::Shrink;
        has_minimal |= slot->_sizing_rule == FudgetDistributedSizingRule::Minimal;

        if (!slot->_size_from_space)
            continue;

        Float2 size(sizes[ix]);
        OppositeRef(size) = Opposite(space);

        Float2 wanted_size = Float2::Zero;
        Float2 min_size = Float2::Zero;
        Float2 max_size = Float2::Zero;
        slot->_control->OnMeasure(size, wanted_size, min_size, max_size);

        wanted_sizes.Add(wanted_size);
        shrink_sizes.Add(Float2(Math::Max(wanted_size.X - min_size.X, 0.f), Math::Max(wanted_size.Y - min_size.Y, 0.f)));
        max_sizes.Add(max_size);
    }

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (!slot->_size_from_space)
            continue;

        Float2 wanted_size = wanted_sizes[ix];
        Float2 shrink_size = shrink_sizes[ix];
        Float2 max_size = max_sizes[ix];

        if (has_expanding || has_grow_exact || has_grow_expanding)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Shrink)
            {
                RelevantRef(wanted_size) -= Relevant(shrink_size);
                RelevantRef(shrink_size) = 0.f;
            }
        }
        else if (has_expanding || has_grow_exact || has_grow_expanding || has_shrink)
        {
            if (slot->_sizing_rule == FudgetDistributedSizingRule::Minimal)
            {
                RelevantRef(wanted_size) -= Relevant(shrink_size);
                RelevantRef(shrink_size) = 0.f;
            }
        }

        if (slot->_shrinking_rule == FudgetDistributedShrinkingRule::IgnoreMinimum)
            RelevantRef(shrink_size) = 0.f;

        if (!Math::NearEqual(wanted_size, slot->_wanted_size) || !Math::NearEqual(shrink_size, slot->_shrink_size) || !Math::NearEqual(max_size, slot->_max_size))
        {
            size_changed = true;
            slot->_wanted_size = wanted_size;
            slot->_shrink_size = shrink_size;
            slot->_max_size = max_size;
        }
    }

    Float2 wanted_size = Float2::Zero;
    Float2 min_size = Float2::Zero;
    Float2 max_size = Float2::Zero;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        RelevantRef(wanted_size) += Relevant(slot->_wanted_size);
        OppositeRef(wanted_size) = Math::Max(Opposite(wanted_size), Opposite(slot->_wanted_size));

        RelevantRef(min_size) += Relevant(slot->_wanted_size - slot->_shrink_size);
        OppositeRef(min_size) = Math::Max(Opposite(min_size), Opposite(slot->_wanted_size - slot->_shrink_size));

        RelevantRef(max_size) = AddBigFloats(Relevant(max_size), Relevant(slot->_max_size));
        OppositeRef(max_size) = Math::Min(Opposite(max_size), Opposite(slot->_max_size));
    }
    CacheHintSize(wanted_size);
    CacheMinSize(min_size);
    CacheMaxSize(max_size);

    return size_changed;
}

FudgetLayoutSlot* FudgetListLayout::CreateSlot(FudgetControl *control)
{
    // This function is supposed to return a FudgetlayoutSlot object, but layouts can
    // provide their own derived value. To make use easier, you can define a new GetSlot()
    // function that will hide the original call. See below.

    FudgetLayoutSlot *slot = New<FudgetListLayoutSlot>(SpawnParams(Guid::New(), FudgetLayoutSlot::TypeInitializer));
    slot->_control = control;
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

void FudgetListLayout::PlaceControlInSlotRectangle(int index, FudgetListLayoutSlot *slot, Float2 pos, Float2 size)
{
    Float2 control_pos(0.f);
    Float2 control_size(0.f);

    control_pos.X = pos.X;
    control_pos.Y = pos.Y;
    control_size.X = size.X;
    control_size.Y = size.Y;

    // Making sure it's calculated.
    GetHintSize();

    if (!Math::NearEqual(size.X, slot->_wanted_size.X))
    {
        if (size.X > slot->_wanted_size.X)
        {
            if (slot->_horz_align != FudgetHorzAlign::Fill /*|| slot->_enforce_limits*/)
            {
                float size_X = slot->_wanted_size.X;

                // Making sure it's calculated.
                if (slot->_horz_align == FudgetHorzAlign::Fill)
                {
                    //GetMaxSize();
                    size_X = slot->_max_size.X;
                }

                if (size_X < size.X)
                {
                    control_size.X = size_X;
                    if (slot->_horz_align == FudgetHorzAlign::Right || slot->_horz_align == FudgetHorzAlign::ClipRight)
                        control_pos.X += size.X - size_X;
                    else if (slot->_horz_align == FudgetHorzAlign::Center || slot->_horz_align == FudgetHorzAlign::ClipCenter)
                        control_pos.X += (size.X - size_X) * 0.5f;
                }
            }
        }
        else if (slot->_horz_align == FudgetHorzAlign::ClipLeft)
        {
            control_size.X = slot->_wanted_size.X;
        }
        else if (slot->_horz_align == FudgetHorzAlign::ClipRight)
        {
            control_size.X = slot->_wanted_size.X;
            control_pos.X -= slot->_wanted_size.X - size.X;
        }
    }

    if (!Math::NearEqual(size.Y, slot->_wanted_size.Y))
    {
        if (size.Y > slot->_wanted_size.Y)
        {
            if (slot->_vert_align != FudgetVertAlign::Fill /*|| slot->_enforce_limits*/)
            {
                float size_Y = slot->_wanted_size.Y;

                // Making sure it's calculated.
                if (slot->_horz_align == FudgetHorzAlign::Fill)
                {
                    //GetMaxSize();
                    size_Y = slot->_max_size.Y;
                }

                if (size_Y < size.Y)
                {
                    control_size.Y = size_Y;
                    if (slot->_vert_align == FudgetVertAlign::Bottom || slot->_vert_align == FudgetVertAlign::ClipBottom)
                        control_pos.Y += size.Y - size_Y;
                    else if (slot->_vert_align == FudgetVertAlign::Center || slot->_vert_align == FudgetVertAlign::ClipCenter)
                        control_pos.Y += (size.Y - size_Y) * 0.5f;
                }
            }
        }
        else if (slot->_vert_align == FudgetVertAlign::ClipTop)
        {
            control_size.Y = slot->_wanted_size.Y;
        }
        else if (slot->_vert_align == FudgetVertAlign::ClipBottom)
        {
            control_size.Y = slot->_wanted_size.Y;
            control_pos.Y -= slot->_wanted_size.Y - size.Y;
        }
    }


    SetControlDimensions(index, control_pos, control_size);

}

