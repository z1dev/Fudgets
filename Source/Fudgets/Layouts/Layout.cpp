#include "Layout.h"
#include "../Container.h"
#include "../Utils/Utils.h"

// These could be in any cpp file but it's most likely to be used in layouts so whatever. Declared as extern in Utils.h
const float MaximumFloatLimit = (std::numeric_limits<float>::max() - 1.0f);

float AddBigFloats(float a, float b)
{
	// Should be safe:
	if ((a > 0) != (b > 0))
		return a + b;
	if (a < 0 && b < 0)
		return -AddBigFloats(-a, -b);

	if (MaximumFloatLimit - a < b)
		return MaximumFloatLimit;

	return a + b;
}


FudgetLayoutSlot::FudgetLayoutSlot(FudgetControl *control) : Base(SpawnParams(Guid::New(), TypeInitializer)), _control(control), _hint_size(0.f), _min_size(0.f), _max_size(0.f)
{

}


FudgetLayout::FudgetLayout() : FudgetLayout(SpawnParams(Guid::New(), TypeInitializer), FudgetLayoutFlag::None)
{ 

}

FudgetLayout::FudgetLayout(FudgetLayoutFlag flags) : FudgetLayout(SpawnParams(Guid::New(), TypeInitializer), _flags)
{

}

FudgetLayout::FudgetLayout(const SpawnParams &params) : FudgetLayout(params, FudgetLayoutFlag::None)
{

}

FudgetLayout::FudgetLayout(const SpawnParams &params, FudgetLayoutFlag flags) : ScriptingObject(params), _owner(nullptr),
		_layout_dirty(false), _size_dirty(false), _cached_hint(0.f), _cached_min(0.f), _cached_max(0.f), _flags(flags)
{

}

FudgetLayout::~FudgetLayout()
{
	if (_owner != nullptr)
		CleanUp();
}

void FudgetLayout::SetOwner(FudgetContainer *value)
{
	if (_owner == value)
		return;

	_owner = value;
	_layout_dirty = true;
	_size_dirty = true;

	if (value != nullptr)
		FillSlots();
}

void FudgetLayout::MarkDirty(FudgetDirtType dirt_flags, bool content_changed)
{
	if ((int)dirt_flags & (int)FudgetDirtType::Size)
	{
		_layout_dirty |= (!content_changed && HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerResize)) || (content_changed && HasAnyFlag(FudgetLayoutFlag::LayoutOnContentResize));
		_size_dirty |= (!content_changed && HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerResize)) || (content_changed && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentResize));
	}
	if ((int)dirt_flags & (int)FudgetDirtType::Position)
	{
		_layout_dirty |= (!content_changed && HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerReposition)) || (content_changed && HasAnyFlag(FudgetLayoutFlag::LayoutOnContentReposition));
		_size_dirty |= (!content_changed && HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerReposition)) || (content_changed && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentReposition));
	}
}

void FudgetLayout::MarkDirtyOnLayoutUpdate(FudgetDirtType dirt_flags)
{
	if ((int)dirt_flags & (int)FudgetDirtType::Size)
	{
		_layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerResize);
		_size_dirty |= HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerResize);
	}
	if ((int)dirt_flags & (int)FudgetDirtType::Position)
	{
		_layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContainerReposition);
		_size_dirty |= HasAnyFlag(FudgetLayoutFlag::ResizeOnContainerReposition);
	}
}

void FudgetLayout::ChildAdded(FudgetControl *control, int index)
{
	auto slot = CreateSlot(control);
	if (index == -1)
		_slots.Add(slot);
	else
		_slots.Insert(index, slot);
	
	if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::ResizeOnContentReposition))
		_owner->MarkLayoutDirty(FudgetDirtType::Size, true);
	_layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::LayoutOnContentReposition);
}

void FudgetLayout::ChildRemoved(int index)
{
	Delete(_slots[index]);
	_slots.RemoveAtKeepOrder(index);

	if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::ResizeOnContentReposition))
		_owner->MarkLayoutDirty(FudgetDirtType::Size, true);
	_layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentResize | FudgetLayoutFlag::LayoutOnContentReposition);
}

void FudgetLayout::ChildMoved(int from, int to)
{
	if (from == to || from < 0 || to < 0 || from >= _slots.Count() || to >= _slots.Count())
		return;

	MoveInArray(_slots, from, to);

	if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentIndexChange))
		_owner->MarkLayoutDirty(FudgetDirtType::Size, true);
	_layout_dirty |= HasAnyFlag(FudgetLayoutFlag::LayoutOnContentIndexChange);
}

void FudgetLayout::AllDeleted()
{
	if (_slots.Count() == 0)
		return;

	for (int ix = _slots.Count() - 1; ix >= 0; --ix)
		Delete(_slots[ix]);
	_slots.Clear();
	
	if (_owner != nullptr && HasAnyFlag(FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::ResizeOnContentReposition))
		_owner->MarkLayoutDirty(FudgetDirtType::Size, true);
	_layout_dirty = false;
	_size_dirty = true;
}

bool FudgetLayout::IsControlPositioningPermitted(const FudgetControl *control) const
{
	return false;
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
	return ((int)flags & (int)_flags) == (int)flags;
}

bool FudgetLayout::HasAnyFlag(FudgetLayoutFlag flags) const
{
	return ((int)flags & (int)_flags) != 0;
}

Float2 FudgetLayout::GetHintSize() const
{
	if (!_size_dirty && !Float2::NearEqual(_cached_hint, Float2(-1.0f)))
		return _cached_hint;
	_cached_hint = GetRequestedSize(FudgetSizeType::Hint);
	return _cached_hint;
}

Float2 FudgetLayout::GetMinSize() const
{
	if (!_size_dirty && !Float2::NearEqual(_cached_min, Float2(-1.0f)))
		return _cached_min;
	_cached_min = GetRequestedSize(FudgetSizeType::Min);
	return _cached_min;
}

Float2 FudgetLayout::GetMaxSize() const
{
	if (!_size_dirty && !Float2::NearEqual(_cached_max, Float2(-1.0f)))
		return _cached_max;
	_cached_max = GetRequestedSize(FudgetSizeType::Max);
	return _cached_max;
}

void FudgetLayout::RequestLayoutChildren(bool forced)
{
	if (!_layout_dirty && !forced)
		return;
	if (LayoutChildren())
		_layout_dirty = false;
}

bool FudgetLayout::GoodSlotIndex(int index) const
{
	if (_owner == nullptr)
		return false;
	return index >= 0 && index < _slots.Count();
}

void FudgetLayout::SetControlDimensions(int index, Float2 pos, Float2 size)
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
	for (auto s : _slots)
		Delete(s);
	_slots.Clear();
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

Float2 FudgetLayout::GetRequestedSize(FudgetSizeType type) const
{
	if (_size_dirty)
	{
		_cached_hint = RequestSize(FudgetSizeType::Hint);
		_cached_min = RequestSize(FudgetSizeType::Min);
		_cached_max = RequestSize(FudgetSizeType::Max);
		_size_dirty = false;
	}
	switch (type)
	{
		case FudgetSizeType::Hint:
			return _cached_hint;
		case FudgetSizeType::Min:
			return _cached_min;
		case FudgetSizeType::Max:
			return _cached_max;
		default:
			return Float2(0.f);
	}
}
