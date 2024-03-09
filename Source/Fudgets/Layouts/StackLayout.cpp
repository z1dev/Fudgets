#include "StackLayout.h"
#include "../Container.h"
#include "../Utils/Utils.h"


FudgetStackLayout::FudgetStackLayout(const SpawnParams &params) : Base(params)
{

}

void FudgetStackLayout::LayoutChildren(Int2 space, FudgetContainer *owner, int count)
{
    Int2 layout_wanted = Int2::Zero;
    Int2 layout_min = Int2::Zero;
    Int2 layout_max = Int2::Zero;

    bool unrestricted = IsUnrestrictedSpace(space);
    bool space_dependent = false;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (slot->Control->IsHiddenInLayout())
            continue;

        Int2 wanted = Int2::Zero;
        Int2 min = Int2::Zero;
        Int2 max = Int2::Zero;
        space_dependent |= MeasureSlot(ix, slot->ComputedBounds.Size, wanted, min, max);

        layout_wanted = Int2::Max(layout_wanted, wanted);
        layout_min = Int2::Max(layout_min, min);
        layout_max = Int2::Max(layout_max, max);
    }

    SetMeasuredSizes(FudgetLayoutSizeCache(space, layout_wanted, layout_min, layout_max, space_dependent));

    if (unrestricted)
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (slot->Control->IsHiddenInLayout())
            continue;

        slot->ComputedBounds = Rectangle(Float2::Zero, Float2(space));
    }
}

FudgetLayoutFlag FudgetStackLayout::GetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContainerResize |
        FudgetLayoutFlag::CanProvideSizes | Base::GetInitFlags();
}

