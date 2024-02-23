#include "ContainerLayout.h"
#include "../Container.h"

FudgetContainerLayout::FudgetContainerLayout(const SpawnParams &params) : Base(params)
{

}

//void FudgetContainerLayout::LayoutChildren(Float2 space)
//{
//    auto owner = GetOwner();
//    int count = owner->GetChildCount();
//
//    Float2 space = owner->LayoutSpace();
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        SetControlDimensions(ix, slot->Control->GetHintPosition(), slot->UnrestrictedSizes.Size);
//    }
//    
//    SetMeasuredSizes(space, 0.f, 0.f, 0.f, false);
//}

//bool FudgetContainerLayout::Measure(FudgetContainer *owner, int count, Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
//{
//    wanted_size = Float2::Zero;
//    min_size = Float2::Zero;
//
//    for (int ix = 0; ix < count; ++ix)
//    {
//        auto slot = GetSlot(ix);
//        slot->_size_from_space = slot->_control->OnMeasure(Float2(-1.f), slot->_wanted_size, slot->_min_size, slot->_max_size);
//    }
//    return false;
//}

