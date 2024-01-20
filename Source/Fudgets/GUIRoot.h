#pragma once

#include "Container.h"
#include "IFudgetMouseHook.h"

class WindowBase;

/// <summary>
/// Root container representing the whole area where UI controls can appear. For example the screen.
/// </summary>
API_CLASS(NoSpawn, Hidden)
class FUDGETS_API FudgetGUIRoot : public FudgetContainer
{
    using Base = FudgetContainer;
	DECLARE_SCRIPTING_TYPE_NO_SPAWN(FudgetGUIRoot);
public:
    FudgetGUIRoot();
    FudgetGUIRoot(Fudget *root);

    ~FudgetGUIRoot();

    /// <inheritdoc />
    Float2 GetSize() const override;
    /// <inheritdoc />
    Float2 GetHintSize() const override;
    /// <inheritdoc />
    Float2 GetMinSize() const override;
    /// <inheritdoc />
    Float2 GetMaxSize() const override;

    /// <summary>
    /// Registers a class to receive and modify mouse events before they are handle by anything else
    /// in the UI. If multiple hooks are installed, they are notified of events in their order of
    /// registration. If a hook modifies an event, it is passed on to the next hook in its modified state.
    /// Don't forget to UnregisterMouseHook cleanup, because it is not done automatically.
    /// </summary>
    /// <param name="hook">The mouse hook to register</param>
    API_FUNCTION() static void RegisterMouseHook(IFudgetMouseHook *hook);

    /// <summary>
    /// Unregisters a class as mouse event hook after it was registered with RegisterMouseHook. Always
    /// call when hook is no longer necessary
    /// </summary>
    /// <param name="hook">The mouse hook to unregister</param>
    API_FUNCTION() static void UnregisterMouseHook(IFudgetMouseHook *hook);

    /// <summary>
    /// Call in an OnMouseDown function of a control to direct future mouse events to it until
    /// ReleaseMouseCapture is called. Make sure to call ReleaseMouseCapture() on OnMouseUp for
    /// the same control. Mouse can only be captured for a single button.
    /// If a different control captured the mouse, it'll be released first.
    /// </summary>
    /// <param name="control"></param>
    API_FUNCTION() void StartMouseCapture(FudgetControl *control);

    /// <summary>
    /// Call on an OnMouseUp event when mouse capturing was started on the same control for the same
    /// button. It can be called any other time too but it might disrupt mouse handling for the control
    /// that captured the mouse, if it's not prepared for this.
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() void ReleaseMouseCapture();
private:

    void InitializeEvents();
    void UninitializeEvents();

    // Mouse and Touch input:

    void OnMouseDown(const Float2 &pos, MouseButton button);
    void OnMouseUp(const Float2 &pos, MouseButton button);
    void OnMouseDoubleClick(const Float2 &pos, MouseButton button);
    void OnMouseMove(const Float2 &pos);
    void OnMouseLeave();

    // Used for checking if this class has initialized events with Input.
    bool events_initialized;

    // Control currently capturing th emouse.
    FudgetControl *mouse_capture_control;
    // Which button was pressed before the mouse was captured. This is updated independent of
    // capture status, but is not used when nothing captures it.
    MouseButton mouse_capture_button;
    // The control currently (or last time it was checked) under the mouse pointer. It's not updated
    // if something has captured the mouse already.
    FudgetControl *mouse_over_control;

	// The actor on the scene forwarding all the events to the root
    Fudget *_root;
    // The game window, needed to actually capture the mouse
    WindowBase *_window;

    // Interface objects that were registered to inspect or modify mouse events
    static Array<IFudgetMouseHook*> mouse_hooks;
};

