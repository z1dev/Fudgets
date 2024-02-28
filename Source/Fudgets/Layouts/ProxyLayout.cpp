#include "ProxyLayout.h"
#include "../Container.h"

FudgetProxyLayout::FudgetProxyLayout(const SpawnParams &params) : Base(params)
{

}

FudgetProxyLayout::~FudgetProxyLayout()
{

}

void FudgetProxyLayout::SetComputedBounds(int index, Float2 pos, Float2 size)
{
    auto slot = GetSlot(index);
    slot->ComputedBounds = Rectangle(pos, size);
}

void FudgetProxyLayout::SetControlSizes(const FudgetLayoutSizeCache &sizes)
{
    SetMeasuredSizes(sizes);
}

bool FudgetProxyLayout::GetSlotMeasures(int index, Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &wanted_min, API_PARAM(Out) Float2 &wanted_max)
{
    return MeasureSlot(index, available, wanted_size, wanted_min, wanted_max);
}

void FudgetProxyLayout::PreLayoutChildren(Float2 space, FudgetContainer *owner, int count)
{
    auto iowner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
        return;
    iowner->ProxyInterfacePreLayoutChildren(space);
}

void FudgetProxyLayout::LayoutChildren(Float2 space, FudgetContainer *owner, int count)
{
    auto iowner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
        return;
    iowner->ProxyInterfaceLayoutChildren(space);
}

//bool FudgetProxyLayout::Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
//{
//    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
//    if (owner == nullptr)
//    {
//        CacheHintSize(Float2::Zero);
//        CacheMinSize(Float2::Zero);
//        CacheMaxSize(Float2::Zero);
//        return false;
//    }
//    bool result = owner->ProxyInterfaceMeasure(available, wanted_size, min_size, max_size);
//    CacheHintSize(wanted_size);
//    CacheMinSize(min_size);
//    CacheMaxSize(max_size);
//    CacheMeasureSpace(available);
//    return result;
//}

FudgetLayoutSlot* FudgetProxyLayout::CreateSlot(FudgetControl *control)
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
        return nullptr;
    FudgetLayoutSlot *slot = owner->ProxyInterfaceCreateSlot(control);
    if (slot == nullptr)
    {
        slot = New<FudgetLayoutSlot>();
        slot->Control = control;
    }
    return slot;
}

FudgetLayoutFlag FudgetProxyLayout::GetInitFlags() const
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
        return Base::GetInitFlags();
    return owner->ProxyInterfaceGetInitFlags();
}

void FudgetProxyLayout::SetControlDimensions(int index, Float2 pos, Float2 size)
{
    Base::SetControlDimensions(index, pos, size);
}

void FudgetProxyLayout::PlaceControlInSlotRectangle(int index)
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr || !owner->ProxyInterfacePlaceControlInSlotRectangle(index))
        Base::PlaceControlInSlotRectangle(index);
}
