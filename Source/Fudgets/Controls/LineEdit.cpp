#include "LineEdit.h"
#include "../Styling/Themes.h"
#include "Engine/Core/Types/StringBuilder.h"

FudgetToken FudgetLineEdit::FramePainterToken = -1;
FudgetToken FudgetLineEdit::FrameStyleToken = -1;
FudgetToken FudgetLineEdit::TextPainterToken = -1;
FudgetToken FudgetLineEdit::TextStyleToken = -1;

FudgetToken FudgetLineEdit::CaretDrawToken = -1;
FudgetToken FudgetLineEdit::CaretBlinkTimeToken = -1;
FudgetToken FudgetLineEdit::CaretWidthToken = -1;
FudgetToken FudgetLineEdit::CaretScrollCountToken = -1;

void FudgetLineEdit::InitializeTokens()
{
    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_FrameStyle"));


    TextPainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_TextPainter"));
    TextStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_TextStyle"));

    CaretDrawToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretDraw"));
    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretBlinkTime"));
    CaretWidthToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretWidth"));
    CaretScrollCountToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretScrollCount"));
}

FudgetLineEdit::FudgetLineEdit(const SpawnParams &params) : Base(params), _frame_painter(nullptr), _text_painter(nullptr),
    _blink_passed(0.0f), _character_scroll_count(0), _caret_blink_time(1.0f), _caret_width(2.0f), _scroll_pos(0.0f)
{

}

void FudgetLineEdit::OnInitialize()
{
    FudgetToken background_style;
    if (!GetStyleToken(FrameStyleToken, background_style))
        background_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, background_style);

    FudgetToken text_style;
    if (!GetStyleToken(TextStyleToken, text_style))
        text_style = FudgetToken::Invalid;

    _text_painter = CreateStylePainter<FudgetSingleLineTextPainter>(TextPainterToken, text_style);

    if (!GetStyleDrawArea(CaretDrawToken, _caret_draw))
        _caret_draw = FudgetDrawArea(Color::Black);
    if (!GetStyleFloat(CaretBlinkTimeToken, _caret_blink_time))
        _caret_blink_time = 1.0f;
    if (!GetStyleFloat(CaretWidthToken, _caret_width))
        _caret_width = 2.0f;
    if (!GetStyleInt(CaretScrollCountToken, _character_scroll_count))
        _character_scroll_count = 4;
}

Float2 FudgetLineEdit::GetLayoutHintSize() const
{
    Float2 size = Base::GetLayoutHintSize();
    if (_text_painter == nullptr)
        return size;

    FudgetPadding inner = GetInnerPadding();
    float h = _text_painter->GetFontHeight();
    return Float2(size.X, h + inner.Height());
}

Float2 FudgetLineEdit::GetLayoutMinSize() const
{
    if (_text_painter == nullptr)
        return Base::GetLayoutMinSize();

    FudgetPadding inner = GetInnerPadding();
    float h = _text_painter->GetFontHeight();
    return Float2(inner.Width(), h + inner.Height());
}

void FudgetLineEdit::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, GetVisualState());

    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    bounds = innerPadding.Padded(bounds);
    PushClip(bounds);

    FudgetSingleLineTextOptions options;
    //options.Text = _text;
    //options.Offset = Float2(-_scroll_pos, 0.0f);
    //options.Range.StartIndex = 0;
    //options.Range.EndIndex = _text.Length();

    bounds.Location.X -= _scroll_pos;
    bounds.Size.X += _scroll_pos;

    FudgetTextRangeSpan selection;
    FudgetTextRange sel_range;
    if (GetSelLength() != 0)
    {
        selection.RangeSpan = Span<FudgetTextRange>(&sel_range, 1);
        sel_range.StartIndex = Math::Min(GetSelStart(), GetSelEnd());
        sel_range.EndIndex = Math::Max(GetSelStart(), GetSelEnd());
        options.Spans = Span<FudgetTextRangeSpan>(&selection, 1);
    }
    else
    {
        options.Spans = Span<FudgetTextRangeSpan>();
    }

    FudgetTextRange range;
    range.StartIndex = 0;
    range.EndIndex = _text.Length();
    _text_painter->Draw(this, bounds, _text, range, GetVisualState(), options);

    PopClip();

    if (!VirtuallyFocused())
    {
        _blink_passed = 0.0f;
        return;
    }

    // Draw caret
    if (_blink_passed < _caret_blink_time)
    {
        range.EndIndex = GetCaretPos();
        float caret_left = _text_painter->Measure(this, _text, range, GetVisualState(), options).X;

        DrawArea(_caret_draw, Rectangle(Float2(caret_left - 1.0f + bounds.GetLeft(), bounds.Location.Y), Float2(_caret_width, bounds.GetHeight())));
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
    if (_text_painter == nullptr)
        return 0;

    FudgetPadding innerPadding = GetInnerPadding();
    //_draw_state.Bounds = innerPadding.Padded(GetBounds());

    Rectangle bounds = innerPadding.Padded(GetBounds());
    bounds.Location.X -= _scroll_pos;
    bounds.Size.X += _scroll_pos;

    FudgetSingleLineTextOptions options;
    //options.Text = _text;
    //options.Offset = Float2(-_scroll_pos, 0.0f);
    FudgetTextRange range;
    range.StartIndex = 0;
    range.EndIndex = _text.Length();

    return _text_painter->HitTest(this, bounds, _text, range, GetVisualState(), options, Float2(pos.X, bounds.GetTop()));
}

void FudgetLineEdit::DoPositionChanged(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    ScrollToPos();

    Base::DoPositionChanged(old_caret_pos, old_sel_pos);
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

//void FudgetLineEdit::OnFocusChanged(bool focused, FudgetControl *other)
//{
//    _draw_state.SetState(FudgetVisualControlState::Focused, focused);
//}
//
//void FudgetLineEdit::OnVirtuallyEnabledChanged()
//{
//    _draw_state.SetState(FudgetVisualControlState::Disabled, !VirtuallyEnabled());
//}

void FudgetLineEdit::OnMouseMove(Float2 pos, Float2 global_pos)
{
    Base::OnMouseMove(pos, global_pos);
}

FudgetInputResult FudgetLineEdit::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    return Base::OnMouseDown(pos, global_pos, button, double_click);
}

bool FudgetLineEdit::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    return Base::OnMouseUp(pos, global_pos, button);
}

void FudgetLineEdit::ScrollToPos()
{
    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    Rectangle bounds = innerPadding.Padded(GetBounds());

    int caret_pos = GetCaretPos();

    FudgetSingleLineTextOptions options;
    //options.Text = _text;
    FudgetTextRange range;
    range.StartIndex = 0;
    range.EndIndex = caret_pos;

    float text_width = _text_painter->Measure(this, _text, range, GetVisualState(), options).X;

    if (_scroll_pos > 0.f && text_width - _scroll_pos < 0.f)
    {
        // Caret out towards the start of the text.

        range.StartIndex = 0;
        range.EndIndex = Math::Max(caret_pos - _character_scroll_count, 0);
        _scroll_pos = _text_painter->Measure(this, _text, range, GetVisualState(), options).X;
    }
    else if (text_width - _scroll_pos >= bounds.GetWidth())
    {
        // Caret out towards the end of the text.

        range.StartIndex = caret_pos;
        range.EndIndex = Math::Min(caret_pos + _character_scroll_count, GetTextLength());
        float kerning = 0.f;
        if (caret_pos > 0 && range.EndIndex > caret_pos)
            kerning = (float)_text_painter->GetKerning(caret_pos - 1, caret_pos, 1.f);
        _scroll_pos = _text_painter->Measure(this, _text, range, GetVisualState(), options).X + text_width + kerning - bounds.GetWidth() + _caret_width * 2.0f;
    }
    else if (_scroll_pos > 0.f && text_width - _scroll_pos < bounds.GetWidth())
    {
        // Possibly empty space on the right after end of text.

        range.StartIndex = caret_pos;
        range.EndIndex = GetTextLength();
        float afterSize = _text_painter->Measure(this, _text, range, GetVisualState(), options).X;
        float kerning = 0.f;
        if (caret_pos > 0 && range.EndIndex > caret_pos)
            kerning = (float)_text_painter->GetKerning(caret_pos - 1, caret_pos, 1.f);
        if (text_width + afterSize + kerning - _scroll_pos + _caret_width * 2.0f < bounds.GetWidth())
            _scroll_pos = Math::Max(0.0f, text_width + afterSize + kerning - bounds.GetWidth() + _caret_width * 2.0f);
    }
}

void FudgetLineEdit::FixScrollPos()
{
    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    FudgetSingleLineTextOptions options;
    //options.Text = _text;
    FudgetTextRange range;
    range.StartIndex = 0;
    range.EndIndex = GetTextLength();

    Float2 textSize = _text_painter->Measure(this, _text, range, GetVisualState(), options);

    float w = GetInnerPadding().Padded(GetBounds()).GetWidth() + _scroll_pos;
    if (w > textSize.X)
        _scroll_pos = Math::Max(0.0f, textSize.X - w + _scroll_pos);
}

void FudgetLineEdit::Process(const StringView &value)
{
    // Remove unwanted characters and trim to end of line if word wrapping is disabled

    int len = value.Length();
    StringBuilder str(len);
    int pos = 0;
    bool in_text = true;
    for (int ix = 0, siz = len + 1; ix < siz && pos < len; ++ix)
    {
        if (ix == len || value[ix] == L'\n' || value[ix] == L'\r' || value[ix] == (Char)0x7f)
        {
            if (!in_text)
            {
                pos = ix + 1;
            }
            else
            {
                str.Append(StringView(value.Get() + pos, ix - pos));
                in_text = false;
                pos = ix + 1;
            }

            if (ix < len && value[ix] == L'\n')
                break;
        }
        else
        {
            in_text = true;
        }
    }

    _text = str.ToString();
}

FudgetControlFlag FudgetLineEdit::GetInitFlags() const
{
    return FudgetControlFlag::RegisterToUpdates | Base::GetInitFlags();
}

void FudgetLineEdit::SetTextInternal(const StringView &value)
{
    Process(value);
}

bool FudgetLineEdit::IsWhitespace(int index) const
{
    if (index < 0 || index >= _text.Length())
        return false;
    return StringUtils::IsWhitespace(_text[index]);
}

void FudgetLineEdit::InsertCharacter(int index, Char ch)
{
    _text.Insert(index, StringView(&ch, 1));
}

void FudgetLineEdit::DeleteCharacters(int start_index, int end_index)
{
    _text.Remove(start_index, end_index - start_index);
}

void FudgetLineEdit::ReplaceCharacters(int start_index, int end_index, const StringView &with)
{
    int insert_len = end_index - start_index;
    int w_len = with.Length();
    int skipped = 0;
    if (insert_len > w_len)
        _text.Remove(start_index, insert_len - w_len);
    else if (insert_len < w_len)
    {
        skipped = w_len - insert_len;
        _text.Insert(start_index, StringView(with.Get(), skipped));
    }

    Platform::MemoryCopy(_text.Get() + start_index + skipped, with.Get() + skipped, sizeof(Char) * (w_len - skipped));
}

void FudgetLineEdit::ReplaceCharacters(int start_index, int end_index, Char ch)
{
    int insert_len = end_index - start_index;
    if (insert_len == 0)
    {
        _text.Insert(start_index, StringView(&ch, 1));
        return;
    }

    if (insert_len > 1)
        _text.Remove(start_index, insert_len - 1);

    Platform::MemoryCopy(_text.Get() + start_index, &ch, sizeof(Char));
}

FudgetTextBoxFlags FudgetLineEdit::GetTextBoxInitFlags() const
{
    return FudgetTextBoxFlags::CaretVisible | FudgetTextBoxFlags::Editable | FudgetTextBoxFlags::WordSkip | FudgetTextBoxFlags::MouseSelectable | FudgetTextBoxFlags::KeySelectable;
}
