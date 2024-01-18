#include "Container.h"
#include "Layouts/Layout.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Utils/Utils.h"


FudgetContainer::FudgetContainer(const SpawnParams &params) : Base(params),
_layout(nullptr), _changing(false), _height_from_layout(true), _width_from_layout(true), _root(false)
{

}

FudgetContainer::FudgetContainer(Fudget *_ui_owner) : Base(SpawnParams(Guid::New(), TypeInitializer)),
	_layout(nullptr), _changing(false), _height_from_layout(true), _width_from_layout(true), _root(_ui_owner != nullptr)
{
}

FudgetContainer::~FudgetContainer()
{
	DeleteAll();
	if (_layout != nullptr)
		Delete(_layout);
}

int FudgetContainer::AddChild(FudgetControl *control, int index)
{
	if (control == nullptr)
		return -1;

	if (_changing)
	{
		if (control->GetParent() == this)
			return ChildIndex(control);
		return -1;
	}

	if (control->GetParent() == this)
	{
		int result = control->GetIndexInParent();
		if (result == -1)
			result = ChildIndex(control);
		if (result != -1)
			return result;
	}

	_changing = true;

	if (index >= 0 && index < _children.Count())
	{
		_children.Insert(index, control);
		for (int ix = index + 1, siz = _children.Count(); ix < siz; ++ix)
			_children[ix]->SetIndexInParent(ix);
	}
	else
	{
		index = _children.Count();
		_children.Add(control);
	}

	control->SetParent(this, index);

	if (_layout != nullptr)
	{
		_layout->ChildAdded(control, index);
		// TODO: remove these where the layout calls it
		MakeLayoutDirty(FudgetSizeType::All);
	}

	_changing = false;

	return index;
}

int FudgetContainer::RemoveChild(FudgetControl *control)
{
	if (control == nullptr || control->GetParent() != this || _changing)
		return -1;

	_changing = true;

	int index = control->GetIndexInParent();
	_children.RemoveAtKeepOrder(index);
	control->SetParent(nullptr);

	for (int ix = index, siz = _children.Count(); ix < siz; ++ix)
		_children[ix]->SetIndexInParent(ix);

	if (_layout != nullptr)
	{
		_layout->ChildRemoved(index);
		MakeLayoutDirty(FudgetSizeType::All);
	}

	_changing = false;

	return index;
}

FudgetControl* FudgetContainer::RemoveChild(int index)
{
	if (index < 0 || _children.Count() <= index || _changing)
		return nullptr;
	FudgetControl *control = _children[index];
	RemoveChild(control);
	return control;
}

bool FudgetContainer::MoveChildToIndex(int from, int to)
{
	if (_changing)
		return true;
	if (from < 0 || to < 0 || from >= _children.Count() || to >= _children.Count())
		return false;
	if (from == to)
		return true;

	_changing = true;
	FudgetControl *control = _children[from];
	MoveInArray(_children, from, to);

	for (int ix = Math::Min(from, to), siz = Math::Max(from, to) + 1; ix < siz; ++ix)
		control->SetIndexInParent(ix);

	if (_layout != nullptr)
	{
		_layout->ChildMoved(from, to);
		MakeLayoutDirty(FudgetSizeType::All);
	}

	_changing = false;

	return true;
}

FudgetControl* FudgetContainer::ChildAt(int index) const
{
	if (index < 0 || _children.Count() <= index)
		return nullptr;
	return _children[index];
}

int FudgetContainer::GetChildCount() const
{
	return _children.Count();
}

int FudgetContainer::ChildIndex(FudgetControl *control) const
{
	if (control->GetParent() != this)
		return -1;
	int o = control->GetIndexInParent();
	if (o >= 0 && o < _children.Count() && _children[o] == control)
		return o;

	for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
	{
		if (_children[ix] == control)
			return ix;
	}
	return -1;
}

void FudgetContainer::DeleteAll()
{
	for (int ix = _children.Count() - 1; ix > -1; --ix)
	{
		FudgetControl *c = _children[ix];
		c->SetParent(nullptr);
		Delete(c);
	}
	MakeLayoutDirty(FudgetSizeType::All);
}

Float2 FudgetContainer::GetHintSize() const
{
	if (!_root && _layout != nullptr && (_height_from_layout || _width_from_layout))
	{
		Float2 value = _layout->GetHintSize();
		if (_height_from_layout && _width_from_layout)
			return value;

		Float2 base = Base::GetHintSize();
		if (!_height_from_layout)
			value.Y = base.Y;
		if (!_width_from_layout)
			value.X = base.X;
	}
	return Base::GetHintSize();
}

Float2 FudgetContainer::GetMinSize() const
{
	if (_root)
		return Base::GetHintSize();

	if (_layout != nullptr && (_height_from_layout || _width_from_layout))
	{
		Float2 value = _layout->GetMinSize();

		Float2 base = Base::GetMinSize();
		if (!_height_from_layout)
			value.Y = base.Y;
		if (!_width_from_layout)
			value.X = base.X;
	}
	return Base::GetMinSize();
}

Float2 FudgetContainer::GetMaxSize() const
{
	if (_root)
		return Base::GetHintSize();

	if (_layout != nullptr && (_height_from_layout || _width_from_layout))
	{
		Float2 value = _layout->GetMaxSize();

		Float2 base = Base::GetMaxSize();
		if (!_height_from_layout)
			value.Y = base.Y;
		if (!_width_from_layout)
			value.X = base.X;
	}
	return Base::GetMaxSize();
}

void FudgetContainer::SetUsingLayoutHeight(bool value)
{
	if (_height_from_layout == value)
		return;
	_height_from_layout = value;
	if (_layout == nullptr)
		return;
	MakeLayoutDirty(FudgetSizeType::All);
}

void FudgetContainer::SetUsingLayoutWidth(bool value)
{
	if (_width_from_layout == value)
		return;
	_width_from_layout = value;
	if (_layout == nullptr)
		return;
	MakeLayoutDirty(FudgetSizeType::All);
}

void FudgetContainer::MakeLayoutDirty(FudgetSizeType sizeType)
{
	if (_layout != nullptr)
		_layout->MakeDirty(sizeType);
	else if (GetParent() != nullptr)
		MakeParentLayoutDirty(sizeType);
}

void FudgetContainer::RequestLayout()
{
	if (_layout != nullptr)
		_layout->LayoutChildren(false);
}

void FudgetContainer::Draw()
{
	RequestLayout();
	for (FudgetControl *c : _children)
		c->Draw();
}

void FudgetContainer::AddLayoutInternal(FudgetLayout *layout)
{
	_layout = layout;
	layout->SetOwner(this);
}
