#include "Container.h"
#include "Layouts/Layout.h"
#include "Engine/Core/Math/Rectangle.h"


FudgetContainer::FudgetContainer() : Base(), _layout(nullptr)
{

}

FudgetContainer::~FudgetContainer()
{
	for (FudgetControl *c : _children)
	{
		c->SetParent(nullptr);
		Delete(c);
	}
}

int FudgetContainer::AddChild(FudgetControl *control)
{
	if (control == nullptr)
		return -1;

	if (control->GetParent() == this)
		return control->GetOrder();

	_children.Add(control);
	control->SetParent(this);

	if (_layout != nullptr)
	{
		_layout->ChildAdded(control);
	}

	return _children.Count() - 1;
}

int FudgetContainer::RemoveChild(FudgetControl *control)
{
	if (control == nullptr)
		return -1;

	int order = control->GetOrder();

	// Checking the parent is not enough if called indirectly from control->SetParent()
	if (control->GetParent() != this || _children.Count() <= order || _children[order] != control)
		return -1;


	_children.RemoveAt(control->GetOrder());
	control->SetParent(nullptr);

	if (_layout != nullptr)
	{
		_layout->ChildRemoved(order);
	}

	return order;
}

FudgetControl* FudgetContainer::RemoveChild(int at)
{
	if (at < 0 || _children.Count() <= at)
		return nullptr;
	FudgetControl *control = _children[at];
	RemoveChild(control);
	return control;
}

FudgetControl* FudgetContainer::ChildAt(int at) const
{
	if (at < 0 || _children.Count() <= at)
		return nullptr;
	return _children[at];
}

int FudgetContainer::GetChildCount() const
{
	return _children.Count();
}


Float2 FudgetContainer::GetPreferredSize() const
{
	if (_layout != nullptr)
		return _layout->GetPreferredSize();
	return Base::GetPreferredSize();
}

Float2 FudgetContainer::GetMinSize() const
{
	if (_layout != nullptr)
		return _layout->GetMinSize();
	return Base::GetMinSize();
}

Float2 FudgetContainer::GetMaxSize() const
{
	if (_layout != nullptr)
		return _layout->GetMaxSize();
	return Base::GetMaxSize();
}

void FudgetContainer::MakeLayoutDirty()
{
	if (_layout != nullptr)
		_layout->MakeDirty();
}

void FudgetContainer::RequestLayout()
{
	if (_layout != nullptr)
		_layout->LayoutChildren();
}

void FudgetContainer::Draw()
{
	RequestLayout();
	for (FudgetControl *c : _children)
		c->Draw();
}
