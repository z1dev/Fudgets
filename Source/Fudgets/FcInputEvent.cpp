#include "FcInputEvent.h"
#include "Engine/Input/Input.h"


FcInputEvent::FcInputEvent() : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), bound(false)
{
    Input::ActionTriggered.Bind<FcInputEvent, &FcInputEvent::Handler>(this);
    bound = true;
}


FcInputEvent::FcInputEvent(String name) : ScriptingObject(SpawnParams(Guid::New(), TypeInitializer)), Name(name), bound(false)
{
    Input::ActionTriggered.Bind<FcInputEvent, &FcInputEvent::Handler>(this);
    bound = true;
}

FcInputEvent::~FcInputEvent()
{
    if (bound)
        Input::ActionTriggered.Unbind<FcInputEvent, &FcInputEvent::Handler>(this);
    bound = false;
}


bool FcInputEvent::Active()
{
    return Input::GetAction(Name);
}

InputActionState FcInputEvent::GetState() const
{
    return Input::GetActionState(Name);
}

void FcInputEvent::Dispose()
{
    if (bound)
        Input::ActionTriggered.Unbind<FcInputEvent, &FcInputEvent::Handler>(this);
    bound = false;
    DeleteObject();
    //GC.SuppressFinalize(this);
}

void FcInputEvent::Handler(StringView name, InputActionState state)
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

