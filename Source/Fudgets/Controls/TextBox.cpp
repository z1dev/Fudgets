#include "TextBox.h"

#include "../Styling/Themes.h"
#include "Engine/Core/Types/StringBuilder.h"

FudgetToken FudgetTextBox::ClassToken = -1;

FudgetToken FudgetTextBox::FramePainterToken = -1;
FudgetToken FudgetTextBox::FrameStyleToken = -1;
FudgetToken FudgetTextBox::TextPainterToken = -1;
FudgetToken FudgetTextBox::TextStyleToken = -1;

FudgetToken FudgetTextBox::CaretDrawToken = -1;
FudgetToken FudgetTextBox::CaretBlinkTimeToken = -1;
FudgetToken FudgetTextBox::CaretWidthToken = -1;
FudgetToken FudgetTextBox::CaretScrollCountToken = -1;
FudgetToken FudgetTextBox::SnapTopLineToken = -1;

FudgetToken FudgetTextBox::GetClassToken()
{
    InitializeTokens();
    return ClassToken;
}

FudgetToken FudgetTextBox::GetFramePainterToken()
{
    InitializeTokens();
    return FramePainterToken;
}

FudgetToken FudgetTextBox::GetFrameStyleToken()
{
    InitializeTokens();
    return FrameStyleToken;
}

FudgetToken FudgetTextBox::GetTextPainterToken()
{
    InitializeTokens();
    return TextPainterToken;
}

FudgetToken FudgetTextBox::GetTextStyleToken()
{
    InitializeTokens();
    return TextStyleToken;
}

FudgetToken FudgetTextBox::GetCaretDrawToken()
{
    InitializeTokens();
    return CaretDrawToken;
}

FudgetToken FudgetTextBox::GetCaretBlinkTimeToken()
{
    InitializeTokens();
    return CaretBlinkTimeToken;
}
FudgetToken FudgetTextBox::GetCaretWidthToken()
{
    InitializeTokens();
    return CaretWidthToken;
}

FudgetToken FudgetTextBox::GetCaretScrollCountToken()
{
    InitializeTokens();
    return CaretScrollCountToken;
}

FudgetToken FudgetTextBox::GetSnapTopLineToken()
{
    InitializeTokens();
    return SnapTopLineToken;
}

void FudgetTextBox::InitializeTokens()
{
    if (ClassToken != FudgetToken::Invalid)
        return;

    ClassToken = FudgetThemes::RegisterToken(TEXT("FudgetTextBox"));

    FramePainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_FramePainter"));
    FrameStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_FrameStyle"));


    TextPainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_TextPainter"));
    TextStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_TextStyle"));

    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_CaretDraw"));
    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_CaretBlinkTime"));
    CaretWidthToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_CaretWidth"));
    CaretScrollCountToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_CaretScrollCount"));

    SnapTopLineToken = FudgetThemes::RegisterToken(TEXT("Fudgets_TextBox_SnapTopLine"));
}

FudgetTextBox::FudgetTextBox(const SpawnParams &params) : Base(params), _draw_state(), _frame_painter(nullptr), _text_painter(nullptr),
    _text_measurements(), _blink_passed(0.0f), _character_scroll_count(0), _snap_top_line(true), _caret_blink_time(1.0f), _caret_width(2.0f),
    _caret_updown_x(-1.f), _scroll_pos(0.0f), _line_wrap(false), _wrap_mode(FudgetLineWrapMode::Whitespace), _draw_ellipsis(false)
{

}

void FudgetTextBox::OnInitialize()
{
    FudgetToken background_style;
    if (!GetStyleToken(FrameStyleToken, background_style))
        background_style = FudgetToken::Invalid;

    _frame_painter = CreateStylePainter<FudgetFramedFieldPainter>(FramePainterToken, background_style);

    FudgetToken text_style;
    if (!GetStyleToken(TextStyleToken, text_style))
        text_style = FudgetToken::Invalid;

    _text_painter = CreateStylePainter<FudgetMultiLineTextPainter>(TextPainterToken, text_style);

    if (!GetStyleDrawArea(CaretDrawToken, _caret_draw))
        _caret_draw = FudgetDrawArea(Color::Black);
    if (!GetStyleFloat(CaretBlinkTimeToken, _caret_blink_time))
        _caret_blink_time = 1.0f;
    if (!GetStyleFloat(CaretWidthToken, _caret_width))
        _caret_width = 2.0f;
    if (!GetStyleInt(CaretScrollCountToken, _character_scroll_count))
        _character_scroll_count = 4;

    if (!GetStyleBool(SnapTopLineToken, _snap_top_line))
        _snap_top_line = true;
}

void FudgetTextBox::OnDraw()
{
    Rectangle bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, bounds, _draw_state);

    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    bounds = innerPadding.Padded(bounds);
    PushClip(bounds);

    FudgetMultiLineTextOptions options;
    options.Scale = 1.0f;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;

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

    bounds.Location -= _scroll_pos;
    bounds.Size += _scroll_pos;

    _text_painter->Draw(this, bounds, _text, _draw_state, options, _text_measurements);

    PopClip();

    if (!VirtuallyFocused())
    {
        _blink_passed = 0.0f;
        return;
    }

    // Draw caret
    if (_blink_passed < _caret_blink_time)
    {
        Float2 caret_pos = _text_painter->GetCharacterPosition(this, bounds, _text, _draw_state, options, _text_measurements, GetCaretPos());

        DrawArea(_caret_draw, Rectangle(caret_pos - Float2(1.0f, 0.f),
            Float2(_caret_width, _text_painter->GetCharacterLineHeight(_text, options, _text_measurements, GetCaretPos()) )));
    }
    while (_blink_passed >= _caret_blink_time * 2.0f)
        _blink_passed -= _caret_blink_time * 2.0f;
}

void FudgetTextBox::OnSizeChanged()
{
    ScrollToPos();
}

FudgetPadding FudgetTextBox::GetInnerPadding() const
{
    return _frame_painter != nullptr ? _frame_painter->GetContentPadding() : FudgetPadding(0.0f);
}

int FudgetTextBox::CharIndexAt(Float2 pos)
{
    if (_text_painter == nullptr)
        return 0;

    FudgetPadding innerPadding = GetInnerPadding();
    Rectangle bounds = innerPadding.Padded(GetBounds());

    FudgetMultiLineTextOptions options;
    options.Scale = 1.0f;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;

    bounds.Location -= _scroll_pos;
    bounds.Size += _scroll_pos;

    return _text_painter->HitTest(this, bounds, _text, _draw_state, options, _text_measurements, pos);
}

void FudgetTextBox::DoPositionChanged(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    if (CaretChangeReason != FudgetTextBoxCaretChangeReason::Up && CaretChangeReason != FudgetTextBoxCaretChangeReason::Down &&
        CaretChangeReason != FudgetTextBoxCaretChangeReason::PageUp && CaretChangeReason != FudgetTextBoxCaretChangeReason::PageDown)
        _caret_updown_x = -1.f;

    ScrollToPos();

    Base::DoPositionChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBox::DoTextEdited(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    _caret_updown_x = -1.f;
    Process();

    FixScrollPos();
    ScrollToPos();
    Base::DoTextEdited(old_caret_pos, old_sel_pos);
}

void FudgetTextBox::OnUpdate(float delta_time)
{
    _blink_passed += delta_time;
}

void FudgetTextBox::OnFocusChanged(bool focused, FudgetControl *other)
{
    _draw_state.SetState(FudgetFramedFieldState::Focused, focused);
}

void FudgetTextBox::OnVirtuallyEnabledChanged()
{
    _draw_state.SetState(FudgetFramedFieldState::Disabled, !VirtuallyEnabled());
}

void FudgetTextBox::OnMouseMove(Float2 pos, Float2 global_pos)
{
    Base::OnMouseMove(pos, global_pos);
}

FudgetInputResult FudgetTextBox::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    return Base::OnMouseDown(pos, global_pos, button, double_click);
}

bool FudgetTextBox::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    return Base::OnMouseUp(pos, global_pos, button);
}

void FudgetTextBox::SetLineWrap(bool value)
{
    if (_line_wrap == value)
        return;
    _line_wrap = value;
    Process();
}

void FudgetTextBox::SetLineWrapMode(FudgetLineWrapMode value)
{
    if (!_line_wrap)
    {
        _wrap_mode = value;
        return;
    }
    _wrap_mode = value;
    Process();
}

void FudgetTextBox::ScrollToPos()
{
    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    Rectangle bounds = innerPadding.Padded(GetBounds());

    int caret_pos = GetCaretPos();

    int line_index = Math::Min(_text_measurements.Lines.Count() - 1, _text_painter->GetCharacterLine(_text_measurements, caret_pos));
    if (line_index < 0)
        return;

    const FudgetLineMeasurements &line = _text_measurements.Lines[line_index];

    FudgetMultiLineTextOptions options;

    float text_width = line.Size.X;
    float text_caret_width = _text_painter->Measure(this, StringView(_text.Get() + line.StartIndex, Math::Max(0, caret_pos - line.StartIndex)), _draw_state, 1.0f).X;

    if (_scroll_pos.X > 0.f && text_caret_width - _scroll_pos.X < 0.f)
    {
        // Caret out towards the start of the text.
        _scroll_pos.X = _text_painter->Measure(this, StringView(_text.Get() + line.StartIndex, Math::Max(0, caret_pos - _character_scroll_count - line.StartIndex)), _draw_state, 1.f).X;
    }
    else if (text_caret_width - _scroll_pos.X >= bounds.GetWidth())
    {
        // Caret out towards the end of the text.

        //range.StartIndex = caret_pos;
        //range.EndIndex = Math::Min(caret_pos + _character_scroll_count, GetTextLength());
        int end_index = Math::Min(caret_pos + _character_scroll_count, line.EndIndex);

        float kerning = 0.f;
        if (caret_pos > 0 && end_index > caret_pos)
            kerning = (float)_text_painter->GetKerning(caret_pos - 1, caret_pos, 1.f);
        _scroll_pos.X = _text_painter->Measure(this, StringView(_text.Get() + caret_pos, end_index - caret_pos), _draw_state, 1.f).X + text_caret_width + kerning - bounds.GetWidth() + _caret_width * 2.0f;
    }
    else if (_scroll_pos.X > 0.f && text_caret_width - _scroll_pos.X < bounds.GetWidth())
    {
        // Possibly empty space on the right after end of text.

         if (text_width - _scroll_pos.X + _caret_width * 2.0f < bounds.GetWidth())
            _scroll_pos.X = Math::Max(0.0f, text_width - bounds.GetWidth() + _caret_width * 2.0f);
    }

    if (line.Location.Y < _scroll_pos.Y)
        _scroll_pos.Y = line.Location.Y;
    else if (line.Location.Y + line.Size.Y > bounds.GetHeight() + _scroll_pos.Y)
    {
        if (_snap_top_line)
        {
            float height = bounds.GetHeight();
            int ix = line_index;
            float bottom = line.Location.Y + line.Size.Y;
            float top = line.Location.Y;

            while (ix > 0 && bottom - top <= height)
                top = _text_measurements.Lines[--ix].Location.Y;
            if (bottom - top <= height || ix == line_index)
                _scroll_pos.Y = _text_measurements.Lines[ix].Location.Y;
            else
                _scroll_pos.Y = _text_measurements.Lines[Math::Min(_text_measurements.Lines.Count() - 1, ix + 1)].Location.Y;
        }
        else
            _scroll_pos.Y = Math::Max(0.f, line.Location.Y + line.Size.Y - bounds.GetHeight());
    }
}

void FudgetTextBox::FixScrollPos()
{
    if (_text_painter == nullptr)
        return;

 /*   FudgetPadding innerPadding = GetInnerPadding();
    _draw_state.Bounds = Rectangle(Float2::Zero, Float2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
    FudgetPainterTextDrawOptions options;
    options.Text = _text;
    options.Offset = Float2::Zero;
    options.Range.StartIndex = 0;
    options.Range.EndIndex = GetTextLength();

    Float2 textSize = _text_painter->Measure(this, _draw_state, options);

    float w = GetInnerPadding().Padded(GetBounds()).GetWidth() + _scroll_pos.X;
    if (w > textSize.X)
        _scroll_pos = Math::Max(0.0f, textSize.X - w + _scroll_pos.X);*/
}

void FudgetTextBox::Process()
{
    Rectangle bounds = GetInnerPadding().Padded(GetBounds());

    bounds.Location = Float2::Zero;

    FudgetMultiLineTextOptions options;
    options.Scale = 1.0f;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;
    _text_painter->MeasureLines(this, bounds, _text, options, _text_measurements);
}

FudgetControlFlags FudgetTextBox::GetInitFlags() const
{
    return FudgetControlFlags::RegisterToUpdates | Base::GetInitFlags();
}

void FudgetTextBox::SetTextInternal(const String &value)
{
    _text = value;
    Process();
    SetCaretPos(GetTextLength());
}

bool FudgetTextBox::IsWhitespace(int index) const
{
    if (index < 0 || index >= _text.Length())
        return false;
    return StringUtils::IsWhitespace(_text[index]);
}

void FudgetTextBox::InsertCharacter(int index, Char ch)
{
    _text.Insert(index, String(&ch, 1));
}

void FudgetTextBox::DeleteCharacters(int start_index, int end_index)
{
    _text.Remove(start_index, end_index - start_index);
}

void FudgetTextBox::ReplaceCharacters(int start_index, int end_index, const String &with)
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

void FudgetTextBox::ReplaceCharacters(int start_index, int end_index, Char ch)
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

int FudgetTextBox::GetCaretPosUp()
{
    if (_text_painter == nullptr)
        return GetCaretPos();

    FudgetPadding innerPadding = GetInnerPadding();
    Rectangle bounds = innerPadding.Padded(GetBounds());
    bounds.Location -= _scroll_pos;
    bounds.Size += _scroll_pos;

    FudgetMultiLineTextOptions options;
    options.Scale = 1.0f;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;

    Float2 caret_pos = _text_painter->GetCharacterPosition(this, bounds, _text, _draw_state, options, _text_measurements, GetCaretPos());
    if (_caret_updown_x < 0)
        _caret_updown_x = caret_pos.X - bounds.GetLeft();
    caret_pos.X = _caret_updown_x + bounds.GetLeft();
    int line_index = _text_painter->GetCharacterLine(_text_measurements, GetCaretPos());
    float line_height = _text_measurements.Lines.Count() > 0 ? _text_measurements.Lines[Math::Min(line_index, _text_measurements.Lines.Count() - 1)].Size.Y : 0.f;
    return _text_painter->HitTest(this, bounds, _text, _draw_state, options, _text_measurements, caret_pos - Float2(0.f, 1.f));
}

int FudgetTextBox::GetCaretPosDown()
{
    if (_text_painter == nullptr)
        return GetCaretPos();

    FudgetPadding innerPadding = GetInnerPadding();
    Rectangle bounds = innerPadding.Padded(GetBounds());
    bounds.Location -= _scroll_pos;
    bounds.Size += _scroll_pos;

    FudgetMultiLineTextOptions options;
    options.Scale = 1.0f;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;

    Float2 caret_pos = _text_painter->GetCharacterPosition(this, bounds, _text, _draw_state, options, _text_measurements, GetCaretPos());
    if (_caret_updown_x < 0)
        _caret_updown_x = caret_pos.X - bounds.GetLeft();
    caret_pos.X = _caret_updown_x + bounds.GetLeft();
    int line_index = _text_painter->GetCharacterLine(_text_measurements, GetCaretPos());
    float line_height = _text_measurements.Lines.Count() > 0 ? _text_measurements.Lines[Math::Min(line_index, _text_measurements.Lines.Count() - 1)].Size.Y : 0.f;
    return _text_painter->HitTest(this, bounds, _text, _draw_state, options, _text_measurements, caret_pos + Float2(0.f, line_height + 1.f));
}

int FudgetTextBox::GetCaretPosPageUp()
{
    return GetCaretPos();
}

int FudgetTextBox::GetCaretPosPageDown()
{
    return GetCaretPos();
}

