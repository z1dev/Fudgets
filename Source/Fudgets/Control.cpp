#include "Control.h"

#include "Container.h"

FudgetControl::FudgetControl() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)),
	_parent(nullptr), _order(-1), _pos(0.f), _size(0.0f), _pref_size(120.f, 60.0f), _min_size(30.f, 30.f), _max_size(-1.f, -1.f)
{

}

void FudgetControl::SetParent(FudgetContainer *value)
{
	if (_parent == value)
		return;
	if (_parent != nullptr)
	{
		_parent->RemoveChild(_order);
	}
	_parent = value;
	_order = -1;
	if (_parent != nullptr)
	{
		_order = _parent->AddChild(this);
	}
}

void FudgetControl::SetPreferredSize(Float2 value)
{
	if (Float2::NearEqual(_pref_size, value))
		return;
	_pref_size = value;
	MakeLayoutDirty();
}

void FudgetControl::SetMinSize(Float2 value)
{
	if (Float2::NearEqual(_min_size, value))
		return;
	_min_size = value;
	MakeLayoutDirty();
}

void FudgetControl::SetMaxSize(Float2 value)
{
	if (Float2::NearEqual(_max_size, value))
		return;
	_max_size = value;
	MakeLayoutDirty();
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

void FudgetControl::SetSize(Float2 value)
{
	if (Float2::NearEqual(_size, value))
		return;

	_size = value;
	if (_parent != nullptr)
		_parent->MakeLayoutDirty();
}

Float2 FudgetControl::GetRequestedSize(FudgetSizeType type) const
{
	switch (type)
	{
		case FudgetSizeType::Preferred:
			return GetPreferredSize();
		case FudgetSizeType::Min:
			return GetMinSize();
		case FudgetSizeType::Max:
			return GetMaxSize();
		default:
			return GetSize();
	}
}

void FudgetControl::SetPosition(Float2 value)
{
	if (Float2::NearEqual(_pos, value))
		return;

	_pos = value;
	MakeLayoutDirty();
}

void FudgetControl::MakeLayoutDirty()
{
	if (_parent != nullptr)
		_parent->MakeLayoutDirty();
}

void FudgetControl::LayoutUpdate(Float2 pos, Float2 size)
{
	_pos = pos; 
	_size = size;
}
