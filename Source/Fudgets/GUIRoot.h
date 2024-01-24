#pragma once

#include "Container.h"
#include "IFudgetMouseHook.h"

class WindowBase;

/// <summary>
/// Root container representing the whole area where UI controls can appear. For example the screen.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetGUIRoot : public FudgetContainer
{
    using Base = FudgetContainer;
	DECLARE_SCRIPTING_TYPE(FudgetGUIRoot);
public:
    FudgetGUIRoot(Fudget *root);
    FudgetGUIRoot(const SpawnParams &params, Fudget *root);

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
    /// <param name="type">Whether to register the hook for global, local events or both</param>
    API_FUNCTION() void RegisterMouseHook(IFudgetMouseHook *hook, FudgetMouseHookType type);

    /// <summary>
    /// Unregisters a class as mouse event hook after it was registered with RegisterMouseHook. Always
    /// call when hook is no longer necessary
    /// </summary>
    /// <param name="hook">The mouse hook to unregister</param>
    /// <param name="type">Whether to unregister the hook for global, local events or both</param>
    API_FUNCTION() void UnregisterMouseHook(IFudgetMouseHook *hook, FudgetMouseHookType type);

    /// <summary>
    /// Call in an OnMouseDown function of a control to direct future mouse events to it until
    /// ReleaseMouseCapture is called. Make sure to call ReleaseMouseCapture on OnMouseUp for
    /// the same control. Mouse can only be captured for a single button.
    /// If a different control tries to capture the mouse, it'll be released first, before the
    /// other control starts capturing it.
    /// </summary>
    /// <param name="control">The control that will get mouse messages</param>
    API_FUNCTION() void StartMouseCapture(FudgetControl *control);

    /// <summary>
    /// Call on an OnMouseUp event when mouse capturing was started on the same control for the same
    /// button. It can be called any other time too but it might disrupt mouse handling for the control
    /// that captured the mouse, if it's not prepared for this.
    /// </summary>
    API_FUNCTION() void ReleaseMouseCapture();

    /// <summary>
    /// The control which is currently capturing the mouse on this UI
    /// </summary>
    API_PROPERTY() FudgetControl* GetMouseCaptureControl() const { return mouse_capture_control; }

    /// <summary>
    /// Returns the control currently focused, that will receive keyboard events
    /// </summary>
    API_PROPERTY() FudgetControl* GetFocusedControl() const { return focus_control; }

    /// <summary>
    /// Sets which control should be focused and receive keyboard events
    /// </summary>
    API_PROPERTY() void SetFocusedControl(FudgetControl *value);

    API_FUNCTION() String SerializationTester();

    void Serialize(SerializeStream& stream, const void* otherObj) override;
    void Deserialize(DeserializeStream& stream, ISerializeModifier* modifier) override;
private:
    enum class HookProcessingType
    {
        MouseDown,
        MouseUp,
        MouseMove,
        MouseEnter,
        MouseLeave
    };
    FudgetMouseHookResult ProcessLocalMouseHooks(HookProcessingType type, FudgetControl *control, Float2 pos, Float2 global_pos, MouseButton button, bool double_click);

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

    // The actor on the scene forwarding all the events to the root
    Fudget *_root;
    // The game window, needed to actually capture the mouse
    WindowBase *_window;

    // Control currently capturing th emouse.
    FudgetControl *mouse_capture_control;
    // Which button was pressed before the mouse was captured. This is updated independent of
    // capture status, but is not used when nothing captures it.
    MouseButton mouse_capture_button;
    // The control currently (or last time it was checked) under the mouse pointer. It's not updated
    // if something has captured the mouse already.
    FudgetControl *mouse_over_control;

    // Interface objects that were registered to inspect or modify global mouse events
    Array<IFudgetMouseHook*> global_mouse_hooks;
    // Interface objects that were registered to inspect or stop local mouse events
    Array<IFudgetMouseHook*> local_mouse_hooks;

    // The control currently taking the keyboard input
    FudgetControl *focus_control;
};

