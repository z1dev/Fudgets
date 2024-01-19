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


FudgetLayout::FudgetLayout() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), _dirty(false), _owner(nullptr),
	_hint_dirty(true), _min_dirty(true), _max_dirty(true), _cached_hint(0.f), _cached_min(0.f), _cached_max(0.f)
{ 

}

FudgetLayout::FudgetLayout(const SpawnParams &params) : ScriptingObject(params), _dirty(false), _owner(nullptr),
	_hint_dirty(true), _min_dirty(true), _max_dirty(true), _cached_hint(0.f), _cached_min(0.f), _cached_max(0.f)
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
	_dirty = true;

	if (value != nullptr)
		FillSlots();
}

void FudgetLayout::MakeDirty(FudgetDirtType dirt_flags)
{
	_dirty = true;
	if ((int)dirt_flags & (int)FudgetSizeType::Hint)
		_hint_dirty = true;
	if ((int)dirt_flags & (int)FudgetSizeType::Min)
		_min_dirty = true;
	if ((int)dirt_flags & (int)FudgetSizeType::Max)
		_max_dirty = true;

	if (_owner != nullptr)
		_owner->MakeParentLayoutDirty(dirt_flags);
}

void FudgetLayout::ChildAdded(FudgetControl *control, int index)
{
	MakeDirty(FudgetDirtType::All);
	auto slot = CreateSlot(control);
	if (index == -1)
		_slots.Add(slot);
	else
		_slots.Insert(index, slot);
}

void FudgetLayout::ChildRemoved(int index)
{
	MakeDirty(FudgetDirtType::All);
	Delete(_slots[index]);
	_slots.RemoveAtKeepOrder(index);
}

void FudgetLayout::ChildMoved(int from, int to)
{
	if (from == to || from < 0 || to < 0 || from >= _slots.Count() || to >= _slots.Count())
		return;

	MakeDirty(FudgetDirtType::All);
	MoveInArray(_slots, from, to);
}

void FudgetLayout::AllDeleted()
{
	if (_slots.Count() == 0)
		return;
	
}

Float2 FudgetLayout::GetHintSize() const
{
	if (!_hint_dirty && !Float2::NearEqual(_cached_hint, Float2(-1.0f)))
		return _cached_hint;
	_cached_hint = GetRequestedSize(FudgetSizeType::Hint);
	return _cached_hint;
}

Float2 FudgetLayout::GetMinSize() const
{
	if (!_min_dirty && !Float2::NearEqual(_cached_min, Float2(-1.0f)))
		return _cached_min;
	_cached_min = GetRequestedSize(FudgetSizeType::Min);
	return _cached_min;
}

Float2 FudgetLayout::GetMaxSize() const
{
	if (!_max_dirty && !Float2::NearEqual(_cached_max, Float2(-1.0f)))
		return _cached_max;
	_cached_max = GetRequestedSize(FudgetSizeType::Max);
	return _cached_max;
}

void FudgetLayout::RequestLayoutChildren(bool forced)
{
	if (!_dirty && !forced)
		return;
	if (LayoutChildren())
		_dirty = false;
}

void FudgetLayout::SetControlDimensions(int index, Float2 pos, Float2 size)
{
	if (_owner == nullptr)
		return;
	_owner->ChildAt(index)->LayoutUpdate(pos, size);
}

FudgetLayoutSlot* FudgetLayout::GetSlot(int index) const
{
	if (index < 0 || index >= _slots.Count())
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

void FudgetLayout::UnmarkLayoutDirty()
{
	_dirty = false;
}

Float2 FudgetLayout::GetRequestedSize(FudgetSizeType type) const
{
	switch (type)
	{
		case FudgetSizeType::Hint:
			if (_hint_dirty)
			{
				_cached_hint = RequestSize(type);
				_hint_dirty = false;
			}
			return _cached_hint;
		case FudgetSizeType::Min:
			if (_min_dirty)
			{
				_cached_min = RequestSize(type);
				_min_dirty = false;
			}
			return _cached_min;
		case FudgetSizeType::Max:
			if (_max_dirty)
			{
				_cached_max = RequestSize(type);
				_max_dirty = false;
			}
			return _cached_max;
		default:
			return Float2(0.f);
	}
}
