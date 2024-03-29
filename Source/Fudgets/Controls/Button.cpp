#include "Button.h"
#include "../Styling/Themes.h"
#include "../Styling/Painters/ContentPainter.h"
#include "../Styling/Painters/DrawablePainter.h"
#include "../Styling/PartPainterIds.h"

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

void FudgetButtonBase::OnDownChanged()
{
    SetVisualState(FudgetVisualControlState::Down, GetPressed());
}

void FudgetButtonBase::OnPressedChanged()
{
    SetVisualState(FudgetVisualControlState::Pressed, GetPressed());
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

void FudgetButtonBase::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (!GetPressed() || !MouseIsCaptured())
        return;
    SetVisualState(FudgetVisualControlState::Pressed, WantsMouseEventAtPos(pos, global_pos));
}

FudgetControlFlag FudgetButtonBase::GetInitFlags() const
{
    return FudgetControlFlag::CanHandleMouseMove | FudgetControlFlag::CanHandleMouseEnterLeave | FudgetControlFlag::CanHandleMouseUpDown |
        FudgetControlFlag::CaptureReleaseMouseLeft | FudgetControlFlag::FocusOnMouseLeft | Base::GetInitFlags();
}



FudgetButton::FudgetButton(const SpawnParams &params) : Base(params), _content_painter(nullptr), _pressed_offset(Float2::Zero)
{
}

void FudgetButton::OnInitialize()
{
}

void FudgetButton::OnStyleInitialize()
{
    Base::OnStyleInitialize();

    _content_painter = CreateStylePainter<FudgetStatePainter>(_content_painter, (int)FudgetContentPartIds::ContentPainter);
    if (!GetStylePadding((int)FudgetButtonPartIds::Padding, _padding))
        _padding = FudgetPadding(0);
    if (!GetStyleFloat2((int)FudgetButtonPartIds::PressedOffset, _pressed_offset))
        _pressed_offset = Float2::Zero;
}


void FudgetButton::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_content_painter != nullptr)
    {
        Rectangle r = GetCombinedPadding().Padded(bounds);
        uint64 state = GetVisualState();
        if ((state & uint64(FudgetVisualControlState::Pressed | FudgetVisualControlState::Down)) != 0)
            r.Location += _pressed_offset;
        _content_painter->Draw(this, r, state);
    }
}

FudgetControlFlag FudgetButton::GetInitFlags() const
{
    return Base::GetInitFlags();
}

FudgetPadding FudgetButton::GetCombinedPadding() const
{
    return _padding + GetFramePadding();
}

