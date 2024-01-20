#pragma once

#include "Engine/Scripting/ScriptingType.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Input/Enums.h"

class FudgetGUIRoot;


// TODO: see if there's a way to make the events in the hook correspond to what controls see.

/// <summary>
/// Interface class to register as a hook, that will receive and get the ability to modify raw mouse events.
/// </summary>
API_INTERFACE() class FUDGETS_API IFudgetMouseHook
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(IFudgetMouseHook);
public:
    /// <summary>
    /// Cleans up on destruction
    /// </summary>
    virtual ~IFudgetMouseHook() = default;

    /// <summary>
    /// Receives OnMouseDown events ahead of controls. It is allowed to modify them, but be consistent with the
    /// other function calls. It can result in erratic and buggy behavior if the different functions are not
    /// in sync.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just pressed</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnMouseDown(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives OnMouseUp events ahead of controls. It is allowed to modify them, but be consistent with the
    /// other function calls. It can result in erratic and buggy behavior if the different functions are not
    /// in sync. This function is called even if OnMouseDown stopped that event.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just released</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnMouseUp(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives OnMouseDoubleClick events ahead of controls. It is allowed to modify them, but be consistent
    /// with the other function calls. It can result in erratic and buggy behavior if the different functions
    /// are not in sync. This function is called even if OnMouseDown stopped that event.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just double clicked</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnMouseDoubleClick(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives OnMouseMove events ahead of controls. This is also OnMouseEnter and in many cases OnMouseLeave,
    /// since those events are only virtual, and are created by the GUI root.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnMouseMove(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos) = 0;

    /// <summary>
    /// Receives OnMouseLeave events ahead of controls. This is often not the same as the OnMouseLeave that
    /// other controls receive, as this is sent by the window manager. It's called when the mouse pointer leaves
    /// the window.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnMouseLeave(FudgetGUIRoot *guiRoot) = 0;
};

