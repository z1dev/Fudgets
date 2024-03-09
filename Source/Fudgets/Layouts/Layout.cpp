#include "Layout.h"
#include "../Container.h"
#include "../Utils/Utils.h"

//// These could be in any cpp file but it's most likely to be used in layouts so whatever. Declared as extern in Utils.h
//const float MaximumFloatLimit = (std::numeric_limits<float>::max() - 1.0f);

float AddBigValues(float a, float b)
{
    // Should be safe:
    if ((a > 0) != (b > 0))
        return a + b;
    if (a < 0 && b < 0)
        return -AddBigValues(-a, -b);

    if (MAX_float -a < b)
        return MAX_float;

    return a + b;
}

float AddBigValues(float a, float b, float c)
{
    return AddBigValues(AddBigValues(a, b), c);
}

float AddBigValues(float a, float b, float c, float d)
{
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}

Float2 AddBigValues(Float2 a, Float2 b)
{
    // Should be safe:
    return Float2(AddBigValues(a.X, b.X), AddBigValues(a.Y, b.Y));
}

Float2 AddBigValues(Float2 a, Float2 b, Float2 c)
{
    // Should be safe:
    return AddBigValues(AddBigValues(a, b), c);
}

Float2 AddBigValues(Float2 a, Float2 b, Float2 c, Float2 d)
{
    // Should be safe:
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}


int AddBigValues(int a, int b)
{
    // Should be safe:
    if ((a > 0) != (b > 0))
        return a + b;
    if (a < 0 && b < 0)
        return -AddBigValues(-a, -b);

    if (MAX_int32 - a < b)
        return MAX_int32;

    return a + b;
}

int AddBigValues(int a, int b, int c)
{
    return AddBigValues(AddBigValues(a, b), c);
}

int AddBigValues(int a, int b, int c, int d)
{
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}

Int2 AddBigValues(Int2 a, Int2 b)
{
    // Should be safe:
    return Int2(AddBigValues(a.X, b.X), AddBigValues(a.Y, b.Y));
}

Int2 AddBigValues(Int2 a, Int2 b, Int2 c)
{
    // Should be safe:
    return AddBigValues(AddBigValues(a, b), c);
}

Int2 AddBigValues(Int2 a, Int2 b, Int2 c, Int2 d)
{
    // Should be safe:
    return AddBigValues(AddBigValues(AddBigValues(a, b), c), d);
}


FudgetLayoutSlot::FudgetLayoutSlot(const SpawnParams &params) : Base(params), Control(nullptr), OldSizes(), UnrestrictedSizes(), Sizes(), ComputedBounds(0.f, 0.f)
{

}


FudgetLayout::FudgetLayout(const SpawnParams &params) : Base(params), _owner(nullptr),
        _layout_dirty(false), _sizes(), _unrestricted_sizes(), _flags(FudgetLayoutFlag::ResetFlags), _changing(false)
{

}

FudgetLayout::~FudgetLayout()
{
    if (_owner != nullptr)
        CleanUp();
}

void FudgetLayout::SetOwner(FudgetContainer *value)
{
    if (_changing || _owner == value)
        return;

    _changing = true;
    if (_owner != nullptr)
        _owner->SetLayout(nullptr);
    if (value != nullptr)
        value->SetLayout(this);
    else
        CleanUp();
    _changing = false;
}

bool FudgetLayout::MarkDirty(FudgetLayoutDirtyReason dirt_reason)
{
    bool size_dirty = false;
    bool container = (dirt_reason & FudgetLayoutDirtyReason::Container) == FudgetLayoutDirtyReason::Container;
    if ((dirt_reason & FudgetLayoutDirtyReason::Size) == FudgetLayoutDirtyReason::Size)
    {
        _layout_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerResize)) || (!container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContentResize));
        size_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerResize)) || (!container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentResize));
    }
    if ((dirt_reason & FudgetLayoutDirtyReason::Position) == FudgetLayoutDirtyReason::Position)
    {
        _layout_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerReposition)) || (!container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContentReposition));
        size_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerReposition)) || (!container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentReposition));
    }
    if ((dirt_reason & FudgetLayoutDirtyReason::Index) == FudgetLayoutDirtyReason::Index)
    {
        _layout_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerIndexChange)) || (!container && HasAnyFlag(FudgetLayoutFlag::LayoutOnContentIndexChange));
        size_dirty |= (container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerIndexChange)) || (!container && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentIndexChange));
    }

    if (size_dirty)
    {
        _unrestricted_sizes.IsValid = false;
        _sizes.IsValid = false;
    }

    return size_dirty;
}

void FudgetLayout::MarkControlSizesDirty(int index)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return;
    slot->Sizes.IsValid = false;
    slot->UnrestrictedSizes.IsValid = false;
}

void FudgetLayout::MarkDirtyOnLayoutUpdate(FudgetLayoutDirtyReason dirt_reason)
{
    bool size_dirty = false;
    if ((int)dirt_reason & (int)FudgetLayoutDirtyReason::Size)
    {
        _layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerResize);
        size_dirty |= HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerResize);
    }
    if ((int)dirt_reason & (int)FudgetLayoutDirtyReason::Position)
    {
        _layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerReposition);
        size_dirty |= HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerReposition);
    }

    if (size_dirty)
    {
        _unrestricted_sizes.IsValid = false;
        _sizes.IsValid = false;
    }
}

Int2 FudgetLayout::GetHintSize()
{
    return _sizes.IsValid ? _sizes.Size : _unrestricted_sizes.IsValid ? _unrestricted_sizes.Size : Int2::Zero;
}

Int2 FudgetLayout::GetMinSize()
{
    return _sizes.IsValid ? _sizes.Min : _unrestricted_sizes.IsValid ? _unrestricted_sizes.Min : Int2::Zero;
}

Int2 FudgetLayout::GetMaxSize()
{
    return _sizes.IsValid ? _sizes.Max : _unrestricted_sizes.IsValid ? _unrestricted_sizes.Max : Int2::Zero;
}

bool FudgetLayout::SizeDependsOnSpace() const
{
    return _sizes.IsValid ? _sizes.SizeFromSpace : _unrestricted_sizes.IsValid ? _unrestricted_sizes.SizeFromSpace : false;
}

void FudgetLayout::RequestLayoutChildren(bool forced)
{
    if (!_layout_dirty && !forced)
        return;

    auto owner = GetOwner();
    if (owner == nullptr)
        return;

    Int2 space = owner->LayoutSpace();
    DoLayoutChildren(space);
}

void FudgetLayout::Serialize(SerializeStream& stream, const void* otherObj)
{
    SERIALIZE_GET_OTHER_OBJ(FudgetLayout);
    stream.JKEY("TypeName");
    stream.String(GetType().Fullname);
    SERIALIZE_MEMBER(ID, GetID());

    //SERIALIZE_MEMBER(Flags, _flags);
}

void FudgetLayout::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
    DESERIALIZE_MEMBER(Flags, _flags);
}


bool FudgetLayout::GoodSlotIndex(int index) const
{
    if (_owner == nullptr)
        return false;
    return index >= 0 && index < _slots.Count();
}

void FudgetLayout::SetControlDimensions(int index, Int2 pos, Int2 size)
{
    if (_owner == nullptr)
        return;
    FudgetControl *control = _owner->ChildAt(index);
    control->LayoutUpdate(pos, size);
}

FudgetLayoutSlot* FudgetLayout::GetSlot(int index) const
{
    if (!GoodSlotIndex(index))
        return nullptr;
    return _slots[index];
}

void FudgetLayout::CleanUp()
{
    _slots.ClearDelete();
}

void FudgetLayout::FillSlots()
{
    if (_owner == nullptr)
        return;

    CleanUp();

    for (int ix = 0, siz = _owner->GetChildCount(); ix < siz; ++ix)
    {
        ChildAdded(_owner->ChildAt(ix), ix);
    }
}

void FudgetLayout::PreLayoutChildren(Int2 space, FudgetContainer *owner, int count)
{
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (slot->Control->IsHiddenInLayout())
            continue;
        slot->ComputedBounds.Size = Int2(-1);
    }
}

void FudgetLayout::LayoutChildren(Int2 space, FudgetContainer *owner, int count)
{
    SetMeasuredSizes(FudgetLayoutSizeCache(space));
    if (IsUnrestrictedSpace(space))
        return;

    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (!slot->UnrestrictedSizes.IsValid || slot->Control->IsHiddenInLayout())
            continue;
        slot->Sizes = slot->UnrestrictedSizes;
        slot->Sizes.Space = slot->UnrestrictedSizes.Size;
        slot->ComputedBounds = Rectangle(Float2(slot->Control->GetHintPosition()), Float2(slot->UnrestrictedSizes.Size));
    }
}

void FudgetLayout::DoLayoutChildren(Int2 available)
{
    auto owner = GetOwner();
    if (owner == nullptr)
    {
        _sizes.IsValid = false;
        return;
    }

    int count = owner->GetChildCount();
    bool has_visible_children = false;
    for (int ix = 0; ix < count && !has_visible_children; ++ix)
    {
        auto slot = GetSlot(ix);
        has_visible_children = !slot->Control->IsHiddenInLayout();
    }

    if (!has_visible_children)
    {
        // Nothing to do.

        bool from_space = owner->SizeDependsOnSpace();
        SetMeasuredSizes(FudgetLayoutSizeCache(available, 0, 0, 0, from_space));
        _layout_dirty = false;
        return;
    }

    // Calculates the unrestricted size of every control that is invalid. Most of the time this is enough.
    // Exceptions are controls that might resize themselves after the available space changes. For example
    // word wrapping labels.

    // Number of controls that might need to be measured multiple times, because their size depends on the
    // space in their slot.
    int size_from_space_cnt = 0;
    for (int ix = 0; ix < count; ++ix)
    {
        auto slot = GetSlot(ix);
        if (slot->Control->IsHiddenInLayout())
            continue;

        if (!slot->UnrestrictedSizes.IsValid)
        {
            slot->UnrestrictedSizes.IsValid = true;
            slot->UnrestrictedSizes.SizeFromSpace = slot->Control->OnMeasure(Int2(-1), slot->UnrestrictedSizes.Size, slot->UnrestrictedSizes.Min, slot->UnrestrictedSizes.Max);
            slot->Sizes = slot->UnrestrictedSizes;
        }
        if (slot->UnrestrictedSizes.SizeFromSpace)
            ++size_from_space_cnt;
    }

    bool unrestricted = IsUnrestrictedSpace(available);
    if (!_layout_dirty && _unrestricted_sizes.IsValid && (!_unrestricted_sizes.SizeFromSpace || unrestricted))
    {
        // The layout doesn't require calculations if it already has valid measurements. In this case it is either
        // given unlimited space, or no control's measurement depends on the available space.
        return;
    }
    if (!_layout_dirty && _sizes.IsValid && available == _sizes.Space)
    {
        // The layout doesn't require calculations if it already has valid measurements. In this case it was previously
        // laid out in the same available space.
        return;
    }

    // Layout and measure until the sizes are determined

    // Allow the layout to initialize its values and clean up calculated slots
    PreLayoutChildren(available, owner, count);

    // Keeps track if the layout has controls that need to be measured multiple times for different slot sizes.
    bool need_remeasure = false;
    // Measurements saved for such controls. If this changes after the layout, they will be measured again.
    Array<FudgetLayoutSizeCache> compare_cache(size_from_space_cnt);
    compare_cache.AddUninitialized(size_from_space_cnt);

    // Limits the number of iterations in case a control keeps changing its size during measurements.
    // TODO: make the maximum iterations customizable
    const int max_iterations = 3;
    int iteration = 0;

    do
    {
        // Save measurements for size changing controls
        for (int ix = 0, pos = 0; !unrestricted && ix < count && pos < size_from_space_cnt; ++ix)
        {
            auto slot = GetSlot(ix);
            if (!slot->UnrestrictedSizes.SizeFromSpace || slot->Control->IsHiddenInLayout())
                continue;
            compare_cache[pos++] = slot->Sizes;
        }
        // Do the layout
        LayoutChildren(available, owner, count);
        // Check if the saved measurements for size changing controls were changed.
        if (!unrestricted && size_from_space_cnt > 0)
        {
            for (int ix = 0, pos = 0; ix < count && pos < size_from_space_cnt && !need_remeasure; ++ix)
            {
                auto slot = GetSlot(ix);
                if (!slot->UnrestrictedSizes.SizeFromSpace || slot->Control->IsHiddenInLayout())
                    continue;
                FudgetLayoutSizeCache cached = compare_cache[pos++];
                need_remeasure = !slot->Sizes.IsValid || cached.Size != slot->Sizes.Size || cached.Min != slot->Sizes.Min || cached.Max != slot->Sizes.Max;
            }
        }
    } while (need_remeasure && (++iteration < max_iterations));

    if (!unrestricted)
    {
        for (int ix = 0; ix < count; ++ix)
            if (!GetSlot(ix)->Control->IsHiddenInLayout())
                PlaceControlInSlotRectangle(ix);

        _layout_dirty = !_sizes.IsValid && !_unrestricted_sizes.IsValid;
        if (_layout_dirty)
            LOG(Warning, "Layout measurements were invalid or were not set with SetMeasuredSizes.");
    }
}

bool FudgetLayout::MeasureSlot(int index, Int2 available, API_PARAM(Out) Int2 &wanted_size, API_PARAM(Out) Int2 &wanted_min, API_PARAM(Out) Int2 &wanted_max)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return false;

    if (slot->UnrestrictedSizes.IsValid && (!slot->UnrestrictedSizes.SizeFromSpace || IsUnrestrictedSpace(available)))
    {
        wanted_size = slot->UnrestrictedSizes.Size;
        wanted_min = slot->UnrestrictedSizes.Min;
        wanted_max = slot->UnrestrictedSizes.Max;
        return slot->UnrestrictedSizes.SizeFromSpace;
    }
    else if (slot->Sizes.IsValid && available == slot->Sizes.Space)
    {
        wanted_size = slot->Sizes.Size;
        wanted_min = slot->Sizes.Min;
        wanted_max = slot->Sizes.Max;
        return slot->Sizes.SizeFromSpace;
    }

    Int2 size;
    Int2 min;
    Int2 max;
    bool from_space = slot->Control->OnMeasure(available, size, min, max);

    if (IsUnrestrictedSpace(available))
    {
        slot->UnrestrictedSizes.IsValid = true;
        slot->UnrestrictedSizes.Space = available;
        wanted_min = slot->UnrestrictedSizes.Min = min;
        wanted_max = slot->UnrestrictedSizes.Max = Int2::Max(min, max);
        wanted_size = slot->UnrestrictedSizes.Size = Int2::Clamp(size, wanted_min, wanted_max);
        slot->UnrestrictedSizes.SizeFromSpace = from_space;

        if (!from_space)
            slot->Sizes = slot->UnrestrictedSizes;
    }
    else
    {
        slot->Sizes.IsValid = true;
        slot->Sizes.Space = available;
        wanted_min = slot->Sizes.Min = min;
        wanted_max = slot->Sizes.Max = Int2::Max(min, max);
        wanted_size = slot->Sizes.Size = Int2::Clamp(size, wanted_min, wanted_max);
        slot->Sizes.SizeFromSpace = from_space;
    }

    return from_space;
}

bool FudgetLayout::SlotSizeFromSpace(int index)
{
    auto slot = GetSlot(index);
    if (slot == nullptr)
        return false;

    return slot->UnrestrictedSizes.SizeFromSpace;
}

void FudgetLayout::SetMeasuredSizes(const FudgetLayoutSizeCache &sizes)
{
    if (IsUnrestrictedSpace(sizes.Space) || !sizes.SizeFromSpace)
    {
        _unrestricted_sizes.IsValid = true;
        _unrestricted_sizes.Space = -1;
        _unrestricted_sizes.Size = sizes.Size;
        _unrestricted_sizes.Min = sizes.Min;
        _unrestricted_sizes.Max = sizes.Max;

        if (!sizes.SizeFromSpace)
            _sizes = _unrestricted_sizes;
    }
    else
    {
        _sizes.IsValid = true;
        _sizes.Space  = sizes.Space;
        _sizes.Size = sizes.Size;
        _sizes.Min = sizes.Min;
        _sizes.Max = sizes.Max;
    }
    _unrestricted_sizes.SizeFromSpace = sizes.SizeFromSpace;
    _sizes.SizeFromSpace = sizes.SizeFromSpace;
}

void FudgetLayout::PlaceControlInSlotRectangle(int index)
{
    auto slot = GetSlot(index);
    SetControlDimensions(index, slot->ComputedBounds.Location, Int2::Clamp(slot->ComputedBounds.Size, slot->Sizes.Min, slot->Sizes.Max));
}

FudgetLayoutSlot* FudgetLayout::CreateSlot(FudgetControl *control)
{
    FudgetLayoutSlot *slot = New<FudgetLayoutSlot>(SpawnParams(Guid::New(), FudgetLayoutSlot::TypeInitializer));
    slot->Control = control;
    return slot;
}

void FudgetLayout::ChildAdded(FudgetControl *control, int index)
{
    auto slot = CreateSlot(control);
    if (index == -1)
    {
        _slots.Add(slot);
        index = _slots.Count() - 1;
    }
    else
        _slots.Insert(index, slot);
    
    if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentChange))
        _owner->MarkLayoutDirty(FudgetLayoutDirtyReason::Size);

    FudgetContainer *content = dynamic_cast<FudgetContainer*>(_slots[index]->Control);
    if (content != nullptr)
        content->MarkLayoutDirty(FudgetLayoutDirtyReason::All | FudgetLayoutDirtyReason::Container);

    _layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentChange);
}

void FudgetLayout::ChildRemoved(int index)
{
    Delete(_slots[index]);
    _slots.RemoveAtKeepOrder(index);

    if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentChange))
        _owner->MarkLayoutDirty(FudgetLayoutDirtyReason::Size);
    if (_slots.Count() != 0)
        _layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentChange);
    else
        _layout_dirty = false;
}

void FudgetLayout::ChildMoved(int from, int to)
{
    if (from == to || from < 0 || to < 0 || from >= _slots.Count() || to >= _slots.Count())
        return;

    MoveInArray(_slots, from, to);

    if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentIndexChange))
        _owner->MarkLayoutDirty(FudgetLayoutDirtyReason::Index);

    for (int ix = Math::Min(from, to), siz = Math::Max(from, to) + 1; ix < siz; ++ix)
    {
        FudgetContainer *content = dynamic_cast<FudgetContainer*>(_slots[ix]->Control);
        if (content != nullptr)
            content->MarkLayoutDirty(FudgetLayoutDirtyReason::Index | FudgetLayoutDirtyReason::Container);
    }

    _layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentIndexChange);
}

void FudgetLayout::AllDeleted()
{
    if (_slots.Count() == 0)
        return;

    for (int ix = _slots.Count() - 1; ix >= 0; --ix)
        Delete(_slots[ix]);
    _slots.Clear();
    
    if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentChange))
        _owner->MarkLayoutDirty(FudgetLayoutDirtyReason::Size);
    _layout_dirty = false;
    //_size_dirty = true;
}

void FudgetLayout::MarkOwnerDirty()
{
    if (_owner == nullptr)
        return;
    _owner->MarkLayoutDirty(FudgetLayoutDirtyReason::Container | FudgetLayoutDirtyReason::Size);
}

FudgetLayoutFlag FudgetLayout::GetLayoutFlags() const
{
    return _flags;
}

void FudgetLayout::SetLayoutFlags(FudgetLayoutFlag flags)
{
    _flags = flags;
}

bool FudgetLayout::HasAllFlags(FudgetLayoutFlag flags) const
{
    return (flags & _flags) == flags;
}

bool FudgetLayout::HasAnyFlag(FudgetLayoutFlag flags) const
{
    return ((int)flags & (int)_flags) != 0;
}

//void FudgetLayout::RecalculateSizes(bool forced)
//{
//    if (_size_dirty || forced)
//    {
//        _cached_hint = RequestSize(FudgetSizeType::Hint);
//        _cached_min = RequestSize(FudgetSizeType::Min);
//        _cached_max = RequestSize(FudgetSizeType::Max);
//        _size_dirty = false;
//    }
//}

void FudgetLayout::SetOwnerInternal(FudgetContainer *value)
{
    if (_owner == value)
        return;

    _owner = value;
    _layout_dirty = true;
    //_size_dirty = true;
    _unrestricted_sizes.IsValid = false;
    _sizes.IsValid = false;

    if (value != nullptr)
    {
        if ((_flags & FudgetLayoutFlag::ResetFlags) == FudgetLayoutFlag::ResetFlags)
            _flags = GetInitFlags();
        FillSlots();
    }
    else
        CleanUp();
}
