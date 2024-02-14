#include "LineEdit.h"
#include "../Styling/Themes.h"
#include "Engine/Core/Types/StringBuilder.h"

//#include "Engine/Render2D/Font.h"
//#include "Engine/Core/Types/Variant.h"


FudgetToken FudgetLineEdit::ClassToken = -1;

FudgetToken FudgetLineEdit::FramePainterToken = -1;
FudgetToken FudgetLineEdit::FrameStyleToken = -1;
FudgetToken FudgetLineEdit::TextPainterToken = -1;
FudgetToken FudgetLineEdit::TextStyleToken = -1;

FudgetToken FudgetLineEdit::CaretDrawToken = -1;
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

FudgetToken FudgetLineEdit::GetTextPainterToken()
{
    InitializeTokens();
    return TextPainterToken;
}

FudgetToken FudgetLineEdit::GetTextStyleToken()
{
    InitializeTokens();
    return TextStyleToken;
}

FudgetToken FudgetLineEdit::GetCaretDrawToken()
{
    InitializeTokens();
    return CaretDrawToken;
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


    TextPainterToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_TextPainter"));
    TextStyleToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_TextStyle"));

    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretDraw"));
    CaretBlinkTimeToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretBlinkTime"));
    CaretWidthToken = FudgetThemes::RegisterToken(TEXT("Fudgets_LineEdit_CaretWidth"));
}

FudgetLineEdit::FudgetLineEdit(const SpawnParams &params) : Base(params), _draw_state(), _frame_painter(nullptr), _text_painter(nullptr),
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

    _text_painter = CreateStylePainter<FudgetTextPainter>(TextPainterToken, text_style);

    if (!GetStyleDrawArea(CaretDrawToken, _caret_draw))
        _caret_draw = FudgetDrawArea(Color::Black);
    if (!GetStyleFloat(CaretBlinkTimeToken, _caret_blink_time))
        _caret_blink_time = 1.0f;
    if (!GetStyleFloat(CaretWidthToken, _caret_width))
        _caret_width = 2.0f;
}

void FudgetLineEdit::SetText(const String &value)
{
    _text = Process(value);
}

void FudgetLineEdit::OnDraw()
{
    _draw_state.Bounds = GetBounds();
    if (_frame_painter != nullptr)
        _frame_painter->Draw(this, _draw_state);

    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    _draw_state.Bounds = innerPadding.Padded(_draw_state.Bounds);
    PushClip(_draw_state.Bounds);

    _text_options.Text = _text;
    _text_options.Offset = Float2(-_scroll_pos, 0.0f);
    _text_options.Range.X = 0;
    _text_options.Range.Y = _text.Length();

    FudgetTextRangeSpan selection;
    Int2 sel_range;
    if (GetSelLength() != 0)
    {
        selection.RangeSpan = Span<Int2>(&sel_range, 1);
        sel_range.X = Math::Min(GetSelStart(), GetSelEnd());
        sel_range.Y = Math::Max(GetSelStart(), GetSelEnd());
        _text_options.Spans = Span<FudgetTextRangeSpan>(&selection, 1);
    }
    else
    {
        _text_options.Spans = Span<FudgetTextRangeSpan>();
    }

    _text_painter->Draw(this, _draw_state, _text_options);

    PopClip();

    if (!VirtuallyFocused())
    {
        _blink_passed = 0.0f;
        return;
    }

    // Draw caret
    if (_blink_passed < _caret_blink_time)
    {
        // TODO: get a global caret color or pick one for this edit

        float caret_left;

        _text_options.Range.Y = GetCaretPos();
        caret_left = _text_painter->Measure(this, _draw_state, _text_options).X;

        DrawArea(_caret_draw, Rectangle(Float2(caret_left - 1.0f + _draw_state.Bounds.GetLeft() - _scroll_pos, _draw_state.Bounds.Location.Y), Float2(_caret_width, _draw_state.Bounds.GetHeight())));
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
    _draw_state.Bounds = innerPadding.Padded(GetBounds());

    _text_options.Text = _text;
    _text_options.Offset = Float2(-_scroll_pos, 0.0f);
    _text_options.Range.X = 0;
    _text_options.Range.Y = _text.Length();

    return _text_painter->HitTest(this, _draw_state, _text_options, Float2(pos.X, _draw_state.Bounds.GetTop()));
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
    _draw_state.Bounds = innerPadding.Padded(GetBounds());
    _text_options.Text = _text;
    _text_options.Offset = Float2(-_scroll_pos, 0.0f);
    _text_options.Range.X = 0;
    _text_options.Range.Y = GetCaretPos();

    Float2 textSize = _text_painter->Measure(this, _draw_state, _text_options);

    if (_scroll_pos > 0.f && textSize.X - _scroll_pos < 0.f)
    {
        // Caret out towards the start of the text.

        _text_options.Range.X = 0;
        _text_options.Range.Y = Math::Max(GetCaretPos() - _character_scroll_count, 0);
        _scroll_pos = _text_painter->Measure(this, _draw_state, _text_options).X;
    }
    else if (textSize.X - _scroll_pos >= _draw_state.Bounds.GetWidth())
    {
        // Caret out towards the end of the text.

        _text_options.Range.X = GetCaretPos();
        _text_options.Range.Y = Math::Min(GetCaretPos() + _character_scroll_count, GetTextLength());
        _scroll_pos = _text_painter->Measure(this, _draw_state, _text_options).X + textSize.X - _draw_state.Bounds.GetWidth() + _caret_width * 2.0f;
    }
    else if (_scroll_pos > 0.f && textSize.X - _scroll_pos < _draw_state.Bounds.GetWidth())
    {
        // Possibly empty space on the right after end of text.

        _text_options.Range.X = GetCaretPos();
        _text_options.Range.Y = GetTextLength(); //- GetCaretPos();
        float afterSize = _text_painter->Measure(this, _draw_state, _text_options).X;
        if (textSize.X + afterSize - _scroll_pos + _caret_width * 2.0f < _draw_state.Bounds.GetWidth())
            _scroll_pos = Math::Max(0.0f, textSize.X + afterSize - _draw_state.Bounds.GetWidth() + _caret_width * 2.0f);
    }
}

void FudgetLineEdit::FixScrollPos()
{
    if (_text_painter == nullptr)
        return;

    FudgetPadding innerPadding = GetInnerPadding();
    _draw_state.Bounds = Rectangle(Float2::Zero, Float2(std::numeric_limits<float>::max() , std::numeric_limits<float>::max()));
    _text_options.Text = _text;
    _text_options.Offset = Float2::Zero;
    _text_options.Range.X = 0;
    _text_options.Range.Y = GetTextLength();

    Float2 textSize = _text_painter->Measure(this, _draw_state, _text_options);

    float w = GetInnerPadding().Padded(GetBounds()).GetWidth() + _scroll_pos;
    if (w > textSize.X)
        _scroll_pos = Math::Max(0.0f, textSize.X - w + _scroll_pos);
}

String FudgetLineEdit::Process(const String &value) const
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

    return str.ToString();
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

