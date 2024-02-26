#pragma once

#include "../Control.h"
#include "../Styling/Token.h"
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

protected:
    /// <summary>
    /// Gets the pressed state of the button. The button's state is pressed if a mouse button was pressed over the button and the mouse
    /// pointer is over the button.
    /// </summary>
    /// <param name="value">The new pressed state</param>
    API_PROPERTY() void SetPressed(bool value);

    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

private:
    // Button is in a state where it's shown pressed
    bool _down;
    // Mouse button is down over the button
    bool _pressed;
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
    void OnUpdate(float delta_time) override;
    /// <inheritdoc />
    void OnDraw() override;
    /// <inheritdoc />
    void OnPressedChanged() override;
    /// <inheritdoc />
    void OnDownChanged() override;
    /// <inheritdoc />
    void OnFocusChanged(bool focused, FudgetControl *other) override;
    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void OnMouseEnter(Float2 pos, Float2 global_pos) override;
    /// <inheritdoc />
    void OnMouseLeave() override;
    /// <inheritdoc />
    void OnVirtuallyEnabledChanged() override;

    /// <summary>
    /// Class Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetClassToken();
    /// <summary>
    /// Background Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetBackgroundToken();
    /// <summary>
    /// Pressed Background Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetPressedBackgroundToken();
    /// <summary>
    /// Disabled Background Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetDisabledBackgroundToken();
    /// <summary>
    /// Frame Painter Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    /// <summary>
    /// Frame Style Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    /// <summary>
    /// Content Painter Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetContentPainterToken();
    /// <summary>
    /// Content Style Token
    /// </summary>
    API_PROPERTY() static FudgetToken GetContentStyleToken();
protected:
    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;
private:
    static void InitializeTokens();

    static FudgetToken ClassToken;
    static FudgetToken BackgroundToken;
    static FudgetToken PressedBackgroundToken;
    static FudgetToken DisabledBackgroundToken;
    static FudgetToken FramePainterToken;
    static FudgetToken FrameStyleToken;
    static FudgetToken ContentPainterToken;
    static FudgetToken ContentStyleToken;

    FudgetPadding FudgetButton::GetInnerPadding() const;

    FudgetPainterStateHelper _draw_state;
    FudgetFramedFieldPainter *_frame_painter;
    FudgetStatePainter *_content_painter;
};


