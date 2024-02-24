#include "ComboBox.h"
#include "Button.h"
#include "LineEdit.h"

#include "../Utils/Utils.h"

#include "../Styling/Themes.h"



FudgetToken FudgetComboBox::ClassToken = -1;
FudgetToken FudgetComboBox::FrameDrawToken = -1;
FudgetToken FudgetComboBox::FocusedFrameDrawToken = -1;
FudgetToken FudgetComboBox::DisabledFrameDrawToken = -1;
FudgetToken FudgetComboBox::FramePainterToken = -1;
FudgetToken FudgetComboBox::FrameStyleToken = -1;
FudgetToken FudgetComboBox::ButtonContentStyleToken = -1;
FudgetToken FudgetComboBox::EditorClassToken = -1;
FudgetToken FudgetComboBox::ButtonClassToken = -1;
FudgetToken FudgetComboBox::ButtonImageToken = -1;
FudgetToken FudgetComboBox::ButtonHoveredImageToken = -1;
FudgetToken FudgetComboBox::ButtonDisabledImageToken = -1;
FudgetToken FudgetComboBox::ButtonWidthToken = -1;

FudgetComboBox::FudgetComboBox(const SpawnParams &params) : Base(params), _draw_state(), _layout(nullptr), _frame_painter(nullptr),
    _button_width(0.f), _editor(nullptr), _button(nullptr), _editor_capturing(false), _last_mouse_pos(0.f)
{

}

FudgetToken FudgetComboBox::GetClassToken() { InitializeTokens(); return ClassToken; }
FudgetToken FudgetComboBox::GetFrameDrawToken() { InitializeTokens(); return FrameDrawToken; }
FudgetToken FudgetComboBox::GetFocusedFrameDrawToken() { InitializeTokens(); return FocusedFrameDrawToken; }
FudgetToken FudgetComboBox::GetDisabledFrameDrawToken() { InitializeTokens(); return DisabledFrameDrawToken; }
FudgetToken FudgetComboBox::GetFramePainterToken() { InitializeTokens(); return FramePainterToken; }
FudgetToken FudgetComboBox::GetFrameStyleToken() { InitializeTokens(); return FrameStyleToken; }
FudgetToken FudgetComboBox::GetButtonContentStyleToken() { InitializeTokens(); return ButtonContentStyleToken; }
FudgetToken FudgetComboBox::GetEditorClassToken() { InitializeTokens(); return EditorClassToken; }
FudgetToken FudgetComboBox::GetButtonClassToken() { InitializeTokens(); return ButtonClassToken; }
FudgetToken FudgetComboBox::GetButtonImageToken() { InitializeTokens(); return ButtonImageToken; }
FudgetToken FudgetComboBox::GetButtonHoveredImageToken() { InitializeTokens(); return ButtonHoveredImageToken; }
FudgetToken FudgetComboBox::GetButtonDisabledImageToken() { InitializeTokens(); return ButtonDisabledImageToken; }
FudgetToken FudgetComboBox::GetButtonWidthToken() { InitializeTokens(); return ButtonWidthToken; }

void FudgetComboBox::InitializeTokens()
{
    ClassToken = FudgetThemes::RegisterToken(TEXT("FudgetComboBox"));
    FrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_FrameDraw"));
    FocusedFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_FocusedFrameDraw"));
    DisabledFrameDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_DisabledFrameDraw"));
    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_FrameStyle"));
    ButtonContentStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_ButtonContentStyle"));
    EditorClassToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_Editor"));
    ButtonClassToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_Button"));
    ButtonImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_ButtonImage"));
    ButtonHoveredImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_ButtonHoveredImage"));
    ButtonDisabledImageToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_ButtonDisabledImage"));
    ButtonWidthToken = FudgetThemes::RegisterToken(TEXT("Fudgets_ComboBox_ButtonWidth"));
}

void FudgetComboBox::OnInitialize()
{
    FudgetToken frame_style;
    if (!GetStyleToken(FrameStyleToken, frame_style))
        frame_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, frame_style);

    // Not using CreateChild makes sure the text box doesn't initialize its styles until AddChild
    _editor = New<FudgetLineEdit>();
    _editor->SetStyle(FudgetThemes::GetStyle(EditorClassToken));
    AddChild(_editor);

    if (!GetStyleFloat(ButtonWidthToken, _button_width))
        _button_width = 20.f;

    _button = New<FudgetButton>();
    _button->SetStyle(FudgetThemes::GetStyle(ButtonClassToken));
    AddChild(_button);

    _layout = CreateLayout<FudgetProxyLayout>();
}

void FudgetComboBox::OnDraw()
{
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, GetBounds(), _draw_state);
}

void FudgetComboBox::OnFocusChanged(bool focused, FudgetControl *other)
{
    _draw_state.SetState(FudgetFramedFieldState::Focused, focused);
}

void FudgetComboBox::OnVirtuallyEnabledChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Disabled, !VirtuallyEnabled());
}

FudgetInputResult FudgetComboBox::OnCharInput(Char ch)
{
    return _editor->OnCharInput(ch);
}

FudgetInputResult FudgetComboBox::OnKeyDown(KeyboardKeys key)
{
    return _editor->OnKeyDown(key);
}

bool FudgetComboBox::OnKeyUp(KeyboardKeys key)
{
    return _editor->OnKeyUp(key);
}

FudgetInputResult FudgetComboBox::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    HandleEnterLeaveMouse(pos, global_pos, false);

    if ((MouseIsCaptured() && _editor_capturing) || (!MouseIsCaptured() && pos.X <= GetWidth() - GetInnerPadding().Right - _button_width))
    {
        return _editor->OnMouseDown(pos - GetInnerPadding().UpperLeft(), global_pos, button, double_click);
    }
    else
    {
        _editor_capturing = false;
        return _button->OnMouseDown(pos - _button->GetPosition(), global_pos, button, double_click);
    }
}

bool FudgetComboBox::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    HandleEnterLeaveMouse(pos, global_pos, false);

    if ((MouseIsCaptured() && _editor_capturing) || (!MouseIsCaptured() && pos.X <= GetWidth() - GetInnerPadding().Right - _button_width))
        return _editor->OnMouseUp(pos - GetInnerPadding().UpperLeft(), global_pos, button);
    else
        return _button->OnMouseUp(pos - _button->GetPosition(), global_pos, button);
}

void FudgetComboBox::OnMouseMove(Float2 pos, Float2 global_pos)
{
    HandleEnterLeaveMouse(pos, global_pos, false);

    if ((MouseIsCaptured() && _editor_capturing) || (!MouseIsCaptured() && pos.X <= GetWidth() - GetInnerPadding().Right - _button_width))
        _editor->OnMouseMove(pos - GetInnerPadding().UpperLeft(), global_pos);
    else
        _button->OnMouseMove(pos - _button->GetPosition(), global_pos);
}

void FudgetComboBox::OnMouseEnter(Float2 pos, Float2 global_pos)
{
    HandleEnterLeaveMouse(pos, global_pos, true);

    if (pos.X <= GetWidth() - GetInnerPadding().Right - _button_width)
        _editor->OnMouseEnter(pos - GetInnerPadding().UpperLeft(), global_pos);
    else
        _button->OnMouseEnter(pos - _button->GetPosition(), global_pos);
}

void FudgetComboBox::OnMouseLeave()
{
    HandleEnterLeaveMouse(Float2(-1.f), Float2(-1.f), false);
    Base::OnMouseLeave();
}

void FudgetComboBox::OnMouseCaptured()
{
    Base::OnMouseCaptured();
    if (MouseIsCaptured())
    {
        _editor_capturing = _last_mouse_pos.X <= GetWidth() - GetInnerPadding().Right - _button_width;
        if (_editor_capturing)
            _editor->DoMouseCaptured();
        else
            _button->DoMouseCaptured();
    }
}

void FudgetComboBox::OnMouseReleased()
{
    Base::OnMouseReleased();
    if (_editor_capturing)
        _editor->DoMouseReleased();
    else
        _button->DoMouseReleased();
}

FudgetLayoutSlot* FudgetComboBox::ProxyInterfaceCreateSlot(FudgetControl *control)
{
    FudgetLayoutSlot *slot = New<FudgetLayoutSlot>();
    slot->Control = control;
    return slot;
}

FudgetLayoutFlag FudgetComboBox::ProxyInterfaceGetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize |
        FudgetLayoutFlag::ResizeOnContainerResize | FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::CanProvideSizes;
}

void FudgetComboBox::ProxyInterfacePreLayoutChildren(Float2 space)
{
}

void FudgetComboBox::ProxyInterfaceLayoutChildren(Float2 space)
{
    if (_editor == nullptr || _button == nullptr)
        return;

    if (!_layout->IsUnrestrictedSpace(space))
    {
        _layout->SetComputedBounds(0, GetInnerPadding().UpperLeft(), GetSize() - Float2(GetInnerPadding().Size().X + _button_width, GetInnerPadding().Size().Y));
        _layout->SetComputedBounds(1, Float2(GetSize().X - _button_width - GetInnerPadding().Right, 0.0f), Float2(_button_width, GetSize().Y));
    }

    _layout->SetControlSizes(FudgetLayoutSizeCache(space, FudgetControl::GetHintSize(), FudgetControl::GetMinSize(), FudgetControl::GetMaxSize(), false));
}

//bool FudgetComboBox::ProxyInterfaceMeasure(Float2 available, API_PARAM(Out) Float2 &wanted_size, API_PARAM(Out) Float2 &min_size, API_PARAM(Out) Float2 &max_size)
//{
//    wanted_size = Float2::Zero;
//    min_size = Float2::Zero;
//    max_size = Float2::Zero;
//    if (_editor != nullptr && _button != nullptr)
//    {
//        Float2 editor_wanted = Float2::Zero;
//        Float2 editor_min = Float2::Zero;
//        Float2 editor_max = Float2::Zero;
//        _editor->OnMeasure(available, editor_wanted, editor_min, editor_max);
//        Float2 button_wanted = Float2::Zero;
//        Float2 button_min = Float2::Zero;
//        Float2 button_max = Float2::Zero;
//        _button->OnMeasure(available, button_wanted, button_min, button_max);
//    }
//
//    wanted_size = FudgetControl::GetHintSize();
//    min_size = FudgetControl::GetMinSize();
//    max_size = FudgetControl::GetMaxSize();
//
//    return false;
//}

bool FudgetComboBox::WantsNavigationKey(KeyboardKeys key)
{
    return key == KeyboardKeys::ArrowLeft || key == KeyboardKeys::ArrowRight || key == KeyboardKeys::ArrowUp || key == KeyboardKeys::ArrowDown;
}

FudgetControlFlags FudgetComboBox::GetInitFlags() const
{
    return FudgetControlFlags::CanHandleMouseUpDown | FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave |
        FudgetControlFlags::BlockMouseEvents | FudgetControlFlags::FocusOnMouseLeft | FudgetControlFlags::CaptureReleaseMouseLeft |
        FudgetControlFlags::CanHandleKeyEvents | FudgetControlFlags::CanHandleNavigationKeys | FudgetControlFlags::CompoundControl | Base::GetInitFlags();
}

FudgetPadding FudgetComboBox::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0.0f);
}

void FudgetComboBox::HandleEnterLeaveMouse(Float2 pos, Float2 global_pos, bool on_enter)
{
    if (MouseIsCaptured())
        return;

    bool was_editor = !on_enter && _last_mouse_pos.X <= GetWidth() - GetInnerPadding().Right - _button_width;
    bool is_editor = pos.X >= 0 && pos.X <= GetWidth() - GetInnerPadding().Right - _button_width;

    if (was_editor && !is_editor)
        _editor->OnMouseLeave();
    else if (!was_editor && is_editor)
        _editor->OnMouseEnter(pos - GetInnerPadding().UpperLeft(), global_pos);

    bool was_button = !on_enter && _last_mouse_pos.X > GetWidth() - GetInnerPadding().Right - _button_width;
    bool is_button = pos.X > GetWidth() - GetInnerPadding().Right - _button_width;

    if (was_button && !is_button)
        _button->OnMouseLeave();
    else if (!was_button && is_button)
        _button->OnMouseEnter(pos - _button->GetPosition(), global_pos);

    _last_mouse_pos = pos;
}
