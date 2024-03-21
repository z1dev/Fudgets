#include "ComboBox.h"
#include "Button.h"
#include "LineEdit.h"
#include "ListBox.h"

#include "../Utils/Utils.h"

#include "../Styling/Themes.h"
#include "../Styling/Painters/DrawablePainter.h"
#include "../Styling/PartPainterIds.h"



FudgetComboBox::FudgetComboBox(const SpawnParams &params) : Base(params), _layout(nullptr), /*_frame_painter(nullptr),*/
    _button_width(0), _editor(nullptr), _button(nullptr), _list_box(nullptr), _list_data(nullptr), _editor_capturing(false),
    _button_capturing(false), _last_mouse_pos(0.f)
{
}

FudgetComboBox::~FudgetComboBox()
{
    if (_list_data != nullptr)
        Delete(_list_data);
}

void FudgetComboBox::OnInitialize()
{
    _editor = CreateChild<FudgetLineEdit>(FudgetThemes::COMBOBOX_EDITOR_STYLE);
    _editor->SetFramed(false);

    _button = CreateChild<FudgetButton>(FudgetThemes::COMBOBOX_BUTTON_STYLE);
    _button->EventPressed.Bind<FudgetComboBox, &FudgetComboBox::ButtonPressed>(this);

    _list_box = CreateChild<FudgetListBox>(FudgetThemes::COMBOBOX_LIST_STYLE);

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
    Base::OnStyleInitialize();

    if (!GetStylePadding((int)FudgetFieldPartIds::Padding, _content_padding))
        _content_padding = FudgetPadding(0);
    if (!GetStyleInt((int)FudgetComboBoxPartIds::ButtonWidth, _button_width))
        _button_width = 20;
}

void FudgetComboBox::OnDraw()
{
    Base::OnDraw();
    //if (_frame_painter != nullptr)
    //    _frame_painter->Draw(this, GetBounds(), GetVisualState());
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

    if (_editor_capturing || (!_button_capturing && PosOnEditor(pos)))
        return _editor->DoMouseDown(pos - _editor->GetPosition(), global_pos, button, double_click);
    else if (_button_capturing || (!_editor_capturing && PosOnButton(pos)))
        return _button->DoMouseDown(pos - _button->GetPosition(), global_pos, button, double_click);

    return FudgetInputResult::Consume;
}

bool FudgetComboBox::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    HandleEnterLeaveMouse(pos, global_pos, false);
    if (!MouseIsCaptured())
        return true;

    bool r = true;
    if (_editor_capturing)
        r = _editor->DoMouseUp(pos - _editor->GetPosition(), global_pos, button);
    else if (_button_capturing)
        r =  _button->DoMouseUp(pos - _button->GetPosition(), global_pos, button);

    return r;
}

void FudgetComboBox::OnMouseMove(Float2 pos, Float2 global_pos)
{
    HandleEnterLeaveMouse(pos, global_pos, false);

    if (_editor_capturing || (!_button_capturing && PosOnEditor(pos)))
        _editor->DoMouseMove(pos - _editor->GetPosition(), global_pos);
    else if(_button_capturing || (!_editor_capturing && PosOnButton(pos)))
        _button->DoMouseMove(pos - _button->GetPosition(), global_pos);
}

void FudgetComboBox::OnMouseEnter(Float2 pos, Float2 global_pos)
{
    HandleEnterLeaveMouse(pos, global_pos, true);

    if (PosOnEditor(pos))
        _editor->DoMouseEnter(pos - _editor->GetPosition(), global_pos);
    else if (PosOnButton(pos))
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
        _editor_capturing = PosOnEditor(_last_mouse_pos);
        _button_capturing = !_editor_capturing && PosOnButton(_last_mouse_pos);
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
    _editor_capturing = false;
    _button_capturing = false;
}

void FudgetComboBox::OnSizeChanged()
{
    Int2 siz = _list_box->GetHintSize();
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

void FudgetComboBox::ProxyInterfacePreLayoutChildren(Int2 space)
{
}

void FudgetComboBox::ProxyInterfaceLayoutChildren(Int2 space)
{
    if (_editor == nullptr || _button == nullptr)
        return;

    Int2 txt_wanted = Int2::Zero;
    Int2 txt_min = Int2::Zero;
    Int2 txt_max = Int2::Zero;
    _layout->GetSlotMeasures(0, Int2(-1), txt_wanted, txt_min, txt_max);

    FudgetPadding text_padding = GetCombinedPadding();
    FudgetPadding button_padding = GetFramePadding();
    text_padding.Right = button_padding.Right + _button_width;

    Int2 min = txt_min + text_padding.Size();
    min.X = Math::Max(min.X, Base::GetMinSize().X);
    Int2 wanted = txt_wanted + text_padding.Size();

    if (!_layout->IsUnrestrictedSpace(space))
    {
        wanted.X = Math::Max(min.X, Math::Min(space.X, Base::GetMaxSize().X));

        _layout->SetComputedBounds(0, text_padding.UpperLeft(), Int2::Max(Int2::Min(space, wanted), min) - text_padding.Size());
        Int2 btn_pos = Int2(wanted.X - button_padding.Right - _button_width, button_padding.Top);
        _layout->SetComputedBounds(1, btn_pos, Int2(Math::Min(_button_width, space.X - button_padding.Width()), Math::Max(Math::Min(space.Y, wanted.Y), min.Y) - button_padding.Height()));
    }

    txt_max.Y = wanted.Y;
    _layout->SetControlSizes(FudgetLayoutSizeCache(space, wanted, min, AddBigValues(txt_max, text_padding.Size()), false));
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
        FudgetControlFlag::BlockMouseEvents | FudgetControlFlag::FocusOnMouseLeft | FudgetControlFlag::CaptureReleaseMouseLeft | FudgetControlFlag::Framed |
        FudgetControlFlag::CanHandleKeyEvents | FudgetControlFlag::CanHandleNavigationKeys | FudgetControlFlag::CompoundControl | Base::GetInitFlags();
}

FudgetPadding FudgetComboBox::GetCombinedPadding() const
{
    return _content_padding + GetFramePadding();
}

bool FudgetComboBox::PosOnEditor(Float2 pos) const
{
    FudgetPadding pad = GetCombinedPadding();
    pad.Right = GetFramePadding().Right + _button_width;

    return pad.Padded(GetBounds()).Contains(pos);
}

bool FudgetComboBox::PosOnButton(Float2 pos) const
{
    FudgetPadding pad = GetFramePadding();
    Rectangle r = GetBounds();
    return pos.Y >= r.GetTop() + pad.Top && pos.Y <= r.GetBottom() - pad.Bottom && pos.X > Math::Max((float)pad.Left, r.GetRight() - pad.Right - _button_width) && pos.X < r.GetRight() - pad.Right;
}

void FudgetComboBox::HandleEnterLeaveMouse(Float2 pos, Float2 global_pos, bool on_enter)
{
    if (MouseIsCaptured())
        return;

    bool was_editor = !on_enter && PosOnEditor(_last_mouse_pos);
    bool is_editor = PosOnEditor(pos);

    if (was_editor && !is_editor)
        _editor->DoMouseLeave();
    else if (!was_editor && is_editor)
        _editor->DoMouseEnter(pos - _editor->GetPosition(), global_pos);

    bool was_button = !on_enter && !was_editor && PosOnButton(_last_mouse_pos);
    bool is_button = !is_editor && PosOnButton(pos);

    if (was_button && !is_button)
        _button->DoMouseLeave();
    else if (!was_button && is_button)
        _button->DoMouseEnter(pos - _button->GetPosition(), global_pos);

    _last_mouse_pos = pos;
}
