#include "ContainerLayout.h"
#include "../Container.h"

FudgetContainerLayout::FudgetContainerLayout(const SpawnParams &params) : Base(params)
{

}

bool FudgetContainerLayout::LayoutChildren()
{
    auto owner = GetOwner();
    if (owner == nullptr)
        return true;
    int count = owner->GetChildCount();
    if (count == 0)
        return true;
    Float2 space = owner->LayoutSpace();

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        SetControlDimensions(ix, slot->_control->GetHintPosition(), slot->_wanted_size /*slot->_control->GetHintSize()*/);
    }
    return true;
}

bool FudgetContainerLayout::Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
{
    CacheMeasureSpace(available);
    CacheHintSize(0.f);
    CacheMinSize(0.f);
    CacheMaxSize(0.f);
    wanted_size = Float2::Zero;
    min_size = Float2::Zero;

    auto owner = GetOwner();
    if (owner == nullptr)
        return false;
    int count = owner->GetChildCount();
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        slot->_size_from_space = slot->_control->OnMeasure(Float2(-1.f), slot->_wanted_size, slot->_min_size, slot->_max_size);
    }
    return false;
}

