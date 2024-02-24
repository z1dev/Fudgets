#include "GUIRoot.h"
#include "Fudget.h"
#include "IFudgetMouseHook.h"
#include "Styling/Themes.h"
#include "Utils/Utils.h"

#include "Engine/Level/Scene/Scene.h"
#include "Engine/Engine/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Engine/Screen.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Platform/Base/WindowBase.h"

#include "Engine/Core/Log.h"
#include <Engine/Serialization/JsonTools.h>
#include <Engine/Serialization/JsonWriters.h>


FUDGET_FACTORY(FudgetControl, control);

FudgetGUIRoot::FudgetGUIRoot(const SpawnParams& params) : FudgetGUIRoot(params, nullptr)
{
}

FudgetGUIRoot::FudgetGUIRoot(Fudget* root) : FudgetGUIRoot(SpawnParams(Guid::New(), TypeInitializer), root)
{
}

FudgetGUIRoot::FudgetGUIRoot(const SpawnParams &params, Fudget *root) : Base(params),
	events_initialized(false), _root(root), _window((WindowBase*)Screen::GetMainWindow()), _on_top_count(0),
	_mouse_capture_control(nullptr), _mouse_capture_button(), _mouse_over_control(nullptr), _focus_control(nullptr),
	_processing_updates(false)
{
	_guiRoot = this;
}

FudgetGUIRoot::~FudgetGUIRoot()
{
	UninitializeEvents();
	UnregisterControlUpdates();
}

void FudgetGUIRoot::FudgetInit()
{
	if (IsInRunningGame())
		InitializeEvents();
	Base::Initialize();
}

int FudgetGUIRoot::AddChild(FudgetControl *control, int index)
{
	FudgetControlFlags flags = control->_flags;

	if ((flags & FudgetControlFlags::ResetFlags) == FudgetControlFlags::ResetFlags)
		flags = control->GetInitFlags();

	bool on_top = (flags & FudgetControlFlags::AlwaysOnTop) == FudgetControlFlags::AlwaysOnTop;
	if (!on_top)
	{
		if (index < 0 || index > GetChildCount() - _on_top_count)
			index = GetChildCount() - _on_top_count;
	}
	else
	{
		if (index >= 0)
			index += GetChildCount() - _on_top_count;
		if (index < 0 || index > GetChildCount())
			index = GetChildCount();
	}

	int result = Base::AddChild(control, index);
	if (result != -1)
	{
		if ((!on_top && result < GetChildCount() - _on_top_count) || result < GetChildCount() - _on_top_count - 1)
		{
			control->_flags &= ~(FudgetControlFlags::AlwaysOnTop);
		}
		else
		{
			++_on_top_count;
			control->_flags |= FudgetControlFlags::AlwaysOnTop;
		}
	}
	return result;
}

int FudgetGUIRoot::RemoveChild(FudgetControl *control)
{
	if (control == nullptr || control->GetParent() != this)
		return -1;

	if (_focus_control == control)
		SetFocusedControl(nullptr);

	if (control->HasAnyFlag(FudgetControlFlags::AlwaysOnTop))
		--_on_top_count;
	return Base::RemoveChild(control);
}

bool FudgetGUIRoot::MoveChildToIndex(int from, int to)
{
	int cnt = GetChildCount();
	if (from < 0 || to < 0 || from >= cnt || to >= cnt)
		return false;
	if (from == to)
		return true;

	bool fromtop = from >= cnt - _on_top_count;
	bool totop = to >= cnt - _on_top_count;

	FudgetControl *control = ChildAt(from);

	if (fromtop != totop)
	{
		LOG(Warning, "Moving a control from always-on-top to not always-on-top or the reverse is not supported.");
		return false;
	}

	return Base::MoveChildToIndex(from, to);
}

int FudgetGUIRoot::FudgetGUIRoot::ChangeControlAlwaysOnTop(FudgetControl *control, bool set_always_on_top, int index)
{
	if (control == nullptr || control->_parent != this || control->HasAnyFlag(FudgetControlFlags::AlwaysOnTop) == set_always_on_top)
		return -1;
	int cnt = !set_always_on_top ? GetChildCount() - _on_top_count : _on_top_count;
	if (index < 0 || index > cnt)
		index = cnt;
	int from = control->_index;
	int to;
	if (!set_always_on_top)
		to = index;
	else
		to = GetChildCount() - 1 - _on_top_count + index;

	int new_index = from != to ? Base::MoveChildToIndex(from, to) : control->_index;
	if (new_index != -1)
	{
		if (set_always_on_top)
		{
			++_on_top_count;
			control->_flags |= FudgetControlFlags::AlwaysOnTop;
		}
		else
		{
			--_on_top_count;
			control->_flags &= ~FudgetControlFlags::AlwaysOnTop;
		}
	}
	return new_index;
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
		_global_mouse_hooks.AddUnique(hook);
	if (type == FudgetMouseHookType::Local || type == FudgetMouseHookType::Both)
		_local_mouse_hooks.AddUnique(hook);
}

void FudgetGUIRoot::UnregisterMouseHook(IFudgetMouseHook *hook, FudgetMouseHookType type)
{
	if ((type == FudgetMouseHookType::Global || type == FudgetMouseHookType::Both) && _global_mouse_hooks.Remove(hook))
		LOG(Warning, "Mouse hook passed in UnregisterMouseHook was not registered as a global mouse hook");
	if ((type == FudgetMouseHookType::Local || type == FudgetMouseHookType::Both) && _local_mouse_hooks.Remove(hook))
		LOG(Warning, "Mouse hook passed in UnregisterMouseHook was not registered as a local mouse hook");
}


void FudgetGUIRoot::StartMouseCapture(FudgetControl *control)
{
	if (_mouse_capture_control == control)
		return;
	if (_mouse_capture_control != nullptr)
		ReleaseMouseCapture();
	_mouse_capture_control = control;
	if (_mouse_capture_control != nullptr && _window != nullptr)
	{
		_window->StartTrackingMouse(false);
		_mouse_capture_control->DoMouseCaptured();
	}
}

void FudgetGUIRoot::ReleaseMouseCapture()
{
	if (_mouse_capture_control == nullptr)
		return;
	FudgetControl *tmp = _mouse_capture_control;
	_mouse_capture_control = nullptr;
	_window->EndTrackingMouse();
	tmp->DoMouseReleased();
}

void FudgetGUIRoot::SetFocusedControl(FudgetControl *value)
{
	if (_focus_control == value)
		return;
	if (!_focus_control_keys.IsEmpty())
	{
		HashSet<KeyboardKeys> tmp = _focus_control_keys;
		for (auto k : tmp)
		{
			OnKeyUp(k.Item);
		}

		_focus_control_keys.Clear();
	}

	if (_focus_control != nullptr)
		_focus_control->DoFocusChanging(false, value);
	if (value != nullptr)
		value->DoFocusChanging(true, _focus_control);

	std::swap(_focus_control, value);
	if (value != nullptr)
		value->DoFocusChanged(false, _focus_control);
	if (_focus_control != nullptr)
		_focus_control->DoFocusChanged(true, value);
}

void FudgetGUIRoot::UpdateCursor(FudgetControl *control)
{
	if (control == this || _mouse_over_control != control)
		return;

	CursorType cursor = control != nullptr ? control->GetContextCursor() : CursorType::Default;
	if (GetCursor() == cursor)
		return;

	SetCursor(cursor);

	if (_window != nullptr)
		_window->SetCursor(cursor);
}

bool FudgetGUIRoot::RegisterControlUpdate(FudgetControl *control, bool value)
{
	if (control == nullptr)
		return false;
	if (control->IsUpdateRegistered() == value)
		return true;

	if (_processing_updates)
	{
		LOG(Warning, "Can't register or unregister a control during updates. The control will be (un)registered after all OnUpdates are over. This can be undefined behavior.");
		DelayRegisterControlUpdate(control, value);
		return false;
	}

	if (_root == nullptr)
		return value == false;
	auto scene = _root->GetScene();
	if (scene == nullptr)
		return value == false;

	if (value)
	{
		_updating_controls.Add(control);
		if (_updating_controls.Count() == 1)
			scene->Ticking.Update.AddTick<FudgetGUIRoot, &FudgetGUIRoot::ControlUpdates>(this);
	}
	else
	{
		_updating_controls.Remove(control);
		if (_updating_controls.Count() == 0)
			scene->Ticking.Update.RemoveTick(this);
	}

	return true;
}

void FudgetGUIRoot::DelayRegisterControlUpdate(FudgetControl *control, bool value)
{
	if (control == nullptr)
		return;
	if (!_processing_updates)
	{
		control->RegisterToUpdate(value);
		return;
	}
	if (value)
		_controls_to_add_to_updating.Add(control);
	else
		_controls_to_remove_from_updating.Add(control);
}

void FudgetGUIRoot::UnregisterControlUpdates()
{
	_controls_to_add_to_updating.Clear();
	_controls_to_remove_from_updating.Clear();
	Array<FudgetControl*> tmp = _updating_controls;
	for (FudgetControl *c : tmp)
		c->RegisterToUpdate(false);
}

bool FudgetGUIRoot::IsNavigationKey(KeyboardKeys key) const
{
	// TODO: use the current input mapping instead of built in arrow keys

	return key == KeyboardKeys::ArrowLeft || key == KeyboardKeys::ArrowRight || key == KeyboardKeys::ArrowUp || key == KeyboardKeys::ArrowDown || key == KeyboardKeys::Tab;
}

void FudgetGUIRoot::OnResized(Float2 new_size)
{
	SizeModified();
	if (Resized.IsBinded())
		Resized(new_size);
}

void FudgetGUIRoot::DoLayout()
{
	RequestLayout();
}

void FudgetGUIRoot::InitializeEvents()
{
	if (events_initialized)
		return;

	Input::MouseDoubleClick.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseDoubleClick>(this);
	Input::MouseDown.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseDown>(this);
	Input::MouseUp.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseUp>(this);
	Input::MouseMove.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseMove>(this);
	Input::MouseLeave.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseLeave>(this);

	Input::CharInput.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleCharInput>(this);
	Input::KeyDown.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleKeyDown>(this);
	Input::KeyUp.Bind<FudgetGUIRoot, &FudgetGUIRoot::HandleKeyUp>(this);

	events_initialized = true;
}

void FudgetGUIRoot::UninitializeEvents()
{
	if (!events_initialized)
		return;

	Input::MouseDoubleClick.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseDoubleClick>(this);
	Input::MouseDown.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseDown>(this);
	Input::MouseUp.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseUp>(this);
	Input::MouseMove.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseMove>(this);
	Input::MouseLeave.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleMouseLeave>(this);

	Input::CharInput.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleCharInput>(this);
	Input::KeyDown.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleKeyDown>(this);
	Input::KeyUp.Unbind<FudgetGUIRoot, &FudgetGUIRoot::HandleKeyUp>(this);

	events_initialized = false;
}

void FudgetGUIRoot::ControlUpdates()
{
	_processing_updates = true;
	float time = Time::GetUnscaledDeltaTime();
	for (FudgetControl *c : _updating_controls)
	{
		c->DoUpdate(time);
	}
	_processing_updates = false;

	for (FudgetControl *c : _controls_to_add_to_updating)
		c->RegisterToUpdate(true);
	for (FudgetControl *c : _controls_to_remove_from_updating)
		c->RegisterToUpdate(false);
	_controls_to_add_to_updating.Clear();
	_controls_to_remove_from_updating.Clear();
}

FudgetMouseHookResult FudgetGUIRoot::ProcessLocalMouseHooks(HookProcessingType type, FudgetControl *control, Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
	FudgetMouseHookResult result = FudgetMouseHookResult::NormalProcessing;
	for (int ix = 0, siz = _local_mouse_hooks.Count(); ix < siz && result == FudgetMouseHookResult::NormalProcessing; ++ix)
	{
		switch (type)
		{
			case HookProcessingType::MouseDown:
				result = _local_mouse_hooks[ix]->OnLocalMouseDown(this, control, global_pos, pos, button, double_click);
				break;
			case HookProcessingType::MouseUp:
				result = _local_mouse_hooks[ix]->OnLocalMouseUp(this, control, global_pos, pos, button);
				break;
			case HookProcessingType::MouseMove:
				result = _local_mouse_hooks[ix]->OnLocalMouseMove(this, control, global_pos, pos);
				break;
			case HookProcessingType::MouseEnter:
				result = _local_mouse_hooks[ix]->OnLocalMouseEnter(this, control, global_pos, pos);
				break;
			case HookProcessingType::MouseLeave:
				result = _local_mouse_hooks[ix]->OnLocalMouseLeave(this, control);
				break;
			default:
				break;
		}
	}
	return result;
}

void FudgetGUIRoot::HandleMouseDown(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!_global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = _global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!_global_mouse_hooks[ix]->OnGlobalMouseDown(this, pos, button))
				return;
	}

	if (_mouse_capture_control == nullptr)
	{
		_mouse_capture_button = button;
	}
	else
	{
		Float2 cpos = _mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, _mouse_capture_control, cpos, pos, button, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			FudgetInputResult result = _mouse_capture_control->OnMouseDown(cpos, pos, button, false);
			if (result == FudgetInputResult::Consume)
			{
				if ((button == MouseButton::Left && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
					(button == MouseButton::Right && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
					_mouse_capture_control->SetFocused(true);
			}
		}
		return;
	}

	Array<FudgetControl*> controls_for_input;
	ControlsAtPosition(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, FudgetControlFlags::None, FudgetControlFlags::CompoundControl, controls_for_input);
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos, pos)) 
			{
				if (!_local_mouse_hooks.IsEmpty())
				{
					FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, false);
					if (result == FudgetMouseHookResult::EatEvent)
						return;
					if (result == FudgetMouseHookResult::SkipControl)
						continue;
				}

				FudgetInputResult result = c->OnMouseDown(cpos, pos, button, false);
				if (result == FudgetInputResult::Consume)
				{
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
						c->CaptureMouseInput();
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
						c->SetFocused(true);
				}

				if (result != FudgetInputResult::PassThrough || _mouse_capture_control != nullptr)
					break;
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::HandleMouseUp(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!_global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = _global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!_global_mouse_hooks[ix]->OnGlobalMouseUp(this, pos, button))
				return;
	}

	if (_mouse_capture_control != nullptr && _mouse_capture_button == button)
	{
		Float2 cpos = _mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseUp, _mouse_capture_control, cpos, pos, button, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			if (_mouse_capture_control->OnMouseUp(cpos, pos, button))
			{
				if ((button == MouseButton::Left && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
					(button == MouseButton::Right && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
				{
					ReleaseMouseCapture();
				}

				HandleMouseMove(pos);
			}
		}
		return;
	}

	Array<FudgetControl*> controls_for_input;
	ControlsAtPosition(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, FudgetControlFlags::None, FudgetControlFlags::CompoundControl, controls_for_input);
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
					HandleMouseMove(pos);
					break;
				}
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::HandleMouseDoubleClick(const Float2 &__pos, MouseButton button)
{
	Float2 pos = Input::GetMousePosition();

	if (!_global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = _global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!_global_mouse_hooks[ix]->OnGlobalMouseDoubleClick(this, pos, button))
				return;
	}

	if (_mouse_capture_control != nullptr && _mouse_capture_button == button)
	{
		LOG(Error, "MouseCapture not null on OnMouseDoubleClick");
		ReleaseMouseCapture();
		return;
	}

	if (_mouse_capture_control == nullptr)
	{
		_mouse_capture_button = button;
	}
	else
	{
		Float2 cpos = _mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, _mouse_capture_control, cpos, pos, button, true);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			FudgetInputResult result = _mouse_capture_control->OnMouseDown(cpos, pos, button, true);
			if (result == FudgetInputResult::Consume)
			{
				if ((button == MouseButton::Left && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
					(button == MouseButton::Right && _mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
					_mouse_capture_control->SetFocused(true);
			}
		}
		return;
	}

	Array<FudgetControl*> controls_for_input;
	ControlsAtPosition(pos, FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::BlockMouseEvents, FudgetControlFlags::None, FudgetControlFlags::CompoundControl, controls_for_input);

	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseUpDown))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos, pos))
			{
				FudgetMouseHookResult hook_result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, true);
				if (hook_result == FudgetMouseHookResult::EatEvent)
					return;
				if (hook_result == FudgetMouseHookResult::SkipControl)
					continue;

				FudgetInputResult result = c->OnMouseDown(cpos, pos, button, true);
				if (result == FudgetInputResult::Consume)
				{
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
						c->CaptureMouseInput();
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
						c->SetFocused(true);
				}

				if (result != FudgetInputResult::PassThrough || _mouse_capture_control != nullptr)
				{
					HandleMouseMove(pos);
					break;
				}
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
}

void FudgetGUIRoot::HandleMouseMove(const Float2 &__pos)
{
	Float2 pos = Input::GetMousePosition();

	if (!_global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = _global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!_global_mouse_hooks[ix]->OnGlobalMouseMove(this, pos))
				return;
	}

	if (_mouse_capture_control != nullptr)
	{
		Float2 cpos = _mouse_capture_control->GlobalToLocal(pos);
		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseMove, _mouse_capture_control, cpos, pos, MouseButton::None, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
			_mouse_capture_control->OnMouseMove(cpos, pos);
		return;
	}

	Array<FudgetControl*> controls_for_input;

	ControlsAtPosition(pos, FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::BlockMouseEvents, FudgetControlFlags::None, FudgetControlFlags::CompoundControl, controls_for_input);

	bool post_leave = _mouse_over_control != nullptr;
	for (int ix = controls_for_input.Count() - 1; ix >= 0; --ix)
	{
		FudgetControl *c = controls_for_input[ix];
		if (c->HasAnyFlag(FudgetControlFlags::CanHandleMouseMove))
		{
			Float2 cpos = c->GlobalToLocal(pos);
			if (c->WantsMouseEventAtPos(cpos, pos))
			{
				if (_mouse_over_control != c)
				{
					bool wants_enterleave = c->HasAnyFlag(FudgetControlFlags::CanHandleMouseEnterLeave);
					FudgetControl *old_mouse_control = _mouse_over_control;
					// Make sure the old control can check which is the new one.
					_mouse_over_control = wants_enterleave ? c : nullptr;

					if (old_mouse_control != nullptr)
					{
						FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, c, Float2(), Float2(), MouseButton::None, false);
						if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
							old_mouse_control->DoMouseLeave();
					}

					if (wants_enterleave)
					{
						// Let the new control check where the mouse came from
						_mouse_over_control = old_mouse_control;

						FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseEnter, c, cpos, pos, MouseButton::None, false);
						if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
							c->DoMouseEnter(cpos, pos);
						_mouse_over_control = c;
					}
				}

				FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseMove, c, cpos, pos, MouseButton::None, false);
				if (result == FudgetMouseHookResult::EatEvent)
					return;
				if (result == FudgetMouseHookResult::SkipControl)
					continue;

				c->OnMouseMove(cpos, pos);
				UpdateCursor(c);
				return;
			}
		}
		if (c->HasAnyFlag(FudgetControlFlags::BlockMouseEvents))
			break;
	}
	if (post_leave)
	{
		FudgetControl *old_mouse_control = _mouse_over_control;
		// Make sure the control can check that no control has the mouse
		_mouse_over_control = nullptr;

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, old_mouse_control, Float2(), Float2(), MouseButton::None, false);
		if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
			old_mouse_control->DoMouseLeave();
	}
	UpdateCursor(_mouse_over_control);
}

void FudgetGUIRoot::HandleMouseLeave()
{
	if (!_global_mouse_hooks.IsEmpty())
	{
		for (int ix = 0, siz = _global_mouse_hooks.Count(); ix < siz; ++ix)
			if (!_global_mouse_hooks[ix]->OnGlobalMouseLeave(this))
				return;
	}

	if (_mouse_capture_control != nullptr)
	{
#if !USE_EDITOR
		// MouseLeave is sent in editor for floating game window when moving mouse after button down. TODO: test in cooked game.

		LOG(Error, "MouseCapture not null on DoMouseLeave");
		ReleaseMouseCapture();
#endif
		return;
	}

	if (_mouse_over_control != nullptr)
	{
		FudgetControl *old_mouse_control = _mouse_over_control;
		// Make sure the control can check that no control has the mouse
		_mouse_over_control = nullptr;

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseLeave, old_mouse_control, Float2(), Float2(), MouseButton::None, false);
		if (result != FudgetMouseHookResult::EatEvent && result != FudgetMouseHookResult::SkipControl)
			old_mouse_control->DoMouseLeave();
	}
	UpdateCursor(_mouse_over_control);
}

void FudgetGUIRoot::HandleKeyDown(KeyboardKeys key)
{
	FudgetControl *c = FindKeyboardInputControl(key);
	while (c != nullptr)
	{
		if (IsNavigationKey(key) && (!c->HasAnyFlag(FudgetControlFlags::CanHandleNavigationKeys) || !c->WantsNavigationKey(key) ))
		{
			NavigateWithKey(key);
			break;
		}

		_focus_control_keys.Add(key);
		FudgetInputResult result = c->OnKeyDown(key);
		if (result == FudgetInputResult::PassThrough)
		{
			c = c->GetParent();
			_focus_control_keys.Remove(key);
		}
		else
		{
			if (result == FudgetInputResult::Ignore && IsNavigationKey(key))
				NavigateWithKey(key);
			break;
		}
	}
}

void FudgetGUIRoot::HandleKeyUp(KeyboardKeys key)
{
	if (!_focus_control_keys.Contains(key))
		return;

	FudgetControl *c = FindKeyboardInputControl(key);
	while (c != nullptr)
	{
		if (!c->OnKeyUp(key))
			c = c->GetParent();
		else
			break;
	}
	_focus_control_keys.Remove(key);
}

void FudgetGUIRoot::HandleCharInput(Char ch)
{
	if (ch == 127)
		return;

	FudgetControl *c = FindKeyboardInputControl(KeyboardKeys::None);
	while (c != nullptr)
	{
		FudgetInputResult result = c->OnCharInput(ch);
		if (result == FudgetInputResult::PassThrough)
		{
			c = c->GetParent();
		}
		else
		{
			if (result == FudgetInputResult::Ignore)
			{
				// TODO: forward the input to the game
			}
			break;
		}
	}
}

FudgetControl* FudgetGUIRoot::FindKeyboardInputControl(KeyboardKeys key) const
{
	FudgetControl *c = _focus_control;
	while (c != nullptr)
	{
		if (!c->HasAnyFlag(FudgetControlFlags::CanHandleKeyEvents))
		{
			c = c->GetParent();
			continue;
		}
		else
		{
			break;
		}
	}
	return c;
}



