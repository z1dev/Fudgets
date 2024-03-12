#include "AnchorLayout.h"
#include "../Container.h"

FudgetAnchorLayoutSlot::FudgetAnchorLayoutSlot(const SpawnParams &params) : Base(params),
    leftAnchor(FudgetHorizontalAnchor::None), rightAnchor(FudgetHorizontalAnchor::None),
    topAnchor(FudgetVerticalAnchor::None), bottomAnchor(FudgetVerticalAnchor::None),
    leftPercent(0.f), rightPercent(0.f), topPercent(0.f), bottomPercent(0.f)
{

}

FudgetAnchorLayout::FudgetAnchorLayout(const SpawnParams &params) : Base(params)
{

}

void FudgetAnchorLayout::LayoutChildren(Int2 space, FudgetContainer *owner, int count)
{
    // All controls sizes will be their hint size if the anchors don't change the control's size. For anchors that affect
    // the size too, the control's requested sizes are ignored. If the resulting size would be negative, the control will
    // have 0 size.

    SetMeasuredSizes(FudgetLayoutSizeCache(space, Int2::Zero, Int2::Zero, Int2::Zero, false));

    if (IsUnrestrictedSpace(space))
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (slot->Control->IsHiddenInLayout())
            continue;

        Int2 wanted = Int2::Zero;
        Int2 min = Int2::Zero;
        Int2 max = Int2::Zero;
        MeasureSlot(ix, slot->ComputedBounds.Size, wanted, min, max);

        Int2 control_size = Int2::Zero;
        Int2 control_pos = Int2::Zero;

        // Horizontal first:
        if (slot->leftAnchor == FudgetHorizontalAnchor::None && slot->rightAnchor == FudgetHorizontalAnchor::None)
        {
            float center = (slot->leftPercent + slot->rightPercent) * 0.5f;
            control_size.X = wanted.X;
            control_pos.X = int((space.X * center) - control_size.X * 0.5f);
        }
        else if (slot->rightAnchor == FudgetHorizontalAnchor::None)
        {
            control_size.X = wanted.X;
            if (slot->leftAnchor == FudgetHorizontalAnchor::Left)
                control_pos.X = int(space.X * slot->leftPercent);
            else
                control_pos.X = int(space.X * (1.0f - slot->leftPercent));
        }
        else if (slot->leftAnchor == FudgetHorizontalAnchor::None)
        {
            control_size.X = wanted.X;
            if (slot->rightAnchor == FudgetHorizontalAnchor::Right)
                control_pos.X = int(space.X * (1 - slot->rightPercent) - control_size.X);
            else
                control_pos.X = int(space.X * slot->rightPercent);
        }
        else
        {
            float left;
            if (slot->leftAnchor == FudgetHorizontalAnchor::Left)
                left = space.X * slot->leftPercent;
            else
                left = space.X * (1.0f - slot->leftPercent);
            float right;
            if (slot->rightAnchor == FudgetHorizontalAnchor::Right)
                right = space.X * (1 - slot->rightPercent);
            else
                right = space.X * slot->rightPercent;

            if (left >= right)
            {
                control_size.X = 0;
                control_pos.X = (int)((slot->leftPercent + slot->rightPercent) * 0.5f * space.X);
            }
            else
            {
                control_size.X = int(right - left);
                control_pos.X = int(left);
            }
        }

        // Vertical:
        if (slot->topAnchor == FudgetVerticalAnchor::None && slot->bottomAnchor == FudgetVerticalAnchor::None)
        {
            float center = (slot->topPercent + slot->bottomPercent) * 0.5f;
            control_size.Y = wanted.Y;
            control_pos.Y = int((space.Y * center) - control_size.Y * 0.5f);
        }
        else if (slot->bottomAnchor == FudgetVerticalAnchor::None)
        {
            control_size.Y = wanted.Y;
            if (slot->topAnchor == FudgetVerticalAnchor::Top)
                control_pos.Y = int(space.Y * slot->topPercent);
            else
                control_pos.Y = int(space.Y * (1.0f - slot->topPercent));
        }
        else if (slot->topAnchor == FudgetVerticalAnchor::None)
        {
            control_size.Y = wanted.Y;
            if (slot->bottomAnchor == FudgetVerticalAnchor::Bottom)
                control_pos.Y = int(space.Y * (1 - slot->bottomPercent) - control_size.Y);
            else
                control_pos.Y = int(space.Y * slot->bottomPercent);
        }
        else
        {
            float top;
            if (slot->topAnchor == FudgetVerticalAnchor::Top)
                top = space.Y * slot->topPercent;
            else
                top = space.Y * (1.0f - slot->topPercent);
            float bottom;
            if (slot->bottomAnchor == FudgetVerticalAnchor::Bottom)
                bottom = space.Y * (1 - slot->bottomPercent);
            else
                bottom = space.Y * slot->bottomPercent;

            if (top >= bottom)
            {
                control_size.Y = 0;
                control_pos.Y = int((slot->topPercent + slot->bottomPercent) * 0.5f * space.Y);
            }
            else
            {
                control_size.Y = int(bottom - top);
                control_pos.Y = int(top);
            }
        }

        slot->ComputedBounds = Rectangle(Float2(control_pos), Float2(control_size));
    }
}

FudgetAnchorLayoutSlot* FudgetAnchorLayout::GetSlot(int index)
{
    return (FudgetAnchorLayoutSlot*)Base::GetSlot(index);
}

const FudgetAnchorLayoutSlot* FudgetAnchorLayout::GetSlot(int index) const
{
    return (FudgetAnchorLayoutSlot*)Base::GetSlot(index);
}

FudgetLayoutFlag FudgetAnchorLayout::GetInitFlags() const
{
    return Base::GetInitFlags() | FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::ResizeOnContentResize;
}

FudgetLayoutSlot* FudgetAnchorLayout::CreateSlot(FudgetControl *control)
{
    auto slot = New<FudgetAnchorLayoutSlot>();
    slot->Control = control;
    return slot;
}

FudgetHorizontalAnchor FudgetAnchorLayout::GetLeftAnchor(int index) const { return GetSlot(index)->leftAnchor; }
void FudgetAnchorLayout::SetLeftAnchor(int index, FudgetHorizontalAnchor value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->leftAnchor = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}

FudgetHorizontalAnchor FudgetAnchorLayout::GetRightAnchor(int index) const { return GetSlot(index)->rightAnchor; }
void FudgetAnchorLayout::SetRightAnchor(int index, FudgetHorizontalAnchor value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->rightAnchor = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}

FudgetVerticalAnchor FudgetAnchorLayout::GetTopAnchor(int index) const { return GetSlot(index)->topAnchor; }
void FudgetAnchorLayout::SetTopAnchor(int index, FudgetVerticalAnchor value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->topAnchor = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}
FudgetVerticalAnchor FudgetAnchorLayout::GetBottomAnchor(int index) const { return GetSlot(index)->bottomAnchor; }
void FudgetAnchorLayout::SetBottomAnchor(int index, FudgetVerticalAnchor value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->bottomAnchor = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}

float FudgetAnchorLayout::GetLeftPercent(int index) const { return GetSlot(index)->leftPercent; }
void FudgetAnchorLayout::SetLeftPercent(int index, float value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->leftPercent = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}
float FudgetAnchorLayout::GetRightPercent(int index) const { return GetSlot(index)->rightPercent; }
void FudgetAnchorLayout::SetRightPercent(int index, float value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->rightPercent = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}
float FudgetAnchorLayout::GetTopPercent(int index) const { return GetSlot(index)->topPercent; }
void FudgetAnchorLayout::SetTopPercent(int index, float value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->topPercent = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}
float FudgetAnchorLayout::GetBottomPercent(int index) const { return GetSlot(index)->bottomPercent; }
void FudgetAnchorLayout::SetBottomPercent(int index, float value)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->bottomPercent = value;
    if (slot->Control->IsHiddenInLayout())
        return;
    slot->Control->SizeModified();
    MarkDirty(FudgetLayoutDirtyReason::SizePos);
}
