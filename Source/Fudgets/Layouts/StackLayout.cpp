#include "StackLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"


FudgetStackLayout::FudgetStackLayout(const SpawnParams &params) : Base(params)
{

}

void FudgetStackLayout::LayoutChildren(Float2 space)
{
    auto owner = GetOwner();
    int count = owner->GetChildCount();

    Float2 layout_wanted = Float2::Zero;
    Float2 layout_min = Float2::Zero;
    Float2 layout_max = Float2::Zero;

    bool unrestricted = IsUnrestrictedSpace(space);
    bool space_dependent = false;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);

        Float2 wanted = Float2::Zero;
        Float2 min = Float2::Zero;
        Float2 max = Float2::Zero;
        space_dependent |= MeasureSlot(ix, slot->ComputedBounds.Size, wanted, min, max);

        layout_wanted = Float2::Max(layout_wanted, wanted);
        layout_min = Float2::Max(layout_min, min);
        layout_max = Float2::Max(layout_max, max);
    }

    SetMeasuredSizes(space, layout_wanted, layout_min, layout_max, space_dependent);

    if (unrestricted)
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        slot->ComputedBounds = Rectangle(Float2::Zero, space);
    }
}

//bool FudgetStackLayout::Measure(FudgetContainer *owner, int count, Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
//{
//    auto owner = GetOwner();
//    int count = owner->GetChildCount();
//
//    bool space_dependent = false;
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        slot->_size_from_space = slot->_control->OnMeasure(available, slot->_wanted_size, slot->_min_size, slot->_max_size);
//        space_dependent |= slot->_size_from_space;
//
//        if (!space_dependent)
//        {
//            Float2::Max(wanted_size, slot->_wanted_size);
//            Float2::Max(min_size, slot->_min_size);
//            Float2::Min(max_size, slot->_max_size);
//        }
//    }
//
//    if (!space_dependent)
//    {
//        CacheHintSize(wanted_size);
//        CacheMinSize(min_size);
//        CacheMinSize(max_size);
//    }
//
//    if (space_dependent)
//    {
//        LayoutChildren();
//        for (int ix = 0; ix < count; ++ix)
//        {
//            auto slot = GetSlot(ix);
//            wanted_size = GetHintSize();
//            min_size = GetMinSize();
//            max_size = GetMaxSize();
//            //Float2::Max(wanted_size, slot->_wanted_size);
//            //Float2::Max(min_size, slot->_min_size);
//        }
//    }
//
//    return space_dependent;
//}
//
//bool FudgetStackLayout::RepeatMeasure(Float2 available)
//{
//    auto owner = GetOwner();
//    if (owner == nullptr)
//    {
//        CacheHintSize(Float2::Zero);
//        CacheMinSize(Float2::Zero);
//        CacheMaxSize(Float2::Zero);
//        return false;
//    }
//
//
//    int count = owner->GetChildCount();
//
//    if (count == 0)
//    {
//        CacheHintSize(Float2::Zero);
//        CacheMinSize(Float2::Zero);
//        CacheMaxSize(Float2::Zero);
//        return false;
//    }
//
//    Array<Float2> wanted_sizes(count);
//    Array<Float2> min_sizes(count);
//
//    bool size_changed = false;
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        if (!slot->_size_from_space)
//            continue;
//
//        Float2 wanted_size = Float2::Zero;
//        Float2 min_size = Float2::Zero;
//        Float2 max_size = Float2::Zero;
//        slot->_control->OnMeasure(available, wanted_size, min_size, max_size);
//        if (!Math::NearEqual(wanted_size, slot->_wanted_size) || !Math::NearEqual(min_size, slot->_min_size) || !Math::NearEqual(max_size, slot->_max_size))
//        {
//            size_changed = true;
//            slot->_wanted_size = wanted_size;
//            slot->_min_size = min_size;
//            slot->_max_size = max_size;
//        }
//    }
//
//    Float2 wanted_size = Float2::Zero;
//    Float2 min_size = Float2::Zero;
//    Float2 max_size = Float2::Zero;
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        Float2::Max(wanted_size, slot->_wanted_size);
//        Float2::Max(min_size, slot->_min_size);
//        Float2::Min(max_size, slot->_max_size);
//    }
//
//    CacheHintSize(wanted_size);
//    CacheMinSize(min_size);
//    CacheMinSize(max_size);
//
//    return size_changed;
//}

FudgetLayoutFlag FudgetStackLayout::GetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
        FudgetLayoutFlag::CanProvideSizes | Base::GetInitFlags();
}

