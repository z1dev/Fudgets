#include "GUIRoot.h"
#include "Fudget.h"

#include "Engine/Input/Input.h"

FudgetGUIRoot::FudgetGUIRoot() : Base(SpawnParams(Guid::New(), TypeInitializer)), events_initialized(false)
{
	_root = nullptr;
}

FudgetGUIRoot::FudgetGUIRoot(Fudget *root) : FudgetGUIRoot()
{
	_root = root;
	InitializeEvents();
}

FudgetGUIRoot::~FudgetGUIRoot()
{
	UninitializeEvents();
}

Float2 FudgetGUIRoot::GetSize() const
{
	if (_root != nullptr)
		return _root->GetSize();
	return Base::GetHintSize();
}

Float2 FudgetGUIRoot::GetHintSize() const
{
	if (_root != nullptr)
		return _root->GetSize();
	return Base::GetHintSize();
}

Float2 FudgetGUIRoot::GetMinSize() const
{
	if (_root != nullptr)
		return _root->GetSize();
	return Base::GetHintSize();
}

Float2 FudgetGUIRoot::GetMaxSize() const
{
	if (_root != nullptr)
		return _root->GetSize();
	return Base::GetHintSize();
}

void FudgetGUIRoot::InitializeEvents()
{
	if (events_initialized)
		return;

	Input::MouseDoubleClick.Bind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseDoubleClick>(this);
	Input::MouseDown.Bind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseDown>(this);
	Input::MouseUp.Bind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseUp>(this);
	Input::MouseMove.Bind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseMove>(this);
	Input::MouseLeave.Bind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseLeave>(this);

	events_initialized = true;
}

void FudgetGUIRoot::UninitializeEvents()
{
	if (!events_initialized)
		return;

	Input::MouseDoubleClick.Unbind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseDoubleClick>(this);
	Input::MouseDown.Unbind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseDown>(this);
	Input::MouseUp.Unbind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseUp>(this);
	Input::MouseMove.Unbind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseMove>(this);
	Input::MouseLeave.Unbind<FudgetGUIRoot, &FudgetGUIRoot::OnMouseLeave>(this);

	events_initialized = false;
}

void FudgetGUIRoot::OnMouseDown(const Float2 &pos, MouseButton button)
{

}

void FudgetGUIRoot::OnMouseUp(const Float2 &pos, MouseButton button)
{

}

void FudgetGUIRoot::OnMouseMove(const Float2 &pos)
{

}

void FudgetGUIRoot::OnMouseLeave()
{

}

void FudgetGUIRoot::OnMouseDoubleClick(const Float2 &pos, MouseButton button)
{

}


