#include "GUIRoot.h"
#include "Fudget.h"
#include "IFudgetMouseHook.h"

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

void FudgetGUIRoot::RegisterMouseHook(IFudgetMouseHook *hook, FudgetMouseHookType type)
{
	if (type == FudgetMouseHookType::Global || type == FudgetMouseHookType::Both)
		global_mouse_hooks.AddUnique(hook);
	if (type == FudgetMouseHookType::Local || type == FudgetMouseHookType::Both)
		local_mouse_hooks.AddUnique(hook);
}

void FudgetGUIRoot::UnregisterMouseHook(IFudgetMouseHook *hook, FudgetMouseHookType type)
{
	if ((type == FudgetMouseHookType::Global || type == FudgetMouseHookType::Both) && global_mouse_hooks.Remove(hook))
		LOG(Warning, "Mouse hook passed in UnregisterMouseHook was not registered as a global mouse hook");
	if ((type == FudgetMouseHookType::Local || type == FudgetMouseHookType::Both) && local_mouse_hooks.Remove(hook))
		LOG(Warning, "Mouse hook passed in UnregisterMouseHook was not registered as a global mouse hook");
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

FudgetMouseHookResult FudgetGUIRoot::ProcessLocalMouseHooks(HookProcessingType type, FudgetControl *control, Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
	FudgetMouseHookResult result = FudgetMouseHookResult::NormalProcessing;
	for (int ix = 0, siz = local_mouse_hooks.Count(); ix < siz && result == FudgetMouseHookResult::NormalProcessing; ++ix)
	{
		switch (type)
		{
			case HookProcessingType::MouseDown:
				result = local_mouse_hooks[ix]->OnLocalMouseDown(this, control, global_pos, pos, button, double_click);
				break;
			case HookProcessingType::MouseUp:
				result = local_mouse_hooks[ix]->OnLocalMouseUp(this, control, global_pos, pos, button);
				break;
			case HookProcessingType::MouseMove:
				result = local_mouse_hooks[ix]->OnLocalMouseMove(this, control, global_pos, pos);
				break;
			case HookProcessingType::MouseEnter:
				result = local_mouse_hooks[ix]->OnLocalMouseEnter(this, control, global_pos, pos);
				break;
			case HookProcessingType::MouseLeave:
				result = local_mouse_hooks[ix]->OnLocalMouseLeave(this, control);
				break;
			default:
				break;
		}
	}
	return result;
}

void FudgetGUIRoot::OnMouseDown(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!global_mouse_hooks[ix]->OnGlobalMouseDown(this, pos, button))
				return;
	}

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
			if (c->WantsMouseEventAtPos(cpos, pos)) 
			{
				if (!local_mouse_hooks.IsEmpty())
				{
					FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, false);
					if (result == FudgetMouseHookResult::EatEvent)
						return;
					if (result == FudgetMouseHookResult::SkipControl)
						continue;
				}

				if (c->OnMouseDown(cpos, pos, button, false) || mouse_capture_control != nullptr)
					break;
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::OnMouseUp(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!global_mouse_hooks[ix]->OnGlobalMouseUp(this, pos, button))
				return;
	}

	if (mouse_capture_control != nullptr && mouse_capture_button == button)
	{
		Float2 cpos = mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseUp, mouse_capture_control, cpos, pos, button, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			mouse_capture_control->OnMouseUp(cpos, pos, button);
			//ReleaseMouseCapture();
			OnMouseMove(pos);
		}
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
			if (c->WantsMouseEventAtPos(cpos, pos))
			{
				FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseUp, c, cpos, pos, button, false);
				if (result == FudgetMouseHookResult::EatEvent)
					return;
				if (result == FudgetMouseHookResult::SkipControl)
					continue;

				if (c->OnMouseUp(cpos, pos, button))
				{
					ReleaseMouseCapture();
					OnMouseMove(pos);
					break;
				}
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::OnMouseDoubleClick(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!global_mouse_hooks[ix]->OnGlobalMouseDoubleClick(this, pos, button))
				return;
	}

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
			if (c->WantsMouseEventAtPos(cpos, pos))
			{
				FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, true);
				if (result == FudgetMouseHookResult::EatEvent)
					return;
				if (result == FudgetMouseHookResult::SkipControl)
					continue;

				if (c->OnMouseDown(cpos, pos, button, true) || mouse_capture_control != nullptr)
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

	if (!global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!global_mouse_hooks[ix]->OnGlobalMouseMove(this, pos))
				return;
	}

	if (mouse_capture_control != nullptr)
	{
		Float2 cpos = mouse_capture_control->GlobalToLocal(pos);
		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseMove, mouse_capture_control, cpos, pos, MouseButton::None, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
			mouse_capture_control->OnMouseMove(cpos, pos);
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
			if (c->WantsMouseEventAtPos(cpos, pos))
			{
				if (mouse_over_control != c)
				{
					bool wants_enterleave = c->HasAnyFlag(FudgetControlFlags::CanHandleMouseEnterLeave);
					FudgetControl *old_mouse_control = mouse_over_control;
					// Make sure the old control can check which is the new one.
					mouse_over_control = wants_enterleave ? c : nullptr;

					if (old_mouse_control != nullptr)
					{
						FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, c, Float2(), Float2(), MouseButton::None, false);
						if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
							old_mouse_control->OnMouseLeave();
					}

					if (wants_enterleave)
					{
						// Let the new control check where the mouse came from
						mouse_over_control = old_mouse_control;

						FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseEnter, c, cpos, pos, MouseButton::None, false);
						if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
							c->OnMouseEnter(cpos, pos);
						mouse_over_control = c;
					}
				}

				FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseMove, c, cpos, pos, MouseButton::None, false);
				if (result == FudgetMouseHookResult::EatEvent)
					return;
				if (result == FudgetMouseHookResult::SkipControl)
					continue;

				c->OnMouseMove(cpos, pos);
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

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, old_mouse_control, Float2(), Float2(), MouseButton::None, false);
		if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
			old_mouse_control->OnMouseLeave();
	}
}

void FudgetGUIRoot::OnMouseLeave()
{
	if (!global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!global_mouse_hooks[ix]->OnGlobalMouseLeave(this))
				return;
	}

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

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, old_mouse_control, Float2(), Float2(), MouseButton::None, false);
		if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
			old_mouse_control->OnMouseLeave();
	}
}

