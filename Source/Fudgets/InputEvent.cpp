#include "InputEvent.h"
#include "Engine/Input/Input.h"


FudgetInputEvent::FudgetInputEvent() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), bound(false)
{
    Input::ActionTriggered.Bind<FudgetInputEvent, &FudgetInputEvent::Handler>(this);
    bound = true;
}


FudgetInputEvent::FudgetInputEvent(String name) : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), Name(name), bound(false)
{
    Input::ActionTriggered.Bind<FudgetInputEvent, &FudgetInputEvent::Handler>(this);
    bound = true;
}

FudgetInputEvent::~FudgetInputEvent()
{
    if (bound)
        Input::ActionTriggered.Unbind<FudgetInputEvent, &FudgetInputEvent::Handler>(this);
    bound = false;
}


bool FudgetInputEvent::Active()
{
    return Input::GetAction(Name);
}

InputActionState FudgetInputEvent::GetState() const
{
    return Input::GetActionState(Name);
}

void FudgetInputEvent::Dispose()
{
    if (bound)
        Input::ActionTriggered.Unbind<FudgetInputEvent, &FudgetInputEvent::Handler>(this);
    bound = false;
    DeleteObject();
    //GC.SuppressFinalize(this);
}

void FudgetInputEvent::Handler(StringView name, InputActionState state)
{
    if (!StringUtils::CompareIgnoreCase(*name, *Name))
        return;
    switch (state)
    {
        case InputActionState::None: break;
        case InputActionState::Waiting: break;
        case InputActionState::Pressing:
        {
            if (Pressing.IsBinded())
                Pressing();
            break;
        }
        case InputActionState::Press:
        {
            if (Pressed.IsBinded())
                Pressed();
            break;
        }
        case InputActionState::Release:
        {
            if (Released.IsBinded())
                Released();
            break;
        }
        default: break;
    }
}

