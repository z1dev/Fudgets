#include "Button.h"
#include "../Styling/Themes.h"

FudgetButtonBase::FudgetButtonBase(const SpawnParams &params) : Base(params), _down(false), _pressed(false)
{

}

void FudgetButtonBase::SetDown(bool value)
{
    if (_down == value)
        return;
    _down = value;
    OnDownChanged();
}

void FudgetButtonBase::SetPressed(bool value)
{
    if (_pressed == value)
        return;
    _pressed = value;
    OnPressedChanged();
}

FudgetInputResult FudgetButtonBase::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (button != MouseButton::Left)
        return FudgetInputResult::Ignore;

    if (!_pressed)
        SetPressed(true);

    return FudgetInputResult::Consume;
}

bool FudgetButtonBase::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    if (button != MouseButton::Left || !_pressed)
        return true;

    SetPressed(false);

    return true;
}

FudgetControlFlags FudgetButtonBase::GetInitFlags() const
{
    return FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown |
        FudgetControlFlags::CaptureReleaseMouseLeft | FudgetControlFlags::FocusOnMouseLeft | Base::GetInitFlags();
}


FudgetToken FudgetButton::ClassToken = -1;
FudgetToken FudgetButton::BackgroundToken = -1;
FudgetToken FudgetButton::PressedBackgroundToken = -1;
FudgetToken FudgetButton::DisabledBackgroundToken = -1;

FudgetToken FudgetButton::FramePainterToken = -1;
FudgetToken FudgetButton::FrameStyleToken = -1;

FudgetToken FudgetButton::ContentPainterToken = -1;
FudgetToken FudgetButton::ContentStyleToken = -1;

FudgetButton::FudgetButton(const SpawnParams &params) : Base(params), _draw_state(), _frame_painter(nullptr), _content_painter(nullptr)
{

}

void FudgetButton::OnInitialize()
{
    FudgetToken frame_style;
    if (!GetStyleToken(FrameStyleToken, frame_style))
        frame_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, frame_style);

    FudgetToken content_style;
    if (!GetStyleToken(ContentStyleToken, content_style))
        content_style = FudgetToken::Invalid;

    _content_painter = CreateStylePainter<FudgetStatePainter>(ContentPainterToken, content_style);
}

FudgetPadding FudgetButton::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0.0f);
} 

void FudgetButton::OnUpdate(float delta_time)
{
}

void FudgetButton::OnDraw()
{
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, _draw_state);
    if (_content_painter != nullptr)
        _content_painter->Draw(this, _draw_state);
}

void FudgetButton::OnPressedChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Pressed, GetPressed());
}

void FudgetButton::OnDownChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Down, GetDown());
}

void FudgetButton::OnFocusChanged(bool focused, FudgetControl *other)
{
    _draw_state.SetState(FudgetFramedFieldState::Focused, focused);
}

void FudgetButton::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (!GetPressed() || !MouseIsCaptured())
        return;
    _draw_state.SetState(FudgetFramedFieldState::Pressed, WantsMouseEventAtPos(pos, global_pos));
}

void FudgetButton::OnMouseEnter(Float2 pos, Float2 global_pos)
{
    _draw_state.SetState(FudgetFramedFieldState::Hovered);
}

void FudgetButton::OnMouseLeave()
{
    _draw_state.SetState(FudgetFramedFieldState::Hovered, false);
}

void FudgetButton::OnVirtuallyEnabledChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Disabled, !VirtuallyEnabled());
}

FudgetToken FudgetButton::GetClassToken()
{
    InitializeTokens();
    return ClassToken;
}

FudgetToken FudgetButton::GetBackgroundToken()
{
    InitializeTokens();
    return BackgroundToken;
}

FudgetToken FudgetButton::GetPressedBackgroundToken()
{
    InitializeTokens();
    return PressedBackgroundToken;
}

FudgetToken FudgetButton::GetDisabledBackgroundToken()
{
    InitializeTokens();
    return DisabledBackgroundToken;
}

FudgetToken FudgetButton::GetFramePainterToken()
{
    InitializeTokens();
    return FramePainterToken;
}

FudgetToken FudgetButton::GetFrameStyleToken()
{
    InitializeTokens();
    return FrameStyleToken;
}

FudgetToken FudgetButton::GetContentPainterToken()
{
    InitializeTokens();
    return ContentPainterToken;
}

FudgetToken FudgetButton::GetContentStyleToken()
{
    InitializeTokens();
    return ContentStyleToken;
}

FudgetControlFlags FudgetButton::GetInitFlags() const
{
    return Base::GetInitFlags() | FudgetControlFlags::RegisterToUpdates;
}

void FudgetButton::InitializeTokens()
{
    if (ClassToken != FudgetToken::Invalid)
        return;

    ClassToken = FudgetThemes::RegisterToken(TEXT("FudgetButton"));
    BackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_BackgroundToken"));
    PressedBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_PressedBackgroundToken"));
    DisabledBackgroundToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_DisabledBackgroundToken"));
    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_FrameStyle"));
    ContentPainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_ContentPainter"));
    ContentStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_FudgetButton_ContentStyle"));

}