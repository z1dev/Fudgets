#pragma once

#include "Engine/Scripting/ScriptingType.h"
#include "Engine/Core/Math/Vector2.h"
#include "Engine/Input/Enums.h"

class FudgetGUIRoot;

/// <summary>
/// The types of mouse hooks that can be registered to monitor mouse events in the UI
/// </summary>
API_ENUM()
enum class FudgetMouseHookType
{
    /// <summary>
    /// Use for mouse hooks that implement the OnGlobal*** events to inspect or modify mouse events before they
    /// are processed by the UI.
    /// </summary>
    Global,
    /// <summary>
    /// Use for mouse hooks that implement the OnLocal*** events to inspect mouse events before they are sent
    /// to each control, or to stop that from happening.
    /// </summary>
    Local,
    /// <summary>
    /// For mouse hooks that want to handle both global and local mouse events.
    /// </summary>
    Both
};


/// <summary>
/// The result of mouse event handling by mouse hooks, that determines what happens to the event
/// </summary>
API_ENUM()
enum class FudgetMouseHookResult
{
    /// <summary>
    /// Let the mouse event continue to be inspected by other hooks and then delivered to the control
    /// </summary>
    NormalProcessing,
    /// <summary>
    /// Stops all further handling of the event in the UI and not letting other hooks or controls use it
    /// </summary>
    EatEvent,
    /// <summary>
    /// Stops further handling of the event for the current control. The control will be skipped, but controls
    /// below will be allowed to see it. Before the next control can do that, the hook will have an opportunity
    /// to make a decision again
    /// </summary>
    SkipControl,
    /// <summary>
    /// Don't interfere with event handling, but stop hooks that were registered later from looking at it
    /// </summary>
    SkipHooks
};


/// <summary>
/// Interface class to register as a hook, that will receive and get the ability to modify raw mouse events.
/// </summary>
API_INTERFACE() class FUDGETS_API IFudgetMouseHook
{
    DECLARE_SCRIPTING_TYPE_MINIMAL(IFudgetMouseHook);
public:
    /// <summary>
    /// Receives the global OnMouseDown event coming from Input. It is allowed to modify it, but should be
    /// consistent with the other functions.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just pressed</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnGlobalMouseDown(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives the global OnMouseUp event coming from Input. It is allowed to modify it, but should be
    /// consistent with the other functions.
    /// This function is called even if OnMouseDown stopped that event.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just released</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnGlobalMouseUp(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives the global OnMouseDoubleClick event coming from Input. It is allowed to modify it, but should
    /// be consistent with the other functions
    /// This function is called even if OnMouseDown stopped that event.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <param name="button">Button just double clicked</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnGlobalMouseDoubleClick(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos, API_PARAM(Ref) MouseButton &button) = 0;

    /// <summary>
    /// Receives the global OnMouseMove event coming from Input. Take into consideration that the control
    /// version of OnMouseEnter and OnMouseLeave events are invented by the UI on mouse move too.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="pos">Position of mouse relative to the upper left corner of the game screen</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnGlobalMouseMove(FudgetGUIRoot *guiRoot, API_PARAM(Ref) Float2 &pos) = 0;

    /// <summary>
    /// Receives the global OnMouseLeave event coming fron Input. This event doesn't match the controls version
    /// of OnMouseLeave. It is called when the mouse pointer leaves the window.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <returns>Whether handling of the mouse event should continue. False "eats" the event.</returns>
    API_FUNCTION() virtual bool OnGlobalMouseLeave(FudgetGUIRoot *guiRoot) = 0;

    /// <summary>
    /// Called when a mouse button was pressed over a control and just before the control can handle it.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="control">The control that will receive this event</param>
    /// <param name="pos">Local mouse position for the control</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just pressed</param>
    /// <param name="double_click">Whether the call is the result of double clicking</param>
    /// <returns>How to process the event, for example to prevent it reaching the control</returns>
    API_FUNCTION() virtual FudgetMouseHookResult OnLocalMouseDown(FudgetGUIRoot *guiRoot, FudgetControl *control, Float2 pos, Float2 global_pos, MouseButton button, bool double_click) = 0;

    /// <summary>
    /// Called when a mouse button was released over a control or for a control that was capturing the mouse,
    /// and just before the control can handle it.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="control">The control that will receive this event</param>
    /// <param name="pos">Local mouse position for the control</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <param name="button">Button that was just released</param>
    /// <returns>How to process the event, for example to prevent it reaching the control</returns>
    API_FUNCTION() virtual FudgetMouseHookResult OnLocalMouseUp(FudgetGUIRoot *guiRoot, FudgetControl *control, Float2 pos, Float2 global_pos, MouseButton button) = 0;

    /// <summary>
    /// Called when the mouse pointer was moved on a control, or a control was capturing the mouse,
    /// and just before the control can handle it.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="control">The control that will receive this event</param>
    /// <param name="pos">Local mouse position for the control</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <returns>How to process the event, for example to prevent it reaching the control</returns>
    API_FUNCTION() virtual FudgetMouseHookResult OnLocalMouseMove(FudgetGUIRoot *guiRoot, FudgetControl *control, Float2 pos, Float2 global_pos) = 0;

    /// <summary>
    /// Called when the mouse pointer entered a control and just before the control can handle it. It's not called
    /// while a control is capturing the mouse.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="control">The control that will receive this event</param>
    /// <param name="pos">Local mouse position for the control</param>
    /// <param name="global_pos">Global mouse position</param>
    /// <returns>How to process the event, for example to prevent it reaching the control</returns>
    API_FUNCTION() virtual FudgetMouseHookResult OnLocalMouseEnter(FudgetGUIRoot *guiRoot, FudgetControl *control, Float2 pos, Float2 global_pos) = 0;

    /// <summary>
    /// Called when the mouse pointer left a control and just before the control can handle it. It is also called
    /// if a control was capturing the mouse but the capture ended while the pointer was not over this control.
    /// </summary>
    /// <param name="guiRoot">The root container of the user interface, forwarding this event</param>
    /// <param name="control">The control that will receive this event</param>
    /// <returns>How to process the event, for example to prevent it reaching the control</returns>
    API_FUNCTION() virtual FudgetMouseHookResult OnLocalMouseLeave(FudgetGUIRoot *guiRoot, FudgetControl *control) = 0;

};

