#include "FcContainer.h"
#include "Layouts/FcLayout.h"


FcContainer::FcContainer() : FcControl(), layout(nullptr)
{

}

FcContainer::~FcContainer()
{
	for (FcControl *c : _children)
	{
		c->SetParent(nullptr);
		Delete(c);
	}
}

bool FcContainer::AddChild(FcControl *control)
{
	int ix = _children.Find(control);
	if (ix >= 0)
		return false;

	_children.Add(control);
	control->SetParent(this);

	return true;
}

bool FcContainer::RemoveChild(FcControl *control)
{
	int ix = _children.Find(control);
	if (ix < 0)
		return false;

	_children.RemoveAt(ix);
	control->SetParent(this);

	return true;
}

void FcContainer::Draw()
{
	for (FcControl *c : _children)
		c->Draw();
}
