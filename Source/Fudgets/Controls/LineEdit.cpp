#include "LineEdit.h"
#include "../Styling/Themes.h"

#include "Engine/Render2D/Font.h"



FudgetToken FudgetLineEdit::ClassToken = -1;

FudgetToken FudgetLineEdit::FramePainterToken = -1;
FudgetToken FudgetLineEdit::FrameStyleToken = -1;

FudgetToken FudgetLineEdit::SelectionDrawToken = -1;
FudgetToken FudgetLineEdit::FocusedSelectionDrawToken = -1;
FudgetToken FudgetLineEdit::DisabledSelectionDrawToken = -1;

FudgetToken FudgetLineEdit::TextColorToken = -1;
FudgetToken FudgetLineEdit::DisabledTextColorToken = -1;
FudgetToken FudgetLineEdit::SelectedTextColorToken = -1;
FudgetToken FudgetLineEdit::FocusedSelectedTextColorToken = -1;
FudgetToken FudgetLineEdit::DisabledSelectedTextColorToken = -1;

FudgetToken FudgetLineEdit::FontToken = -1;

FudgetToken FudgetLineEdit::CaretBlinkTimeToken = -1;
FudgetToken FudgetLineEdit::CaretWidthToken = -1;

FudgetToken FudgetLineEdit::GetClassToken()
{
    InitializeTokens();
    return ClassToken;
}
FudgetToken FudgetLineEdit::GetFramePainterToken()
{ 
    InitializeTokens();
    return FramePainterToken;
}
FudgetToken FudgetLineEdit::GetFrameStyleToken()
{
    InitializeTokens();
    return FrameStyleToken;
}
FudgetToken FudgetLineEdit::GetSelectionDrawToken()
{
    InitializeTokens();
    return SelectionDrawToken;
}
FudgetToken FudgetLineEdit::GetFocusedSelectionDrawToken()
{
    InitializeTokens();
    return FocusedSelectionDrawToken;
}
FudgetToken FudgetLineEdit::GetDisabledSelectionDrawToken()
{
    InitializeTokens();
    return DisabledSelectionDrawToken;
}
FudgetToken FudgetLineEdit::GetTextColorToken()
{
    InitializeTokens();
    return TextColorToken;
}
FudgetToken FudgetLineEdit::GetDisabledTextColorToken()
{
    InitializeTokens();
    return DisabledTextColorToken;
}
FudgetToken FudgetLineEdit::GetSelectedTextColorToken()
{
    InitializeTokens();
    return SelectedTextColorToken;
}
FudgetToken FudgetLineEdit::GetFocusedSelectedTextColorToken()
{
    InitializeTokens();
    return FocusedSelectedTextColorToken;
}
FudgetToken FudgetLineEdit::GetDisabledSelectedTextColorToken()
{
    InitializeTokens();
    return DisabledSelectedTextColorToken;
}
FudgetToken FudgetLineEdit::GetFontToken()
{
    InitializeTokens();
    return FontToken;
}
FudgetToken FudgetLineEdit::GetCaretBlinkTimeToken()
{
    InitializeTokens();
    return CaretBlinkTimeToken;
}
FudgetToken FudgetLineEdit::GetCaretWidthToken()
{
    InitializeTokens();
    return CaretWidthToken;
}

void FudgetLineEdit::InitializeTokens()
{
    if (ClassToken != FudgetToken::Invalid)
        return;

    ClassToken = FudgetThemes::RegisterToken(TEXT("FudgetLineEdit"));

    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FrameStyle"));

    SelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_SelectionDraw"));
    FocusedSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FocusedSelectionDraw"));
    DisabledSelectionDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_DisabledSelectionDraw"));

    TextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_TextColor"));
    DisabledTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_DisabledTextColor"));
    SelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_SelectedTextColor"));
    FocusedSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FocusedSelectedTextColor"));
    DisabledSelectedTextColorToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_DisabledSelectedTextColor"));

    FontToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_Font"));

    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretBlinkTime"));
    CaretWidthToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretWidth"));
}

FudgetLineEdit::FudgetLineEdit(const SpawnParams &params) : Base(params), _draw_state(), _frame_painter(nullptr),
    _sel_area(), _focused_sel_area(), _disabled_sel_area(),  _text_color(Color::White), _disabled_text_color(Color::White),
    _selected_text_color(Color::White), _focused_selected_text_color(Color::White), _disabled_selected_text_color(Color::White),
    _blink_passed(0.0f), _scroll_pos(0.0f), _character_scroll_count(0), _caret_blink_time(1.0f), _caret_width(2.0f), _font()
{

}

void FudgetLineEdit::OnInitialize()
{
    FudgetToken background_style;
    if (!GetStyleToken(FrameStyleToken, background_style))
        background_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, background_style);

    if (!GetStyleDrawArea(SelectionDrawToken, _sel_area))
        _sel_area = FudgetDrawArea(Color(0.2f, 0.4f, 0.8f, 1.0f));
    if (!GetStyleDrawArea(FocusedSelectionDrawToken, _focused_sel_area))
        _focused_sel_area = _sel_area;
    if (!GetStyleDrawArea(DisabledSelectionDrawToken, _disabled_sel_area))
        _disabled_sel_area = _sel_area;

    if (!GetStyleColor(TextColorToken, _text_color))
        _text_color = Color::Black;
    if (!GetStyleColor(DisabledTextColorToken, _disabled_text_color))
        _disabled_text_color = _text_color;
    if (!GetStyleColor(SelectedTextColorToken, _selected_text_color))
        _selected_text_color = Color::White;
    if (!GetStyleColor(SelectedTextColorToken, _focused_selected_text_color))
        _focused_selected_text_color = _selected_text_color;
    if (!GetStyleColor(SelectedTextColorToken, _disabled_selected_text_color))
        _disabled_selected_text_color = _selected_text_color;

    if (!GetStyleFloat(CaretBlinkTimeToken, _caret_blink_time))
        _caret_blink_time = 1.0f;
    if (!GetStyleFloat(CaretWidthToken, _caret_width))
        _caret_width = 2.0f;

    GetStyleFont(FontToken, _font);
}

void FudgetLineEdit::OnDraw()
{
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, _draw_state);

    if (_font.Font == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();

    Color selTextColor = !VirtuallyEnabled() ? _disabled_selected_text_color : VirtuallyFocused() ? _focused_selected_text_color : _selected_text_color;
    Color textColor = !VirtuallyEnabled() ? _disabled_text_color : _text_color;

    TextLayoutOptions opt;
    opt.BaseLinesGapScale = 1;
    opt.Scale = 1;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;

    PushClip(innerPadding.Padded(GetBounds()));

    float caret_left = 0;
    TextRange range;
    if (GetSelStart() == GetCaretPos())
    {
        FudgetPadding tmp = innerPadding;
        tmp.Left -= _scroll_pos;
        opt.Bounds = tmp.Padded(GetBounds());
        DrawText(_font.Font, GetText(), textColor, opt);

        if (GetCaretPos() > 0)
        {
            range.StartIndex = 0;
            range.EndIndex = GetCaretPos();
            caret_left = opt.Bounds.Location.X + _font.Font->MeasureText(GetText(), range).X;
        }
        else
            caret_left = opt.Bounds.Location.X;
    }
    else
    {
        Rectangle r = innerPadding.Padded(GetBounds());
        r.Location = Float2(r.Location.X - _scroll_pos, r.Location.Y);
        opt.Bounds = r;
        int sel_min = Math::Min(GetSelStart(), GetCaretPos());
        int sel_max = Math::Max(GetSelStart(), GetCaretPos());

        range.StartIndex = 0;
        range.EndIndex = sel_min;
        if (GetSelStart() > 0)
            DrawText(_font.Font, GetText(), range, textColor, opt);

        r.Location = Float2(r.Location.X + _font.Font->MeasureText(GetText(), range).X, r.Location.Y);
        opt.Bounds = r;

        if (GetCaretPos() == sel_min)
            caret_left = r.Location.X;

        range.StartIndex = sel_min;
        range.EndIndex = sel_max;
        Float2 selRectSize = _font.Font->MeasureText(GetText(), range);

        FudgetDrawArea sel_bg = !VirtuallyEnabled() ? _disabled_sel_area : VirtuallyFocused() ? _focused_sel_area : _sel_area;

        DrawArea(sel_bg, Rectangle(opt.Bounds.Location, Float2(selRectSize.X, opt.Bounds.Size.Y)));

        DrawText(_font.Font, GetText(), range, selTextColor, opt);

        if (sel_max != GetTextLength() || GetCaretPos() == sel_max)
        {
            r.Location = Float2(r.Location.X + _font.Font->MeasureText(GetText(), range).X, r.Location.Y);
            opt.Bounds = r;

            if (GetCaretPos() == sel_max)
                caret_left = r.Location.X;

            if (sel_max != GetTextLength())
            {
                range.StartIndex = sel_max;
                range.EndIndex = GetTextLength();
                DrawText(_font.Font, GetText(), range, textColor, opt);
            }
        }
    }

    PopClip();

    range.StartIndex = 0;
    range.EndIndex = GetCaretPos();

    if (!VirtuallyFocused())
    {
        _blink_passed = 0.0f;
        return;
    }

    // Draw caret
    if (_blink_passed < _caret_blink_time)
    {
        FillRectangle(Rectangle(Float2(caret_left - 1.0f, opt.Bounds.Location.Y), Float2(_caret_width, opt.Bounds.GetHeight())), textColor);
    }
    while (_blink_passed >= _caret_blink_time * 2.0f)
        _blink_passed -= _caret_blink_time * 2.0f;
}

void FudgetLineEdit::OnSizeChanged()
{
    ScrollToPos();
}

FudgetPadding FudgetLineEdit::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0.0f);
}

int FudgetLineEdit::CharIndexAt(Float2 pos)
{
    if (_font.Font == nullptr)
        return 0;

    return _font.Font->HitTestText(GetText(), Float2(pos.X + _scroll_pos - GetInnerPadding().Left, 0.0f));
}

void FudgetLineEdit::DoPosChanged(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    ScrollToPos();

    Base::DoPosChanged(old_caret_pos, old_sel_pos);
}

void FudgetLineEdit::DoTextEdited(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    FixScrollPos();
    ScrollToPos();
    Base::DoTextEdited(old_caret_pos, old_sel_pos);
}

void FudgetLineEdit::OnUpdate(float delta_time)
{
    _blink_passed += delta_time;
}

void FudgetLineEdit::OnFocusChanged(bool focused, FudgetControl *other)
{
    _draw_state.SetState(FudgetFramedFieldState::Focused, focused);
}

void FudgetLineEdit::OnVirtuallyEnabledChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Disabled, !VirtuallyEnabled());
}

void FudgetLineEdit::OnMouseMove(Float2 pos, Float2 global_pos)
{
    Base::OnMouseMove(pos - GetInnerPadding().UpperLeft(), global_pos);
}

FudgetInputResult FudgetLineEdit::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    return Base::OnMouseDown(pos - GetInnerPadding().UpperLeft(), global_pos, button, double_click);
}

bool FudgetLineEdit::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    return Base::OnMouseUp(pos - GetInnerPadding().UpperLeft(), global_pos, button);
}

void FudgetLineEdit::ScrollToPos()
{
    if (_font.Font == nullptr)
        return;

    TextLayoutOptions opt = TextLayoutOptions();
    opt.BaseLinesGapScale = 1;
    opt.Scale = 1;
    opt.VerticalAlignment = TextAlignment::Center;
    opt.HorizontalAlignment = TextAlignment::Near;
    opt.TextWrapping = TextWrapping::NoWrap;

    opt.Bounds = GetInnerPadding().Padded(GetBounds());

    Float2 textSize = _font.Font->MeasureText(GetText().Substring(0, GetCaretPos()), opt);

    if (_scroll_pos > 0.f && textSize.X - _scroll_pos < 0.f)
    {
        // Caret out towards the start of the text.
        _scroll_pos = _font.Font->MeasureText(GetText().Substring(0, Math::Max(GetCaretPos() - _character_scroll_count, 0))).X;
    }
    else if (textSize.X - _scroll_pos >= opt.Bounds.GetWidth())
    {
        // Caret out towards the end of the text.
        _scroll_pos = _font.Font->MeasureText(GetText().Substring(0, Math::Min(GetCaretPos() + _character_scroll_count, GetTextLength()))).X - opt.Bounds.GetWidth() + _caret_width * 2.0f;
    }
    else if (_scroll_pos > 0.f && textSize.X - _scroll_pos < opt.Bounds.GetWidth())
    {

        Float2 afterSize = _font.Font->MeasureText(GetText().Substring(GetCaretPos(), GetTextLength() - GetCaretPos()), opt);
        if (textSize.X + afterSize.X - _scroll_pos < opt.Bounds.GetWidth())
            _scroll_pos = Math::Max(0.0f, textSize.X + afterSize.X - opt.Bounds.GetWidth());
    }
}

void FudgetLineEdit::FixScrollPos()
{
    if (_font.Font == nullptr)
        return;

    Float2 textSize = _font.Font->MeasureText(GetText());

    float w = GetInnerPadding().Padded(GetBounds()).GetWidth() + _scroll_pos;
    if (w > textSize.X)
        _scroll_pos = Math::Max(0.0f, textSize.X - w + _scroll_pos);
}

FudgetControlFlags FudgetLineEdit::GetInitFlags() const
{
    return FudgetControlFlags::RegisterToUpdates | Base::GetInitFlags();
}

bool FudgetLineEdit::IsWhitespace(int index)
{
    if (index < 0 || index >= _text.Length())
        return false;
    return StringUtils::IsWhitespace(_text[index]);
}

void FudgetLineEdit::InsertCharacter(int index, Char ch)
{
    _text.Insert(index, String(&ch, 1));
}

void FudgetLineEdit::DeleteCharacters(int start_index, int end_index)
{
    _text.Remove(start_index, end_index - start_index);
}

void FudgetLineEdit::ReplaceCharacters(int start_index, int end_index, const String &with)
{
    int insert_len = end_index - start_index;
    int w_len = with.Length();
    int skipped = 0;
    if (insert_len > w_len)
        _text.Remove(start_index, insert_len - w_len);
    else if (insert_len < w_len)
    {
        skipped = w_len - insert_len;
        _text.Insert(start_index, String(with.Get(), skipped));
    }

    Platform::MemoryCopy(_text.Get() + start_index + skipped, with.Get() + skipped, sizeof(Char) * (w_len - skipped));
}

void FudgetLineEdit::ReplaceCharacters(int start_index, int end_index, Char ch)
{
    int insert_len = end_index - start_index;
    if (insert_len == 0)
    {
        _text.Insert(start_index, String(&ch, 1));
        return;
    }

    if (insert_len > 1)
        _text.Remove(start_index, insert_len - 1);

    Platform::MemoryCopy(_text.Get() + start_index, &ch, sizeof(Char));
}

