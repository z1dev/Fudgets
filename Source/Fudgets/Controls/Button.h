#pragma once

#include "../Control.h"
#include "../Styling/PartPainters.h"

/// <summary>
/// Base class of buttons which handles user input. Derived classes should draw the button and the contents.
/// </summary>
API_CLASS(Abstract)
class FUDGETS_API FudgetButtonBase : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetButtonBase);
public:
    /// <summary>
    /// Gets the down state of the button. The button's state is down when it was pressed and stays pressed without user interaction.
    /// </summary>
    /// <returns>Whether the button's state is down</returns>
    API_PROPERTY() FORCE_INLINE bool GetDown() const { return _down; }
    /// <summary>
    /// Sets the down state of the button. The button's state is down when it was pressed and stays pressed without user interaction.
    /// </summary>
    /// <param name="value">The new down state</param>
    API_PROPERTY() void SetDown(bool value);

    /// <summary>
    /// Gets the pressed state of the button. The button's state is pressed if a mouse button was pressed over the button and the mouse
    /// pointer is over the button.
    /// </summary>
    /// <returns>Whether the button's state is pressed</returns>
    API_PROPERTY() FORCE_INLINE bool GetPressed() const { return _pressed; }

    /// <summary>
    /// Called when the button's down state was set or unset
    /// </summary>
    /// <returns></returns>
    API_FUNCTION() virtual void OnDownChanged() {}
    /// <summary>
    /// Called when the button's pressed state was set or unset
    /// </summary>
    API_FUNCTION() virtual void OnPressedChanged() {}

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;


    API_FIELD() Action EventPressed;
protected:
    /// <summary>
    /// Called when the button's pressed state was set or unset. Override OnPressedChanged in derived classes.
    /// </summary>
    API_FUNCTION() virtual void DoPressedChanged();

    /// <summary>
    /// Gets the pressed state of the button. The button's state is pressed if a mouse button was pressed over the button and the mouse
    /// pointer is over the button.
    /// </summary>
    /// <param name="value">The new pressed state</param>
    API_PROPERTY() void SetPressed(bool value);

    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;

private:
    // Button is in a state where it's shown pressed
    bool _down;
    // Mouse button is down over the button
    bool _pressed;
};


API_ENUM()
enum class FudgetButtonIds
{
    First = 1000,

    Background = First,
    PressedBackground,
    DisabledBackground,

    FramePainter,
    FrameStyle,

    ContentPainter,
    ContentStyle,
};

/// <summary>
/// A general button class which can draw anything inside its frame
/// </summary>
API_CLASS()
class FUDGETS_API FudgetButton : public FudgetButtonBase
{
    using Base = FudgetButtonBase;
    DECLARE_SCRIPTING_TYPE(FudgetButton);
public:
    /// <inheritdoc />
    void OnInitialize() override;
    /// <inheritdoc />
    void OnStyleInitialize() override;
    /// <inheritdoc />
    void OnUpdate(float delta_time) override;
    /// <inheritdoc />
    void OnDraw() override;
    /// <inheritdoc />
    void OnPressedChanged() override;
    /// <inheritdoc />
    void OnDownChanged() override;

    ///// <inheritdoc />
    //void OnFocusChanged(bool focused, FudgetControl *other) override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    ///// <inheritdoc />
    //void OnMouseEnter(Float2 pos, Float2 global_pos) override;
    ///// <inheritdoc />
    //void OnMouseLeave() override;

    ///// <inheritdoc />
    //void OnVirtuallyEnabledChanged() override;

protected:
    /// <inheritdoc />
    FudgetControlFlag GetInitFlags() const override;
private:
    FudgetPadding FudgetButton::GetInnerPadding() const;

    FudgetFramedFieldPainter *_frame_painter;
    FudgetStatePainter *_content_painter;
};


