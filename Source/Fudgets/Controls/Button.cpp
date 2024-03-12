#include "Button.h"
#include "../Styling/Themes.h"
#include "../Styling/Painters/AlignedImagePainter.h"
#include "../Styling/Painters/FramedFieldPainter.h"
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



FudgetButton::FudgetButton(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _content_painter(nullptr)
{
    FudgetStyle *parentstyle = FudgetThemes::GetStyle(FudgetThemes::IMAGE_BUTTON_STYLE);
    FudgetStyle *style = parentstyle->CreateInheritedStyle<FudgetButton>();
}

void FudgetButton::OnInitialize()
{
}

void FudgetButton::OnStyleInitialize()
{
    //FudgetStyle *frame_style = nullptr;
    //if (!GetStyleStyle((int)FudgetButtonIds::FrameStyle, frame_style))
    //    frame_style = nullptr;
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(_frame_painter, (int)FudgetFramedControlPartIds::FramePainter);

    //FudgetStyle *content_style = nullptr;
    //if (!GetStyleStyle((int)FudgetButtonIds::ContentStyle, content_style))
    //    content_style = nullptr;
    _content_painter = CreateStylePainter<FudgetStatePainter>(_content_painter, (int)FudgetButtonPartIds::ContentPainter);


}

FudgetPadding FudgetButton::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0);
} 

void FudgetButton::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());
    if (_content_painter != nullptr)
        _content_painter->Draw(this, bounds, GetVisualState());
}

//FudgetControlFlag FudgetButton::GetInitFlags() const
//{
//    return Base::GetInitFlags() | FudgetControlFlag::RegisterToUpdates;
//}

