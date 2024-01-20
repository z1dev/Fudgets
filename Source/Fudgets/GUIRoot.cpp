#include "GUIRoot.h"
#include "Fudget.h"

#include "Engine/Input/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Engine/Screen.h"
#include "Engine/Platform/Base/WindowBase.h"

#include "Engine/Core/Log.h"

FudgetGUIRoot::FudgetGUIRoot() : Base(SpawnParams(Guid::New(), TypeInitializer), FudgetControlFlags::ContainerControl),
	events_initialized(false), mouse_capture_control(nullptr), mouse_capture_button(), mouse_over_control(nullptr)
{
	_root = nullptr;
	_window = nullptr;
}

FudgetGUIRoot::FudgetGUIRoot(Fudget *root) : FudgetGUIRoot()
{
	_root = root;
	_window = (WindowBase*)Screen::GetMainWindow();
	_guiRoot = this;
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

void FudgetGUIRoot::StartMouseCapture(FudgetControl *control)
{
	if (mouse_capture_control == control)
		return;
	if (mouse_capture_control != nullptr)
		ReleaseMouseCapture();
	mouse_capture_control = control;
	if (mouse_capture_control != nullptr && _window != nullptr)
		_window->StartTrackingMouse(false);
}

void FudgetGUIRoot::ReleaseMouseCapture()
{
	if (mouse_capture_control == nullptr)
		return;
	mouse_capture_control = nullptr;
	_window->EndTrackingMouse();
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

void FudgetGUIRoot::OnMouseDown(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (mouse_capture_control == nullptr)
		mouse_capture_button = button;

	Array<FudgetControl*> controls_for_input;
	ControlsUnderMouse(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, controls_for_input);
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos) && c->OnMouseDown(cpos, button) || mouse_capture_control != nullptr)
				break;
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::OnMouseUp(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (mouse_capture_control != nullptr && mouse_capture_button == button)
	{
		mouse_capture_control->OnMouseUp(mouse_capture_control->GlobalToLocal(pos), button);
		ReleaseMouseCapture();
		OnMouseMove(pos);
		return;
	}

	Array<FudgetControl*> controls_for_input;
	ControlsUnderMouse(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, controls_for_input);
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos) && c->OnMouseUp(cpos, button))
			{
				ReleaseMouseCapture();
				OnMouseMove(pos);
				break;
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::OnMouseDoubleClick(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (mouse_capture_control != nullptr && mouse_capture_button == button)
	{
		LOG(Error, "MouseCapture not null on OnMouseDoubleClick");
		ReleaseMouseCapture();
		return;
	}
	Array<FudgetControl*> controls_for_input;
	ControlsUnderMouse(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, controls_for_input);
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			bool double_as_single_click = c->HasAnyFlag(FudgetControlFlags::ConvertDoubleClickToSingle);
			if (c->WantsMouseEventAtPos(cpos))
			{
				if ((!double_as_single_click && c->OnMouseDoubleClick(cpos, button)) ||
					(double_as_single_click && c->OnMouseDown(cpos, button)) ||
					mouse_capture_control != nullptr)
				{
					OnMouseMove(pos);
					break;
				}
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::OnMouseMove(const Float2 &__pos)
{
	Float2 pos = Input::GetMousePosition();

	if (mouse_capture_control != nullptr)
	{
		mouse_capture_control->OnMouseMove(mouse_capture_control->GlobalToLocal(pos));
		return;
	}

	Array<FudgetControl*> controls_for_input;
	ControlsUnderMouse(pos, FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::BlockMouseEvents, controls_for_input);

	bool post_leave = mouse_over_control != nullptr;
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos))
			{
				if (mouse_over_control != c)
				{
					bool wants_enterleave = c->HasAnyFlag(FudgetControlFlags::CanHandleMouseEnterLeave);
					FudgetControl *old_mouse_control = mouse_over_control;
					// Make sure the old control can check which is the new one.
					mouse_over_control = wants_enterleave ? c : nullptr;
					if (old_mouse_control != nullptr)
						old_mouse_control->OnMouseLeave();
					if (wants_enterleave)
					{
						// Let the new control check where the mouse came from
						mouse_over_control = old_mouse_control;
						c->OnMouseEnter(cpos);
						mouse_over_control = c;
					}
				}
				c->OnMouseMove(cpos);
				return;
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
	if (post_leave)
	{
		FudgetControl *old_mouse_control = mouse_over_control;
		// Make sure the control can check that no control has the mouse
		mouse_over_control = nullptr;
		old_mouse_control->OnMouseLeave();
	}
}

void FudgetGUIRoot::OnMouseLeave()
{
	if (mouse_capture_control != nullptr)
	{
		LOG(Error, "MouseCapture not null on OnMouseLeave");
		ReleaseMouseCapture();
		return;
	}

	if (mouse_over_control != nullptr)
	{
		FudgetControl *old_mouse_control = mouse_over_control;
		// Make sure the control can check that no control has the mouse
		mouse_over_control = nullptr;
		old_mouse_control->OnMouseLeave();
	}
}

