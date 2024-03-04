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

void FudgetButtonBase::DoPressedChanged()
{
    OnPressedChanged();
    if (_pressed && EventPressed.IsBinded())
        EventPressed();
}

void FudgetButtonBase::SetPressed(bool value)
{
    if (_pressed == value)
        return;
    _pressed = value;
    DoPressedChanged();
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

FudgetControlFlag FudgetButtonBase::GetInitFlags() const
{
    return FudgetControlFlag::CanHandleMouseMove | FudgetControlFlag::CanHandleMouseEnterLeave | FudgetControlFlag::CanHandleMouseUpDown |
        FudgetControlFlag::CaptureReleaseMouseLeft | FudgetControlFlag::FocusOnMouseLeft | Base::GetInitFlags();
}



FudgetButton::FudgetButton(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _content_painter(nullptr)
{

}

void FudgetButton::OnInitialize()
{
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>((int)FudgetButtonIds::FramePainter);
    _content_painter = CreateStylePainter<FudgetStatePainter>((int)FudgetButtonIds::ContentPainter);
}

void FudgetButton::OnStyleInitialize()
{
    FudgetStyle *frame_style = nullptr;
    if (!GetStyleStyle((int)FudgetButtonIds::FrameStyle, frame_style))
        frame_style = nullptr;
    FudgetStyle *content_style = nullptr;
    if (!GetStyleStyle((int)FudgetButtonIds::ContentStyle, content_style))
        content_style = nullptr;

    InitializeStylePainter(_frame_painter, frame_style);
    InitializeStylePainter(_content_painter, content_style);
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
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());
    if (_content_painter != nullptr)
        _content_painter->Draw(this, bounds, GetVisualState());
}

void FudgetButton::OnPressedChanged()
{
    SetVisualState(FudgetVisualControlState::Pressed, GetPressed());
}

void FudgetButton::OnDownChanged()
{
    SetVisualState(FudgetVisualControlState::Down, GetPressed());
}

void FudgetButton::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (!GetPressed() || !MouseIsCaptured())
        return;
    SetVisualState(FudgetVisualControlState::Pressed, WantsMouseEventAtPos(pos, global_pos));
}

FudgetControlFlag FudgetButton::GetInitFlags() const
{
    return Base::GetInitFlags() | FudgetControlFlag::RegisterToUpdates;
}

