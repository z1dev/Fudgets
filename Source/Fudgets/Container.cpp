#include "Container.h"
#include "Fudget.h"
#include "Layouts/Layout.h"
#include "Engine/Core/Math/Rectangle.h"
#include "Utils/Utils.h"


FudgetContainer::FudgetContainer(const SpawnParams &params) : FudgetContainer(params, FudgetControlFlags::BlockMouseEvents)
{

}

FudgetContainer::FudgetContainer(FudgetControlFlags flags) : FudgetContainer(SpawnParams(Guid::New(), TypeInitializer), flags)
{
}

FudgetContainer::FudgetContainer(const SpawnParams &params, FudgetControlFlags flags) : Base(params, flags | FudgetControlFlags::ContainerControl),
	fill_color(1.0f), fill_background(false),_layout(nullptr), _changing(false), _width_from_layout(false), _height_from_layout(false),
	_min_width_from_layout(false), _max_width_from_layout(false), _min_height_from_layout(false), _max_height_from_layout(false)
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
		_layout->ChildAdded(control, index);

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
		_layout->ChildRemoved(index);

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
		_children[ix]->SetIndexInParent(ix);

	if (_layout != nullptr)
		_layout->ChildMoved(from, to);

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
		// TODO: lock the layout to not change until this loop is over, if it's noticable on performance

		FudgetControl *c = _children[ix];
		c->SetParent(nullptr);
		Delete(c);
	}
	if (_layout != nullptr)
		_layout->AllDeleted();
}

void FudgetContainer::SetHintSize(Float2 value)
{
	if (Float2::NearEqual(_hint_size, value))
		return;
	Base::SetHintSize(value);

	if ((_width_from_layout && _height_from_layout) || _layout == nullptr)
		return;
	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetMinSize(Float2 value)
{
	if (Float2::NearEqual(_min_size, value))
		return;
	Base::SetMinSize(value);

	if ((_min_width_from_layout && _min_height_from_layout) || _layout == nullptr)
		return;
	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetMaxSize(Float2 value)
{
	if (Float2::NearEqual(_max_size, value))
		return;
	Base::SetMaxSize(value);

	if ((_max_width_from_layout && _max_height_from_layout) || _layout == nullptr)
		return;
	MarkLayoutDirty(FudgetDirtType::Size);
}

Float2 FudgetContainer::GetHintSize() const
{
	if (_layout != nullptr && (_width_from_layout || _height_from_layout))
	{
		Float2 value = _layout->GetHintSize();
		if (_width_from_layout && _height_from_layout)
			return value;

		Float2 base = Base::GetHintSize();
		if (!_width_from_layout)
			value.X = base.X;
		if (!_height_from_layout)
			value.Y = base.Y;
		return value;
	}
	return Base::GetHintSize();
}

Float2 FudgetContainer::GetMinSize() const
{
	if (_layout != nullptr && (_min_width_from_layout || _min_height_from_layout))
	{
		Float2 value = _layout->GetMinSize();
		if (_min_width_from_layout || _min_height_from_layout)
			return Float2::Max(Float2::Zero, value);

		Float2 base = Base::GetMinSize();
		if (!_min_width_from_layout)
			value.X = base.X;
		if (!_min_height_from_layout)
			value.Y = base.Y;
		return Float2::Max(Float2::Zero, value);
	}
	return Base::GetMinSize();
}

Float2 FudgetContainer::GetMaxSize() const
{
	Float2 hint_size = GetHintSize();

	if (_layout != nullptr && (_max_width_from_layout || _max_height_from_layout))
	{
		Float2 value = _layout->GetMaxSize();
		if (_max_width_from_layout && _max_height_from_layout)
			return value;

		Float2 base = Base::GetMaxSize();
		if (!_max_width_from_layout)
			value.X = base.X;
		if (!_max_height_from_layout)
			value.Y = base.Y;
		return value;
	}
	return Base::GetMaxSize();

}

void FudgetContainer::SetUsingLayoutWidth(bool value)
{
	if (_width_from_layout == value)
		return;

	_width_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutHeight(bool value)
{
	if (_height_from_layout == value)
		return;

	_height_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMinWidth(bool value)
{
	if (_min_width_from_layout == value)
		return;

	_min_width_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMaxWidth(bool value)
{
	if (_max_width_from_layout == value)
		return;

	_max_width_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMinHeight(bool value)
{
	if (_min_height_from_layout == value)
		return;

	_min_height_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::SetUsingLayoutMaxHeight(bool value)
{
	if (_max_height_from_layout == value)
		return;

	_max_height_from_layout = value;

	if (_layout == nullptr)
		return;

	MarkLayoutDirty(FudgetDirtType::Size);
}

void FudgetContainer::MarkLayoutDirty(FudgetDirtType dirt_flags, bool content_changed)
{
	if (_layout == nullptr)
		return;

	_layout->MarkDirty(dirt_flags, content_changed);

	if (_parent != nullptr && _layout->IsSizeDirty())
	{
		// Notify the parent only, if the change in the layout influences the container's size as well
		if (GetUsingLayoutWidth() || GetUsingLayoutHeight() ||
			GetUsingLayoutMinWidth() || GetUsingLayoutMaxWidth() ||
			GetUsingLayoutMinHeight() || GetUsingLayoutMaxHeight())
			_parent->MarkLayoutDirty(dirt_flags, true);
	}
}

void FudgetContainer::RequestLayout()
{
	if (_layout != nullptr)
	{
		_layout->RequestLayoutChildren(false);
		return;
	}

	// TODO: mark sizes dirty and don't do anything when it's not

	// With no layout, every control is positioned wherever they want to be.
	for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
	{
		FudgetControl *control = _children[ix];
		control->_size = control->_hint_size;
	}
}

void FudgetContainer::Draw()
{
	RequestLayout();

	if (fill_background)
	{
		FillRectangle(Float2(0.0f), GetSize(), fill_color);
	}

	for (FudgetControl *c : _children)
		c->Draw();
}

void FudgetContainer::AddLayoutInternal(FudgetLayout *layout)
{
	_layout = layout;
	// TODO: check that this dirties the parent layouts
	layout->SetOwner(this);
}

void FudgetContainer::SizeOrPosModified(FudgetDirtType dirt_flags)
{
	; // NO-OP
}

bool FudgetContainer::IsControlPositioningPermitted(const FudgetControl *control) const
{
	if (_layout == nullptr)
		return true;
	if (control->GetParent() != this)
		return false;
	return _layout->IsControlPositioningPermitted(control);
}

void FudgetContainer::GetAllControls(const FudgetContainer *container, API_PARAM(Ref) Array<FudgetControl*> &result)
{
	for (int i = 0; i < container->GetChildCount(); i++)
	{
		FudgetControl* child = container->ChildAt(i);
		result.Add(child);

		if (child->Is<FudgetContainer>())
			GetAllControls((FudgetContainer*)child, result);
	}
}

void FudgetContainer::GetAllControls(API_PARAM(Ref) Array<FudgetControl*> &result) const
{
	FudgetContainer::GetAllControls(this, result);
}

void FudgetContainer::ControlsUnderMouse(Float2 pos, FudgetControlFlags request, API_PARAM(Ref) Array<FudgetControl*> &result)
{
	for (int ix = 0, siz = _children.Count(); ix < siz; ++ix)
	{
		FudgetControl *control = _children[ix];
		if (!control->GetBoundingBox().Contains(pos))
			continue;

		if (request == FudgetControlFlags::None || control->HasAnyFlag(request))
			result.Add(control);
		if (control->HasAnyFlag(FudgetControlFlags::ContainerControl))
			dynamic_cast<FudgetContainer*>(control)->ControlsUnderMouse(pos - control->_pos, request, result);
	}
}

void FudgetContainer::Serialize(SerializeStream& stream, const void* otherObj)
{
	Base::Serialize(stream, otherObj);
	_layout->Serialize(stream, otherObj);
}

void FudgetContainer::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
	Base::Deserialize(stream, modifier);
	_layout->Deserialize(stream, modifier);
}

void FudgetContainer::LayoutUpdate(Float2 pos, Float2 size)
{
	bool pos_changed = !Float2::NearEqual(_pos, pos);
	bool size_changed = !Float2::NearEqual(_size, size);

	Base::LayoutUpdate(pos, size);

	if (_layout != nullptr && (pos_changed || size_changed))
	{
		FudgetDirtType type = (FudgetDirtType)((pos_changed ? (int)FudgetDirtType::Position : 0) | (size_changed ? (int)FudgetDirtType::Size : 0));

		_layout->MarkDirtyOnLayoutUpdate(type);
	}
}
