#pragma once

#include "../Control.h"
#include "../Styling/Token.h"
#include "../Styling/PartPainters.h"

/// <summary>
/// Base class of buttons which handles user input. Derived classes should draw the button and the contents.
/// </summary>
API_CLASS()
class FUDGETS_API FudgetButtonBase : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetButtonBase);
public:

    API_PROPERTY() FORCE_INLINE bool GetDown() const { return _down; }
    API_PROPERTY() void SetDown(bool value);

    API_PROPERTY() FORCE_INLINE bool GetPressed() const { return _pressed; }
    API_PROPERTY() void SetPressed(bool value);

    API_FUNCTION() virtual void OnDownChanged() {}
    API_FUNCTION() virtual void OnPressedChanged() {}

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

protected:
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

    API_PROPERTY() static FudgetToken GetClassToken();
    API_PROPERTY() static FudgetToken GetBackgroundToken();
    API_PROPERTY() static FudgetToken GetPressedBackgroundToken();
    API_PROPERTY() static FudgetToken GetDisabledBackgroundToken();
    API_PROPERTY() static FudgetToken GetFramePainterToken();
    API_PROPERTY() static FudgetToken GetFrameStyleToken();
    API_PROPERTY() static FudgetToken GetContentPainterToken();
    API_PROPERTY() static FudgetToken GetContentStyleToken();
protected:
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


