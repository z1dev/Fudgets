#include "ProxyLayout.h"
#include "../Container.h"

FudgetProxyLayout::FudgetProxyLayout(const SpawnParams &params) : Base(params)
{

}

FudgetProxyLayout::~FudgetProxyLayout()
{

}


bool FudgetProxyLayout::LayoutChildren()
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        return true;
    }
    return owner->ProxyInterfaceLayoutChildren();
}

bool FudgetProxyLayout::Measure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
    {
        CacheHintSize(Float2::Zero);
        CacheMinSize(Float2::Zero);
        CacheMaxSize(Float2::Zero);
        return false;
    }
    bool result = owner->ProxyInterfaceMeasure(available, wanted_size, min_size, max_size);
    CacheHintSize(wanted_size);
    CacheMinSize(min_size);
    CacheMaxSize(max_size);
    CacheMeasureSpace(available);
    return result;
}

FudgetLayoutSlot* FudgetProxyLayout::CreateSlot(FudgetControl *control)
{
    auto owner = ToInterface<IProxyLayoutContainer>(GetOwner());
    if (owner == nullptr)
        return nullptr;
    return owner->ProxyInterfaceCreateSlot(control);
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

