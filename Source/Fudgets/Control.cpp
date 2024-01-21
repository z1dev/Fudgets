#include "Control.h"
#include "Container.h"
#include "GUIRoot.h"

#include "Engine/Render2D/Render2D.h"
#include "Engine/Core/Math/Rectangle.h"


FudgetGUIRoot* FudgetControl::_guiRoot = nullptr;


FudgetControl::FudgetControl(const SpawnParams &params) : FudgetControl(params, FudgetControlFlags::BlockMouseEvents)
{
}

FudgetControl::FudgetControl(const SpawnParams &params, FudgetControlFlags flags) : ScriptingObject(params),
	_parent(nullptr), _index(-1), _flags(flags), _pos(0.f), _size(0.0f), _hint_size(120.f, 60.0f), _min_size(30.f, 30.f),
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

void FudgetControl::SetName(String value)
{
	// TODO: event on name change?
	_name = value;
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
	value += GetPosition();

	while (parent != nullptr)
	{
		value += parent->GetPosition();
		parent = parent->_parent;
	}

	return value;
}

Float2 FudgetControl::GlobalToLocal(Float2 value) const
{
	FudgetContainer *parent = _parent;
	value -= GetPosition();

	while (parent != nullptr)
	{
		value -= parent->GetPosition();
		parent = parent->_parent;
	}

	return value;
}

FudgetControlFlags FudgetControl::GetControlFlags() const
{
	return _flags;
}

void FudgetControl::SetControlFlags(FudgetControlFlags flags)
{
	_flags = flags;

	// Might not need recalculation but we can't be sure.
	SizeOrPosModified(FudgetDirtType::All);
}

bool FudgetControl::HasAllFlags(FudgetControlFlags flags) const
{
	return (flags & _flags) == flags;
}

bool FudgetControl::HasAnyFlag(FudgetControlFlags flags) const
{
	return (int)(flags & _flags) != 0;
}

void FudgetControl::FillRectangle(Float2 pos, Float2 size, Color color) const
{
	pos = LocalToGlobal(pos);

	Render2D::FillRectangle(Rectangle(pos, size), color);
}

void FudgetControl::DrawRectangle(Float2 pos, Float2 size, Color color, float thickness) const
{
	pos = LocalToGlobal(pos);

	Render2D::DrawRectangle(Rectangle(pos, size), color, thickness);
}

void FudgetControl::SizeOrPosModified(FudgetDirtType dirt_flags)
{
	if (_parent != nullptr)
		_parent->MarkLayoutDirty(dirt_flags, true);
}

void FudgetControl::CaptureMouseInput()
{
	auto root = GetGUIRoot();
	if (root == nullptr)
		return;

	root->StartMouseCapture(this);
}

void FudgetControl::ReleaseMouseInput()
{
	auto root = GetGUIRoot();
	if (root == nullptr || root->GetMouseCaptureControl() != this)
		return;

	root->ReleaseMouseCapture();
}

bool FudgetControl::GetFocused() const
{
	auto root = GetGUIRoot();
	if (root == nullptr)
		return false;
	return root->GetFocusedControl() == this;
}

void FudgetControl::SetFocused(bool value)
{
	auto root = GetGUIRoot();
	if (root == nullptr)
		return;
	if (value)
		root->SetFocusedControl(this);
	if (!value && root->GetFocusedControl() == this)
		root->SetFocusedControl(nullptr);
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
