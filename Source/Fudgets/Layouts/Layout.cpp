#include "Layout.h"
#include "../Container.h"
#include "../Utils/Utils.h"


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

void FudgetLayout::MakeDirty(FudgetSizeType sizeType)
{
	_dirty = true;
	if (sizeType == FudgetSizeType::Hint || sizeType == FudgetSizeType::All)
		_hint_dirty = true;
	if (sizeType == FudgetSizeType::Min || sizeType == FudgetSizeType::All)
		_min_dirty = true;
	if (sizeType == FudgetSizeType::Max || sizeType == FudgetSizeType::All)
		_max_dirty = true;
}

void FudgetLayout::ChildAdded(FudgetControl *control, int index)
{
	MakeDirty(FudgetSizeType::All);
	auto slot = CreateSlot(control);
	if (index == -1)
		_slots.Add(slot);
	else
		_slots.Insert(index, slot);
}

void FudgetLayout::ChildRemoved(int index)
{
	MakeDirty(FudgetSizeType::All);
	Delete(_slots[index]);
	_slots.RemoveAtKeepOrder(index);
}

void FudgetLayout::ChildMoved(int from, int to)
{
	if (from == to || from < 0 || to < 0 || from >= _slots.Count() || to >= _slots.Count())
		return;

	MakeDirty(FudgetSizeType::All);
	MoveInArray(_slots, from, to);
}

Float2 FudgetLayout::GetHintSize() const
{
	if (!_hint_dirty && !Float2::NearEqual(_cached_hint, Float2(-1.0f)))
		return _cached_hint;
	_hint_dirty = false;
	_cached_hint = GetRequestedSize(FudgetSizeType::Hint);
	return _cached_hint;
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

void FudgetLayout::OnDeleteObject()
{
	if (IsRegistered())
		UnregisterObject();

	Base::OnDeleteObject();
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

void FudgetLayout::ClearedDirt()
{
	_dirty = false;
}
