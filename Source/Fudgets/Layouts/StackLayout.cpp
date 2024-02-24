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

    SetMeasuredSizes(FudgetLayoutSizeCache(space, layout_wanted, layout_min, layout_max, space_dependent));

    if (unrestricted)
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        slot->ComputedBounds = Rectangle(Float2::Zero, space);
    }
}

FudgetLayoutFlag FudgetStackLayout::GetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
        FudgetLayoutFlag::CanProvideSizes | Base::GetInitFlags();
}

