#include "GUIRoot.h"
#include "Fudget.h"
#include "IFudgetMouseHook.h"

#include "Engine/Input/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Engine/Screen.h"
#include "Engine/Scripting/Scripting.h"
#include "Engine/Platform/Base/WindowBase.h"

#include "Engine/Core/Log.h"
#include <Engine/Serialization/JsonTools.h>
#include <Engine/Serialization/JsonWriters.h>

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

void FudgetGUIRoot::SetFocusedControl(FudgetControl *value)
{
	if (focus_control == value)
		return;
	focus_control = value;
}

String FudgetGUIRoot::SerializationTester()
{
	String output;
	rapidjson_flax::StringBuffer buffer;
	PrettyJsonWriter writerObj(buffer);
	JsonWriter& writer = writerObj;
	writer.StartObject();
	Serialize(writer, nullptr);
	writer.EndObject();

	output = buffer.GetString();
	return output;
}

void GetAllControls(FudgetContainer* container, Array<FudgetControl*>& out)
{
	for (int i = 0; i < container->GetChildCount(); i++)
	{
		FudgetControl* child = container->ChildAt(i);
		out.Add(child);

		if (child->Is<FudgetContainer>())
			GetAllControls((FudgetContainer*)child, out);
	}
}

void FudgetGUIRoot::Serialize(SerializeStream& stream, const void* otherObj)
{
	Array<FudgetControl*> allControls;
	GetAllControls(this, allControls);

	Base::Serialize(stream, otherObj);
	stream.JKEY("Controls");
	stream.StartArray();
	for (auto* control : allControls)
	{
		stream.StartObject();
		control->Serialize(stream, nullptr);
		stream.EndObject();
	}
	stream.EndArray();
}

FudgetControl* ControlFactory(ISerializable::DeserializeStream& stream, ISerializeModifier* modifier)
{
	Guid id = JsonTools::GetGuid(stream, "ID");
	modifier->IdsMapping.TryGet(id, id);
	if (!id.IsValid())
	{
		LOG(Warning, "Invalid object id.");
		return nullptr;
	}
	FudgetControl* obj = nullptr;

	const auto typeNameMember = stream.FindMember("TypeName");
	if (typeNameMember != stream.MemberEnd())
	{
		if (!typeNameMember->value.IsString())
		{
			LOG(Warning, "Invalid object type (TypeName must be an object type full name string).");
			return nullptr;
		}
		const StringAnsiView typeName(typeNameMember->value.GetStringAnsiView());
		const ScriptingTypeHandle type = Scripting::FindScriptingType(typeName);
		if (type)
		{
			if (!FudgetControl::TypeInitializer.IsAssignableFrom(type))
			{
				LOG(Warning, "Invalid control type {0} (inherits from: {1}).", type.ToString(true), type.GetType().GetBaseType().ToString());
				return nullptr;
			}
			const ScriptingObjectSpawnParams params(id, type);
			obj = (FudgetControl*)type.GetType().Script.Spawn(params);

			if (obj == nullptr)
			{
				LOG(Warning, "Failed to spawn object of type {0}.", type.ToString(true));
				return nullptr;
			}
		}
		else
		{
			LOG(Warning, "Unknown object type '{0}', ID: {1}", String(typeName.Get(), typeName.Length()), id.ToString());
			return nullptr;
		}
	}
	else
	{
		LOG(Warning, "Invalid object type.");
	}

	return obj;
}

void FudgetGUIRoot::Deserialize(DeserializeStream& stream, ISerializeModifier* modifier)
{
	Base::Deserialize(stream, modifier);
	int32 controlsCount = 0;
	const auto& controlsListMember = stream.FindMember("Controls");
	if (controlsListMember != stream.MemberEnd() && controlsListMember->value.IsArray())
	{
		controlsCount = controlsListMember->value.Size();
	}

	Array<FudgetControl*> controls;
	if (controlsCount)
	{
		const DeserializeStream& items = controlsListMember->value;
		controls.Resize(controlsCount, false);
		for (int32 i = 0; i < controlsCount; i++)
		{
			FudgetControl* control = ControlFactory((DeserializeStream&)items[i], modifier);
			if (control == nullptr)
			{
				// Skip
				continue;
			}

			control->Deserialize((DeserializeStream&)items[i], modifier);
		}
	}
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
	{
		mouse_capture_button = button;
	}
	else
	{
		Float2 cpos = mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, mouse_capture_control, cpos, pos, button, false);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			FudgetMouseButtonResult result = mouse_capture_control->OnMouseDown(cpos, pos, button, false);
			if (result == FudgetMouseButtonResult::Consume)
			{
				if ((button == MouseButton::Left && mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
					(button == MouseButton::Right && mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
					mouse_capture_control->SetFocused(true);
			}
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
				if (!local_mouse_hooks.IsEmpty())
				{
					FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, false);
					if (result == FudgetMouseHookResult::EatEvent)
						return;
					if (result == FudgetMouseHookResult::SkipControl)
						continue;
				}

				FudgetMouseButtonResult result = c->OnMouseDown(cpos, pos, button, false);
				if (result == FudgetMouseButtonResult::Consume)
				{
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
						c->CaptureMouseInput();
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
						c->SetFocused(true);
				}

				if (result != FudgetMouseButtonResult::PassThrough || mouse_capture_control != nullptr)
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
			if (mouse_capture_control->OnMouseUp(cpos, pos, button))
			{
				if ((button == MouseButton::Left && mouse_capture_control->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
					(button == MouseButton::Right && mouse_capture_control->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
				{
					ReleaseMouseCapture();
				}

				OnMouseMove(pos);
			}
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

	if (mouse_capture_control == nullptr)
	{
		mouse_capture_button = button;
	}
	else
	{
		Float2 cpos = mouse_capture_control->GlobalToLocal(pos);

		FudgetMouseHookResult result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, mouse_capture_control, cpos, pos, button, true);
		if (result == FudgetMouseHookResult::EatEvent)
			return;
		if (result != FudgetMouseHookResult::SkipControl)
		{
			FudgetMouseButtonResult result = mouse_capture_control->OnMouseDown(cpos, pos, button, true);
			if (result == FudgetMouseButtonResult::Consume)
			{
				if ((button == MouseButton::Left && mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
					(button == MouseButton::Right && mouse_capture_control->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
					mouse_capture_control->SetFocused(true);
			}
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
				FudgetMouseHookResult hook_result = ProcessLocalMouseHooks(HookProcessingType::MouseDown, c, cpos, pos, button, true);
				if (hook_result == FudgetMouseHookResult::EatEvent)
					return;
				if (hook_result == FudgetMouseHookResult::SkipControl)
					continue;

				FudgetMouseButtonResult result = c->OnMouseDown(cpos, pos, button, true);
				if (result == FudgetMouseButtonResult::Consume)
				{
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::CaptureReleaseMouseRight)))
						c->CaptureMouseInput();
					if ((button == MouseButton::Left && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseLeft)) ||
						(button == MouseButton::Right && c->HasAnyFlag(FudgetControlFlags::FocusOnMouseRight)))
						c->SetFocused(true);
				}

				if (result != FudgetMouseButtonResult::PassThrough || mouse_capture_control != nullptr)
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

