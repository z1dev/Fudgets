#include "ComboBox.h"
#include "Button.h"
#include "LineEdit.h"
#include "ListBox.h"

#include "../Utils/Utils.h"

#include "../Styling/Themes.h"




FudgetComboBox::FudgetComboBox(const SpawnParams &params) : Base(params), _layout(nullptr), _frame_painter(nullptr),
    _button_width(0.f), _editor(nullptr), _button(nullptr), _list_box(nullptr), _editor_capturing(false), _last_mouse_pos(0.f)
{

}

FudgetComboBox::~FudgetComboBox()
{
    Delete(_list_data);
}

void FudgetComboBox::OnInitialize()
{
    FudgetFramedFieldPainterResources frame_res;
    frame_res.FrameDraw = (int)FudgetComboBoxIds::FrameDraw;
    frame_res.HoveredFrameDraw = (int)FudgetComboBoxIds::FrameDraw;
    frame_res.PressedFrameDraw = (int)FudgetComboBoxIds::FrameDraw;
    frame_res.DownFrameDraw = (int)FudgetComboBoxIds::FrameDraw;
    frame_res.DisabledFrameDraw = (int)FudgetComboBoxIds::DisabledFrameDraw;
    frame_res.FocusedFrameDraw = (int)FudgetComboBoxIds::FocusedFrameDraw;
    frame_res.ContentPadding = (int)FudgetComboBoxIds::ContentPadding;
    default_frame_painter_mapping = FudgetPartPainter::InitializeMapping<FudgetFramedFieldPainter>(frame_res);

    _editor = CreateChild<FudgetLineEdit>();
    _editor->SetShowBorder(false);
    _editor->SetStylingName(TEXT("Fudgets_ComboBox"));

    _button = CreateChild<FudgetButton>();
    _button->EventPressed.Bind<FudgetComboBox, &FudgetComboBox::ButtonPressed>(this);
    _button->SetStylingName(TEXT("Fudgets_ComboBox"));

    _list_box = CreateChild<FudgetListBox>();
    _list_box->SetStylingName(TEXT("Fudgets_ComboBox"));

    FudgetGUIRoot *root = GetGUIRoot();
    root->AddChild(_list_box);
    _list_box->SetAlwaysOnTop(true);
    _list_box->SetVisible(false);

    _list_data = New<FudgetStringListProvider>(SpawnParams(Guid::New(), FudgetStringListProvider::TypeInitializer));
    _list_box->SetDataProvider(_list_data);

    _layout = CreateLayout<FudgetProxyLayout>();
}

void FudgetComboBox::OnStyleInitialize()
{
    if (!GetStyleFloat((int)FudgetComboBoxIds::ButtonWidth, _button_width))
        _button_width = 20.f;

    FudgetStyle *frame_style;
    if (!GetStyleStyle((int)FudgetComboBoxIds::FrameStyle, frame_style))
        frame_style = nullptr;
    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(_frame_painter, (int)FudgetComboBoxIds::FramePainter, frame_style , &default_frame_painter_mapping);
}

void FudgetComboBox::OnDraw()
{
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, GetBounds(), GetVisualState());
}

//void FudgetComboBox::OnFocusChanged(bool focused, FudgetControl *other)
//{
//    _draw_state.SetState(FudgetVisualControlState::Focused, focused);
//}
//
//void FudgetComboBox::OnVirtuallyEnabledChanged()
//{
//    _draw_state.SetState(FudgetVisualControlState::Disabled, !VirtuallyEnabled());
//}

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
        _editor->DoMouseEnter(pos - GetInnerPadding().UpperLeft(), global_pos);
    else
        _button->DoMouseEnter(pos - _button->GetPosition(), global_pos);
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

void FudgetComboBox::OnSizeChanged()
{
    Float2 siz = _list_box->GetHintSize();
    siz.X = GetSize().X;
    _list_box->SetHintSize(siz);

    Base::OnSizeChanged();
}

FudgetLayoutSlot* FudgetComboBox::ProxyInterfaceCreateSlot(FudgetControl *control)
{
    return nullptr;
}

FudgetLayoutFlag FudgetComboBox::ProxyInterfaceGetInitFlags() const
{
    return FudgetLayoutFlag::LayoutOnContainerResize | FudgetLayoutFlag::LayoutOnContentResize |
        FudgetLayoutFlag::ResizeOnContainerResize | FudgetLayoutFlag::ResizeOnContentResize | FudgetLayoutFlag::CanProvideSizeHeight;
}

void FudgetComboBox::ProxyInterfacePreLayoutChildren(Float2 space)
{
}

void FudgetComboBox::ProxyInterfaceLayoutChildren(Float2 space)
{
    if (_editor == nullptr || _button == nullptr)
        return;

    Float2 txt_wanted = Float2::Zero;
    Float2 txt_min = Float2::Zero;
    Float2 txt_max = Float2::Zero;
    _layout->GetSlotMeasures(0, Float2(-1.f), txt_wanted, txt_min, txt_max);

    FudgetPadding inner = GetInnerPadding();
    Float2 min = txt_min + inner.Size() + Float2(_button_width, 0.f);
    min.X = Math::Max(min.X, Base::GetMinSize().X);
    Float2 normal = txt_wanted + inner.Size() + Float2(_button_width, 0.f);

    if (!_layout->IsUnrestrictedSpace(space))
    {
        normal.X = Math::Min(space.X, Base::GetMaxSize().X);

        _layout->SetComputedBounds(0, inner.UpperLeft(), Float2::Max(Float2::Min(space, normal), min) - Float2(inner.Width() + _button_width, inner.Height()));
        _layout->SetComputedBounds(1, Float2(Math::Max(Math::Min(space.X, normal.X), min.X) - _button_width - inner.Right, 0.0f), Float2(_button_width, Math::Max(Math::Min(space.Y, normal.Y), min.Y)  ));
    }
    txt_max.Y = normal.Y;

    _layout->SetControlSizes(FudgetLayoutSizeCache(space, normal, min, AddBigFloats(txt_max, Float2(_button_width, 0.f), inner.Size()), false));
}

bool FudgetComboBox::WantsNavigationKey(KeyboardKeys key)
{
    return key == KeyboardKeys::ArrowLeft || key == KeyboardKeys::ArrowRight || key == KeyboardKeys::ArrowUp || key == KeyboardKeys::ArrowDown;
}

void FudgetComboBox::ButtonPressed()
{

}

FudgetControlFlag FudgetComboBox::GetInitFlags() const
{
    return FudgetControlFlag::CanHandleMouseUpDown | FudgetControlFlag::CanHandleMouseMove | FudgetControlFlag::CanHandleMouseEnterLeave |
        FudgetControlFlag::BlockMouseEvents | FudgetControlFlag::FocusOnMouseLeft | FudgetControlFlag::CaptureReleaseMouseLeft |
        FudgetControlFlag::CanHandleKeyEvents | FudgetControlFlag::CanHandleNavigationKeys | FudgetControlFlag::CompoundControl | Base::GetInitFlags();
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
        _editor->DoMouseLeave();
    else if (!was_editor && is_editor)
        _editor->DoMouseEnter(pos - GetInnerPadding().UpperLeft(), global_pos);

    bool was_button = !on_enter && _last_mouse_pos.X > GetWidth() - GetInnerPadding().Right - _button_width;
    bool is_button = pos.X > GetWidth() - GetInnerPadding().Right - _button_width;

    if (was_button && !is_button)
        _button->DoMouseLeave();
    else if (!was_button && is_button)
        _button->DoMouseEnter(pos - _button->GetPosition(), global_pos);

    _last_mouse_pos = pos;
}
