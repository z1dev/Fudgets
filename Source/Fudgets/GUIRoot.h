#pragma once

#include "Container.h"

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

    FudgetControl *mouse_capture_control;
    MouseButton mouse_capture_button;
    FudgetControl *mouse_over_control;

	// The actor on the scene forwarding all the events to the root.
    Fudget *_root;
    WindowBase *_window;

    bool events_initialized;
};

