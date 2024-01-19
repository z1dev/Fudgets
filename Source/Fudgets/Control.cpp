#include "Control.h"
#include "Container.h"

#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"

FudgetControl::FudgetControl(const SpawnParams &params) : ScriptingObject(params),
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
	if (_changing || _parent == nullptr || value == _index || value < 0 || value >= _parent->GetChildCount())
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
	SizeOrPosModified(FudgetDirtType::Size);
}

void FudgetControl::SetMinSize(Float2 value)
{
	if (Float2::NearEqual(_min_size, value))
		return;
	_min_size = value;
	SizeOrPosModified(FudgetDirtType::Size);
}

void FudgetControl::SetMaxSize(Float2 value)
{
	if (Float2::NearEqual(_max_size, value))
		return;
	_max_size = value;
	SizeOrPosModified(FudgetDirtType::Size);
}

Float2 FudgetControl::GetSize() const
{
	if (_parent == nullptr)
		return _hint_size;
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
		default:
			return Float2(0.f);
	}
}

void FudgetControl::SetPosition(Float2 value)
{
	if (Float2::NearEqual(_pos, value) || !IsPositionChangePermitted())
		return;

	_pos = value;
	SizeOrPosModified(FudgetDirtType::Position);
}


Float2 FudgetControl::LocalToGlobal(Float2 value) const
{
	FudgetContainer *parent = _parent;
	value += _pos;

	while (parent != nullptr)
	{
		value += parent->_pos;
		parent = parent->_parent;
	}

	return value;
}


Float2 FudgetControl::GlobalTolocal(Float2 value) const
{
	FudgetContainer *parent = _parent;
	value -= _pos;

	while (parent != nullptr)
	{
		value -= parent->_pos;
		parent = parent->_parent;
	}

	return value;
}


void FudgetControl::FillRectangle(Float2 pos, Float2 size, Color color) const
{

	pos = LocalToGlobal(pos);

	Render2D::FillRectangle(Rectangle(pos, size), color);
}


void FudgetControl::SizeOrPosModified(FudgetDirtType dirt_flags)
{
	if (_parent != nullptr)
		_parent->MarkLayoutDirty(dirt_flags, true);
}

bool FudgetControl::IsPositionChangePermitted() const
{
	if (!_parent)
		return true;
	return _parent->IsControlPositioningPermitted(this);
}

void FudgetControl::LayoutUpdate(Float2 pos, Float2 size)
{
	_pos = pos; 
	_size = size;
}
