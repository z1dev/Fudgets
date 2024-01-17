#include "Control.h"

#include "Container.h"

FudgetControl::FudgetControl() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)),
	_parent(nullptr), _index(-1), _pos(0.f), _size(0.0f), _hint_size(120.f, 60.0f), _min_size(30.f, 30.f),
	_max_size(-1.f, -1.f), _changing(false)
{

}

void FudgetControl::SetParent(FudgetContainer *value)
{
	SetParent(value, -1);
}

void FudgetControl::SetParent(FudgetContainer *value, int order)
{
	if (_parent == value || _changing)
		return;

	_changing = true;
	if (_parent != nullptr)
	{
		_parent->RemoveChild(_index);
	}
	_parent = value;
	_index = -1;
	if (_parent != nullptr)
	{
		if (order >= _parent->GetChildCount())
			order = -1;

		_index = _parent->AddChild(this, order);
	}
	_changing = false;
}

void FudgetControl::SetIndexInParent(int value)
{
	if (_changing || _parent == nullptr || value < 0 || value >= _parent->GetChildCount() || value == _index)
		return;
	_changing = true;
	if (_parent->MoveChildToIndex(_index, value))
		_index = value;
	_changing = false;
}

void FudgetControl::SetHintSize(Float2 value)
{
	if (Float2::NearEqual(_hint_size, value))
		return;
	_hint_size = value;
	MakeParentLayoutDirty(FudgetSizeType::Hint);
}

void FudgetControl::SetMinSize(Float2 value)
{
	if (Float2::NearEqual(_min_size, value))
		return;
	_min_size = value;
	MakeParentLayoutDirty(FudgetSizeType::Min);
}

void FudgetControl::SetMaxSize(Float2 value)
{
	if (Float2::NearEqual(_max_size, value))
		return;
	_max_size = value;
	MakeParentLayoutDirty(FudgetSizeType::Max);
}

Float2 FudgetControl::GetSize() const
{
	if (_parent != nullptr)
		_parent->RequestLayout();
	return _size;
}

Float2 FudgetControl::GetPosition() const
{
	if (_parent != nullptr)
		_parent->RequestLayout();
	return _pos;
}

Float2 FudgetControl::GetRequestedSize(FudgetSizeType type) const
{
	switch (type)
	{
		case FudgetSizeType::Hint:
			return GetHintSize();
		case FudgetSizeType::Min:
			return GetMinSize();
		case FudgetSizeType::Max:
			return GetMaxSize();
		case FudgetSizeType::Current:
			return GetSize();
		default:
			return Float2(0.f);
	}
}

void FudgetControl::SetPosition(Float2 value)
{
	if (Float2::NearEqual(_pos, value))
		return;

	_pos = value;
	MakeParentLayoutDirty(FudgetSizeType::None);
}

void FudgetControl::OnDeleteObject()
{
	if (IsRegistered())
		UnregisterObject();

	Base::OnDeleteObject();
}

void FudgetControl::MakeParentLayoutDirty(FudgetSizeType sizeType)
{
	if (_parent != nullptr)
		_parent->MakeLayoutDirty(sizeType);
}

void FudgetControl::LayoutUpdate(Float2 pos, Float2 size)
{
	_pos = pos; 
	_size = size;
}
