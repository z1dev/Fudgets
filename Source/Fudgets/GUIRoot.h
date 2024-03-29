#pragma once

#include "Container.h"
#include "IFudgetMouseHook.h"

class WindowBase;

/// <summary>
/// Root container representing the whole area where UI controls can appear. For example the screen.
/// </summary>
API_CLASS(Attributes = "HideInEditor")
class FUDGETS_API FudgetGUIRoot : public FudgetContainer
{
    using Base = FudgetContainer;
	DECLARE_SCRIPTING_TYPE(FudgetGUIRoot);
public:
    FudgetGUIRoot(Fudget *root);
    FudgetGUIRoot(const SpawnParams &params, Fudget *root);

    ~FudgetGUIRoot();

    /// <summary>
    /// Called by the Fudget actor when it first initializes the gui.
    /// </summary>
    API_FUNCTION() void FudgetInit();

    /// <summary>
    /// Inserts a control into the layout of the gui root as a top-level control. If an index is provided,
    /// the controls with the same or higher index are moved one index higher. The control with the highest
    /// index is drawn last.
    /// The value of index has a different meaning based on whether the control has the AlwaysOnTop flag set at
    /// insertion or not. If it doesn't have the flag, it will be placed at most above the top not-AlwaysOnTop
    /// control. With the flag, the index of 0 will place the control at the bottom of the AlwaysOnTop controls,
    /// 1 as the second AlwaysOnTop control and so on. A negative value means the top of the corresponding group
    /// of controls.
    /// The AlwaysOnTop flag is added or removed to represent the control's current status, if the insertion
    /// changes it.
    /// The control's drawing and events will be managed by the container. The control's lifetime is also controlled
    /// by the container, destroying the control when the container is destroyed.
    /// </summary>
    /// <param name="control">The control to insert</param>
    /// <param name="index">The requested index of the control</param>
    /// <returns>The new index position of the inserted control or -1 on failure.</returns>
    int AddChild(FudgetControl *control, int index = -1) override;

    /// <inheritdoc />
    int RemoveChild(FudgetControl *control) override;

    /// <summary>
    /// Changes the index of a top-level control from one index position to the other, moving all
    /// controls between to a new index. The indexes are absolute values, including not-AlwaysOnTop and
    /// AlwaysOnTop controls.
    /// It's invalid to move a control that would change its AlwaysOnTop status.
    /// </summary>
    /// <param name="from">The starting index of the control</param>
    /// <param name="to">The index to move the control to</param>
    /// <returns>Returns whether the control's index was set to the target value</returns>
    bool MoveChildToIndex(int from, int to) override;

    /// <summary>
    /// Changes a top-level control's always on top status, moving it from its old group to the index position
    /// in the new one. Set index to negative to place it above the rest of the group.
    /// </summary>
    /// <param name="control">The control to change to or from always-on-top status</param>
    /// <param name="set_always_on_top">Whether the control should be added to the always on top group or removed from it</param>
    /// <param name="index">Position of the control among its group (on top or not on top)</param>
    /// <returns>The new absolute index of the control or -1 on failure</returns>
    API_FUNCTION() int ChangeControlAlwaysOnTop(FudgetControl *control, bool set_always_on_top, int index = -1);

    /// <inheritdoc />
    Int2 GetSize() const override;
    /// <inheritdoc />
    Int2 GetHintSize() const override;
    /// <inheritdoc />
    Int2 GetMinSize() const override;
    /// <inheritdoc />
    Int2 GetMaxSize() const override;

    /// <summary>
    /// Returns the Fudget actor that uses this container for its root.
    /// </summary>
    API_FUNCTION() Fudget* GetRoot() const { return _root; }

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
    API_PROPERTY() FudgetControl* GetMouseCaptureControl() const { return _mouse_capture_control; }

    /// <summary>
    /// Returns the control currently focused, that will receive keyboard events
    /// </summary>
    API_PROPERTY() FudgetControl* GetFocusedControl() const { return _focus_control; }

    /// <summary>
    /// Sets which control should be focused and receive keyboard events
    /// </summary>
    API_PROPERTY() void SetFocusedControl(FudgetControl *value);

    /// <summary>
    /// Returns the control under the mouse pointer during the last mouse event
    /// </summary>
    API_PROPERTY() FudgetControl* GetHoveredControl() const { return _mouse_over_control; }

    /// <summary>
    /// When called for the hovered control, checks if the displayed cursor needs to be changed,
    /// and then changes it. Has no effect if the control doesn't have the mouse pointer.
    /// </summary>
    /// <param name="control">Control to check for updated cursor</param>
    API_FUNCTION() void UpdateCursor(FudgetControl *control);

    /// <summary>
    /// Registers or unregisters the control to receive the global update tick. Its OnUpdate will be called by the root.
    /// </summary>
    /// <param name="control">The control to register its update</param>
    /// <param name="value">True to register and false to unregister</param>
    /// <returns>Whether the registration or unregistration was a success</returns>
    API_FUNCTION() bool RegisterControlUpdate(FudgetControl *control, bool value);

    /// <summary>
    /// Registers or unregisters the control to receive the global update tick. Its OnUpdate will be called by the root.
    /// This function is safe to call during updates, as it will only change the list of controls when updating is done.
    /// Calling this function with both true and false in the same update is undefined behavior.
    /// </summary>
    /// <param name="control">The control to register its update</param>
    /// <param name="value">True to register and false to unregister</param>
    API_FUNCTION() void DelayRegisterControlUpdate(FudgetControl *control, bool value);

    /// <summary>
    /// Unregisters every control from receiving update ticks and removes self too.
    /// </summary>
    API_FUNCTION() void UnregisterControlUpdates();

    /// <summary>
    /// Returns whether the passed key is used for navigation (usually the arrows and tab)
    /// </summary>
    API_FUNCTION() bool IsNavigationKey(KeyboardKeys key) const;

    /// <summary>
    /// Function registered for getting notified when the UI size changes.
    /// TODO: only for screen space UI at the moment
    /// </summary>
    /// <param name="new_size"></param>
    /// <returns></returns>
    API_FUNCTION() void OnResized(Int2 new_size);

    /// <summary>
    /// Starts the layout of the whole control tree. Only controls with a dirty layout are affected.
    /// </summary>
    API_FUNCTION() void DoLayout();

    /// <summary>
    /// Callback event when the size of the GUI area changes
    /// </summary>
    API_EVENT() Delegate<Int2> ResizedEvent;

    /// <summary>
    /// Callback event when the focused control changed from an old control to a new one.
    /// Arguments are (old control, new control).
    /// </summary>
    API_EVENT() Delegate<FudgetControl*, FudgetControl*> FocusChangedEvent;
    /// <summary>
    /// Callback event when a control captured the mouse. Argument is the control gaining the
    /// mouse capture.
    /// </summary>
    API_EVENT() Delegate<FudgetControl*> MouseCapturedEvent;
    /// <summary>
    /// Callback event when a control lost the mouse capture. Argument is the control losing
    /// the mouse capture.
    /// </summary>
    API_EVENT() Delegate<FudgetControl*> MouseReleasedEvent;

    /// <summary>
    /// Called each frame if a control subscribed to it, before other things are handled,
    /// then all bindings are cleared.
    /// </summary>
    API_EVENT() Delegate<> NextFrameEvent;
private:
    //// Called once from the Fudget to initialize during gameplay
    //void Initialize() override;

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

    void ControlUpdates();

    // Mouse and Touch input:

    void HandleMouseDown(const Float2 &pos, MouseButton button);
    void HandleMouseDoubleClick(const Float2 &pos, MouseButton button);
    void DoHandleMouseDown(const Float2 &pos, MouseButton button, bool double_click);
    void HandleMouseUp(const Float2 &pos, MouseButton button);
    void HandleMouseMove(const Float2 &pos);
    void HandleMouseLeave();

    void HandleKeyDown(KeyboardKeys key);
    void HandleKeyUp(KeyboardKeys key);
    void HandleCharInput(Char ch);

    FudgetControl* FindKeyboardInputControl(KeyboardKeys key) const;

    void NavigateWithKey(KeyboardKeys key) const {}

    // Used for checking if this class has initialized events with Input.
    bool events_initialized;

    // The actor on the scene forwarding all the events to the root
    Fudget *_root;
    // The game window, needed to actually capture the mouse
    WindowBase *_window;

    // The number of top-level controls that are always placed on top of all other top-level controls. (Top-level being direct child to this root.)
    // Internally, they will work just like any other control apart from their placement. Controls that are added to the root which don't have
    // the AlwaysOnTop flag, will be placed below these controls, even if the passed index to AddChild is greater.
    int _on_top_count;

    // Control currently capturing the mouse.
    FudgetControl *_mouse_capture_control;
    // Which button was pressed before the mouse was captured. This is updated independent of
    // capture status, but is not used when nothing captures it.
    MouseButton _mouse_capture_button;
    // The control currently (or last time it was checked) under the mouse pointer. It's not updated
    // if something has captured the mouse already.
    FudgetControl *_mouse_over_control;
    // Whether the _mouse_capture_control started capturing the mouse because it had the auto capture flag.
    // Only call the release automatically when this is true.
    bool _auto_mouse_capture;


    // Interface objects that were registered to inspect or modify global mouse events
    Array<IFudgetMouseHook*> _global_mouse_hooks;
    // Interface objects that were registered to inspect or stop local mouse events
    Array<IFudgetMouseHook*> _local_mouse_hooks;

    // The control currently taking the keyboard input
    FudgetControl *_focus_control;
    // The keys that were pressed over the _focus_control but not released yet with OnKeyUp.
    HashSet<KeyboardKeys> _focus_control_keys;

    // Controls whose OnUpdate should be called
    Array<FudgetControl*> _updating_controls;
    // Will add these controls to _updating_controls after the update is done.
    Array<FudgetControl*> _controls_to_add_to_updating;
    // Will remove these controls from _updating_controls after the update is done.
    Array<FudgetControl*> _controls_to_remove_from_updating;
    // Forwarding the OnUpdate call to controls. During this call, adding or removing from _updating_controls
    // should be avoided. It will log a warning if it happens.
    bool _processing_updates;

    //friend class Fudget;
};

