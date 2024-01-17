#include "Layout.h"
#include "../Container.h"


FudgetLayoutSlot::FudgetLayoutSlot(FudgetControl *control) : Base(SpawnParams(Guid::New(), TypeInitializer)), _control(control), _pref_size(0.f), _min_size(0.f), _max_size(0.f)
{

}


FudgetLayout::FudgetLayout() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), _dirty(false), _owner(nullptr),
	_preferred_dirty(true), _min_dirty(true), _max_dirty(true), _cached_preferred(0.f), _cached_min(0.f), _cached_max(0.f)
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

void FudgetLayout::MakeDirty()
{
	_dirty = true;
	_preferred_dirty = true;
	_min_dirty = true;
	_max_dirty = true;
}

void FudgetLayout::ChildAdded(FudgetControl *control)
{
	_dirty = true;
	_slots.Add(CreateSlot(control));
}

void FudgetLayout::ChildRemoved(int at)
{
	_dirty = true;
	Delete(_slots[at]);
	_slots.RemoveAt(at);
}

Float2 FudgetLayout::GetPreferredSize() const
{
	if (!_preferred_dirty && !Float2::NearEqual(_cached_preferred, Float2(-1.0f)))
		return _cached_preferred;
	_preferred_dirty = false;
	_cached_preferred = GetRequestedSize(FudgetSizeType::Preferred);
	return _cached_preferred;
}

Float2 FudgetLayout::GetMinSize() const
{
	if (!_min_dirty && !Float2::NearEqual(_cached_min, Float2(-1.0f)))
		return _cached_min;
	_min_dirty = false;
	_cached_min = GetRequestedSize(FudgetSizeType::Min);
	return _cached_min;
}

Float2 FudgetLayout::GetMaxSize() const
{
	if (!_max_dirty && !Float2::NearEqual(_cached_max, Float2(-1.0f)))
		return _cached_max;
	_max_dirty = false;
	_cached_max = GetRequestedSize(FudgetSizeType::Max);
	return _cached_max;
}

void FudgetLayout::SetControlDimensions(int at, Float2 pos, Float2 size)
{
	if (_owner == nullptr)
		return;
	_owner->ChildAt(at)->LayoutUpdate(pos, size);
}

FudgetLayoutSlot* FudgetLayout::GetSlot(int at) const
{
	if (at < 0 || at >= _slots.Count())
		return nullptr;
	return _slots[at];
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
		ChildAdded(_owner->ChildAt(ix));
	}
}

void FudgetLayout::ClearedDirt()
{
	_dirty = false;
}
