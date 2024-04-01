#include "TextBox.h"

#include "../Styling/Themes.h"
#include "../Layouts/Layout.h"
#include "../Styling/Painters/DrawablePainter.h"
#include "../Styling/PartPainterIds.h"
#include "../Styling/DrawableBuilder.h"

#include "Engine/Core/Types/StringBuilder.h"
#include "Engine/Platform/Base/WindowBase.h"



FudgetTextBox::FudgetTextBox(const SpawnParams &params) : Base(params), /*_frame_painter(nullptr), */_text_painter(nullptr),
    _text_measurements(), _blink_passed(0.0f), _show_beam_cursor(false), _beam_cursor(CursorType::Default), _character_scroll_count(0),
    _snap_top_line(false), _sizing_mode(FudgetTextBoxSizingMode::Normal), _caret_draw(nullptr), _caret_blink_time(1.0f), _caret_width(2), _caret_updown_x(-1),
    _scroll_pos(0), _lines_dirty(false), _line_wrap(false), _wrap_mode(FudgetLineWrapMode::Whitespace),
    _auto_size(FudgetAutoSizing::None), _measure_space(-1), _cached_size(-1)
{
    SetScrollBars(FudgetScrollBars::Both);
}

FudgetTextBox::~FudgetTextBox()
{
}


void FudgetTextBox::OnInitialize()
{    
}

void FudgetTextBox::OnStyleInitialize()
{
    Base::OnStyleInitialize();

    _text_painter = CreateStylePainter<FudgetMultiLineTextPainter>(_text_painter, (int)FudgetMultilineTextFieldPartIds::TextPainter);

    if (!GetStylePadding((int)FudgetFieldPartIds::Padding, _content_padding))
        _content_padding = FudgetPadding(0);
    if (!GetStyleDrawable((int)FudgetTextFieldPartIds::CaretDraw, nullptr, _caret_draw))
        _caret_draw = FudgetDrawable::FromColor(this, nullptr, Color::Black);
    if (!GetStyleFloat((int)FudgetTextFieldPartIds::CaretBlinkTime, _caret_blink_time))
        _caret_blink_time = 1.0f;
    if (!GetStyleInt((int)FudgetTextFieldPartIds::CaretWidth, _caret_width))
        _caret_width = 2;
    if (!GetStyleInt((int)FudgetTextFieldPartIds::CaretScrollCount, _character_scroll_count))
        _character_scroll_count = 4;

    if (!GetStyleEnum<CursorType>((int)FudgetTextFieldPartIds::BeamCursor, _beam_cursor))
        _beam_cursor = CursorType::IBeam;

    if (!GetStyleBool((int)FudgetMultilineTextFieldPartIds::SnapTopLine, _snap_top_line))
        _snap_top_line = false;

    MarkTextDirty();
}

void FudgetTextBox::OnDraw()
{
    Base::OnDraw();
    //if (_frame_painter != nullptr)
    //    _frame_painter->Draw(this, bounds, GetVisualState());

    if (_text_painter == nullptr)
        return;

    FudgetPadding inner_padding = GetCombinedPadding();
    Rectangle bounds = inner_padding.Padded(GetBounds());
    PushClip(bounds);

    FudgetMultiLineTextOptions options;
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

    _text_painter->Draw(this, bounds, -_scroll_pos, GetVisualState(), options, GetMeasurements());

    PopClip();

    if (!VirtuallyFocused())
    {
        _blink_passed = 0.0f;
        return;
    }

    // Draw caret
    if (_blink_passed < _caret_blink_time)
    {
        Int2 caret_pos = _text_painter->GetCharacterPosition(this, GetMeasurements(), GetCaretPos()) + bounds.GetUpperLeft() - _scroll_pos;

        PushClip(GetFramePadding().Padded(GetBounds()));
        DrawDrawable(_caret_draw, 0, Rectangle(Float2(caret_pos) - Float2(1.f, 0.f),
            Int2(_caret_width, _text_painter->GetCharacterLineHeight(GetMeasurements(), GetCaretPos()) )));
        PopClip();
    }
    while (_blink_passed >= _caret_blink_time * 2.0f)
        _blink_passed -= _caret_blink_time * 2.0f;
}

void FudgetTextBox::OnSizeChanged()
{
    ScrollToPos();
}

//FudgetPadding FudgetTextBox::GetInnerPadding() const
//{
//    return _frame_painter != nullptr ? _frame_painter->GetCombinedPadding() : FudgetPadding(0);
//}

int FudgetTextBox::CharIndexAt(Int2 pos)
{
    if (_text_painter == nullptr)
        return 0;

    FudgetPadding inner_padding = GetCombinedPadding();
    Rectangle bounds = inner_padding.Padded(GetBounds());

    return _text_painter->HitTest(this, GetMeasurements(), pos - bounds.GetUpperLeft() + _scroll_pos);
}

void FudgetTextBox::DoPositionChanged(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    if (CaretChangeReason != FudgetTextBoxCaretChangeReason::Up && CaretChangeReason != FudgetTextBoxCaretChangeReason::Down &&
        CaretChangeReason != FudgetTextBoxCaretChangeReason::PageUp && CaretChangeReason != FudgetTextBoxCaretChangeReason::PageDown)
        _caret_updown_x = -1;

    ScrollToPos();

    Base::DoPositionChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBox::DoTextEdited(int old_caret_pos, int old_sel_pos)
{
    _blink_passed = 0.0f;
    _caret_updown_x = -1;

    MarkTextDirty();

    if (_auto_size != FudgetAutoSizing::None)
        SizeModified();

    Base::DoTextEdited(old_caret_pos, old_sel_pos);
}

void FudgetTextBox::OnUpdate(float delta_time)
{
    _blink_passed += delta_time;
}

//void FudgetTextBox::OnFocusChanged(bool focused, FudgetControl *other)
//{
//    _draw_state.SetState(FudgetVisualControlState::Focused, focused);
//}
//
//void FudgetTextBox::OnVirtuallyEnabledChanged()
//{
//    _draw_state.SetState(FudgetVisualControlState::Disabled, !VirtuallyEnabled());
//}

void FudgetTextBox::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (!MouseIsCaptured())
    {
        Rectangle bounds = GetCombinedPadding().Padded(GetBounds());
        _show_beam_cursor = RectContains(bounds, pos);
    }
    Base::OnMouseMove(pos, global_pos);
}

FudgetInputResult FudgetTextBox::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    Rectangle bounds = GetCombinedPadding().Padded(GetBounds());
    if (!RectContains(bounds, pos))
        return FudgetInputResult::Ignore;

    return Base::OnMouseDown(pos, global_pos, button, double_click);
}

bool FudgetTextBox::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    return Base::OnMouseUp(pos, global_pos, button);
}

void FudgetTextBox::OnMouseLeave()
{
    Base::OnMouseLeave();
}

CursorType FudgetTextBox::GetContextCursor() const
{
    CursorType cursor = GetCursor();
    if (cursor != CursorType::Default)
        return cursor;

    if (_show_beam_cursor)
        return _beam_cursor;
    return CursorType::Default;
}

Int2 FudgetTextBox::GetLayoutHintSize() const
{
    if (_auto_size != FudgetAutoSizing::None)
    {
        switch (_auto_size)
        {
            case FudgetAutoSizing::Both:
                return _cached_size;
                break;
            case FudgetAutoSizing::Height:
                return Int2(Base::GetHintSize().X, Math::Max(GetMinSize().Y, _cached_size.Y));
                break;
            case FudgetAutoSizing::Width:
                return Int2(Math::Max(GetMinSize().X, _cached_size.X), Base::GetHintSize().Y);
                break;
        }
    }

    return  Base::GetHintSize();
}

Int2 FudgetTextBox::GetLayoutMinSize() const
{
    return Base::GetMinSize();
}

Int2 FudgetTextBox::GetLayoutMaxSize() const
{
    return Base::GetMaxSize();
}

void FudgetTextBox::OnScrollBarScroll(FudgetScrollBarComponent *scrollbar, int64 old_scroll_pos, bool tracking)
{
    if (_text_painter == nullptr)
        return;

    if (scrollbar == GetHorizontalScrollBar())
    {
        _scroll_pos.X = (int)GetHorizontalScrollBar()->GetScrollPos();
    }
    else if (scrollbar == GetVerticalScrollBar())
    {
        _scroll_pos.Y = (int)GetVerticalScrollBar()->GetScrollPos();
        if (_snap_top_line)
        {
            int lix = _text_painter->LineAtPos(this, GetMeasurements(), _scroll_pos.Y);
            _scroll_pos.Y = GetMeasurements().Lines[lix].Location.Y;
        }
    }
}

bool FudgetTextBox::OnMeasure(Int2 available, API_PARAM(Out) Int2 &wanted, API_PARAM(Out) Int2 &min_size, API_PARAM(Out) Int2 &max_size)
{
    if (_auto_size == FudgetAutoSizing::None || _text_painter == nullptr)
        return Base::OnMeasure(available, wanted, min_size, max_size);

    if ((!FudgetLayout::IsUnrestrictedSpace(available) && _measure_space == available.X) ||
        ((FudgetLayout::IsUnrestrictedSpace(available) || !_line_wrap || _auto_size == FudgetAutoSizing::Both || _auto_size == FudgetAutoSizing::Width) && _measure_space == MAX_int32))
        return Base::OnMeasure(available, wanted, min_size, max_size);

    FudgetPadding inner_padding = GetCombinedPadding();
    //inner_padding.Right += _caret_width * 2;

    if (FudgetLayout::IsUnrestrictedSpace(available) || !_line_wrap || _auto_size == FudgetAutoSizing::Both || _auto_size == FudgetAutoSizing::Width)
    {
        _measure_space = MAX_int32;
        FudgetMultilineTextMeasurements tmp;
        FudgetMultiLineTextOptions opt;
        opt.Wrapping = false;
        opt.WrapMode = _wrap_mode;
        _text_painter->MeasureLines(this, MAX_int32, _text, 1.f, opt, tmp);
        _cached_size = tmp.Size + inner_padding.Size();
    }
    else
    {
        _measure_space = available.X;
        FudgetMultilineTextMeasurements tmp;
        FudgetMultiLineTextOptions opt;
        opt.Wrapping = _line_wrap;
        opt.WrapMode = _wrap_mode;
        _text_painter->MeasureLines(this, available.X, _text, 1.f, opt, tmp);
        _cached_size = tmp.Size + inner_padding.Size();
    }

    return Base::OnMeasure(available, wanted, min_size, max_size);
}

void FudgetTextBox::SetLineWrap(bool value)
{
    if (_line_wrap == value)
        return;
    _line_wrap = value;
    
    MarkTextDirty();
}

void FudgetTextBox::SetLineWrapMode(FudgetLineWrapMode value)
{
    if (!_line_wrap)
    {
        _wrap_mode = value;
        return;
    }
    _wrap_mode = value;

    MarkTextDirty();
}

void FudgetTextBox::SetSnapTopLine(bool value)
{
    if (value == _snap_top_line)
        return;
    _snap_top_line = value;
    if (value)
        SnapTopLine();

    FixScrollPos();
    ScrollToPos();
}

bool FudgetTextBox::SizeDependsOnSpace() const
{
    return _line_wrap && _auto_size != FudgetAutoSizing::None;
}

void FudgetTextBox::SetAutoSize(FudgetAutoSizing value)
{
    if (_auto_size == value)
        return;
    _auto_size = value;
    SizeModified();
}

void FudgetTextBox::MarkTextDirty()
{
    _lines_dirty = true;

    _measure_space = -1;
    if (_auto_size != FudgetAutoSizing::None)
        SizeModified();

    MarkExtentsDirty();
}

void FudgetTextBox::ScrollToPos()
{
    if (_text_painter == nullptr)
        return;

    FudgetPadding inner_padding = GetCombinedPadding();
    Rectangle bounds = inner_padding.Padded(GetBounds());

    int caret_pos = GetCaretPos();

    int line_index = Math::Min(GetMeasurements().Lines.Count() - 1, _text_painter->GetCharacterLine(GetMeasurements(), caret_pos));
    if (line_index < 0)
        return;

    const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];

    int text_width = line.Size.X;
    int text_caret_width = _text_painter->Measure(this, StringView(_text.Get() + line.StartIndex, Math::Max(0, caret_pos - line.StartIndex)), 1.f).X;

    if (_scroll_pos.X > 0.f && text_caret_width - _scroll_pos.X < 0.f)
    {
        // Caret out towards the start of the text.
        _scroll_pos.X = _text_painter->Measure(this, StringView(_text.Get() + line.StartIndex, Math::Max(0, caret_pos - _character_scroll_count - line.StartIndex)), 1.f).X;
    }
    else if (text_caret_width - _scroll_pos.X >= bounds.GetWidth())
    {
        // Caret out towards the end of the text.

        int end_index = Math::Min(caret_pos + _character_scroll_count, line.EndIndex);

        int kerning = 0;
        if (caret_pos > 0 && end_index > caret_pos)
            kerning = _text_painter->GetKerning(caret_pos - 1, caret_pos, 1.f);
        _scroll_pos.X = int(_text_painter->Measure(this, StringView(_text.Get() + caret_pos, end_index - caret_pos), 1.f).X + text_caret_width + kerning - bounds.GetWidth());
    }
    else if (_scroll_pos.X > 0.f && GetMeasurements().Size.X - _scroll_pos.X < bounds.GetWidth())
    {
        // Empty space on the right after end of text.

        _scroll_pos.X = int(Math::Max(0.f, GetMeasurements().Size.X - bounds.GetWidth()));
    }

    if (line.Location.Y < _scroll_pos.Y)
        _scroll_pos.Y = line.Location.Y;
    else if (line.Location.Y + line.Size.Y > bounds.GetHeight() + _scroll_pos.Y)
    {
        if (_snap_top_line)
        {
            int height = int(bounds.GetHeight());
            int ix = line_index;
            int bottom = line.Location.Y + line.Size.Y;
            int top = line.Location.Y;

            while (ix > 0 && bottom - top <= height)
                top = GetMeasurements().Lines[--ix].Location.Y;
            if (bottom - top <= height || ix == line_index)
                _scroll_pos.Y = GetMeasurements().Lines[ix].Location.Y;
            else
                _scroll_pos.Y = GetMeasurements().Lines[Math::Min(GetMeasurements().Lines.Count() - 1, ix + 1)].Location.Y;
        }
        else
            _scroll_pos.Y = int(Math::Max(0.f, line.Location.Y + line.Size.Y - bounds.GetHeight()));
    }

    if (GetHorizontalScrollBar() != nullptr)
        GetHorizontalScrollBar()->SetScrollPos(_scroll_pos.X);
    if (GetVerticalScrollBar() != nullptr)
        GetVerticalScrollBar()->SetScrollPos(_scroll_pos.Y);
}

void FudgetTextBox::FixScrollPos()
{
    if (_text_painter == nullptr || _scroll_pos == Int2::Zero)
        return;

    FudgetPadding inner_padding = GetCombinedPadding();
    Rectangle bounds = inner_padding.Padded(GetBounds());

    Int2 extra = bounds.Size - (GetMeasurements().Size - _scroll_pos);
    if (extra.X > 0 && _scroll_pos.X > 0)
        _scroll_pos.X = Math::Max(_scroll_pos.X - extra.X, 0);
    if (extra.Y > 0 && _scroll_pos.Y > 0)
    {
        if (!_snap_top_line)
            _scroll_pos.Y = Math::Max(_scroll_pos.Y - extra.Y, 0);
        else
        {
            int ix = _text_painter->LineAtPos(this, GetMeasurements(), _scroll_pos.Y);
            while (ix > 0)
            {
                FudgetLineMeasurements line = GetMeasurements().Lines[ix - 1];
                if (line.Size.Y > extra.Y)
                    break;
                --ix;
                extra.Y -= line.Size.Y;
                _scroll_pos.Y -= line.Size.Y;
            }
        }
    }

    if (GetHorizontalScrollBar() != nullptr)
        GetHorizontalScrollBar()->SetScrollPos(_scroll_pos.X);
    if (GetVerticalScrollBar() != nullptr)
        GetVerticalScrollBar()->SetScrollPos(_scroll_pos.Y);
}

void FudgetTextBox::Process()
{
    if (_text_painter == nullptr)
        return;

    _lines_dirty = false;

    Rectangle bounds = GetCombinedPadding().Padded(GetBounds());
    //bounds.Size.X -= _caret_width * 2.0f;

    FudgetMultiLineTextOptions options;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;
    _text_painter->MeasureLines(this, (int)bounds.GetWidth(), _text, 1.f, options, _text_measurements);

    if (_snap_top_line)
        SnapTopLine();

    FixScrollPos();
    ScrollToPos();
}

void FudgetTextBox::SnapTopLine()
{
    if (GetMeasurements().Lines.Count() != 0)
    {
        int line_index = _text_painter->LineAtPos(this, GetMeasurements(), _scroll_pos.Y);
        if (line_index >= 0 && line_index < GetMeasurements().Lines.Count())
        {
            const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];
            _scroll_pos.Y = line.Location.Y;
        }
    }

    if (GetVerticalScrollBar() != nullptr)
        GetVerticalScrollBar()->SetScrollPos(_scroll_pos.Y);
}

FudgetControlFlag FudgetTextBox::GetInitFlags() const
{
    return FudgetControlFlag::RegisterToUpdates | FudgetControlFlag::Framed | Base::GetInitFlags();
}

void FudgetTextBox::SetTextInternal(const StringView &value)
{
    _text = value;

    MarkTextDirty();

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
    _text.Insert(index, StringView(&ch, 1));
}

void FudgetTextBox::DeleteCharacters(int start_index, int end_index)
{
    _text.Remove(start_index, end_index - start_index);
}

void FudgetTextBox::ReplaceCharacters(int start_index, int end_index, const StringView &with)
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

void FudgetTextBox::ReplaceCharacters(int start_index, int end_index, Char ch)
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

int FudgetTextBox::GetCaretPosUp()
{
    int caret_pos = GetCaretPos();
    if (_text_painter == nullptr || GetMeasurements().Lines.Count() == 0)
        return caret_pos;

    if (_caret_updown_x < 0)
        _caret_updown_x = _text_painter->GetCharacterPosition(this, GetMeasurements(), caret_pos).X;

    int line_index = _text_painter->GetCharacterLine(GetMeasurements(), caret_pos);
    if (line_index < 1 || line_index >= GetMeasurements().Lines.Count())
        return caret_pos;
    const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];

    int char_index = _text_painter->LineHitTest(this, GetMeasurements(), line_index - 1, _caret_updown_x);

    // When placing line breaks after any character, the next line usually starts on the same character index where the pervious
    // one ended. In this case it's possible that instead of going up one line, this would move the caret to the end of two lines
    // above. To avoid that, make sure the index is changed to match the correct line.
    if (line_index > 1)
    {
        const FudgetLineMeasurements &prev_line = GetMeasurements().Lines[line_index - 1];
        const FudgetLineMeasurements &prev_prev_line = GetMeasurements().Lines[line_index - 2];
        if (char_index == prev_line.StartIndex && prev_line.StartIndex == prev_prev_line.EndIndex)
            char_index = Math::Min(char_index + 1, prev_line.EndIndex);
    }
    // For same reason as above, when lines end on same index as the next one starts on, the index found might move the caret one
    // line down.
    char_index = Math::Min(char_index, caret_pos);

    return char_index;
}

int FudgetTextBox::GetCaretPosDown()
{
    int caret_pos = GetCaretPos();
    if (_text_painter == nullptr || GetMeasurements().Lines.Count() == 0)
        return caret_pos;

    FudgetMultiLineTextOptions options;
    options.Wrapping = _line_wrap;
    options.WrapMode = _wrap_mode;

    if (_caret_updown_x < 0)
        _caret_updown_x = _text_painter->GetCharacterPosition(this, GetMeasurements(), caret_pos).X;

    int line_index = _text_painter->GetCharacterLine(GetMeasurements(), caret_pos);
    if (line_index < 0 || line_index >= GetMeasurements().Lines.Count() - 1)
        return caret_pos;
    const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];

    int char_index = _text_painter->LineHitTest(this, GetMeasurements(), line_index + 1, _caret_updown_x);

    // When placing line breaks after any character, the next line usually starts on the same character index where the pervious
    // one ended. In this case it's possible that instead of going down, this would move the caret to the end of the current line.
    // To avoid that, make sure the index is changed to match the correct line.
    if (char_index == line.EndIndex && line_index < GetMeasurements().Lines.Count() - 1)
    {
        const FudgetLineMeasurements &next_line = GetMeasurements().Lines[line_index + 1];
        if (line.EndIndex == next_line.StartIndex)
        char_index = Math::Min(char_index + 1, next_line.EndIndex);
    }
    // For same reason as above, when lines end on same index as the next one starts on, the index found might move the caret one
    // line up. This can only happen on the very last line of the text.
    char_index = Math::Max(char_index, caret_pos);
        
    return char_index;
}

int FudgetTextBox::GetCaretPosPageUp()
{
    int caret_pos = GetCaretPos();
    if (_text_painter == nullptr || GetMeasurements().Lines.Count() == 0)
        return caret_pos;

    if (_caret_updown_x < 0)
        _caret_updown_x = _text_painter->GetCharacterPosition(this, GetMeasurements(), caret_pos).X;

    int line_index = _text_painter->GetCharacterLine(GetMeasurements(), caret_pos);
    if (line_index < 1 || line_index >= GetMeasurements().Lines.Count())
        return caret_pos;

    FudgetPadding inner_padding = GetCombinedPadding();
    int height = (int)inner_padding.Padded(GetBounds()).GetHeight();

    const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];

    // TODO: make this an option?
    // Measure one line height less
    height = Math::Max(Math::Max(1, (int)(line.Size.Y * .5f)), height - line.Size.Y);
    Int2 test_pos = Int2(_caret_updown_x, Math::Max(0, line.Location.Y + Math::Max(1, (int)(line.Size.Y * .5f)) - height));

    _scroll_pos.Y = Math::Max(0, _scroll_pos.Y - height);

    int char_index = _text_painter->HitTest(this, GetMeasurements(), test_pos);

    // When placing line breaks after any character, the next line usually starts on the same character index where the pervious
    // one ended. In this case it's possible that instead of going up one page, this would move the caret to the end of two lines
    // above. To avoid that, make sure the index is changed to match the correct line.

    int char_line = _text_painter->GetCharacterLine(GetMeasurements(), char_index);
    const FudgetLineMeasurements &dest_line = GetMeasurements().Lines[char_line];

    if (char_index == dest_line.EndIndex && char_line < line_index - 1 && GetMeasurements().Lines[char_line + 1].StartIndex == char_index)
    {
        const FudgetLineMeasurements &other_dest_line = GetMeasurements().Lines[char_line + 1];

        int end_dist = Math::Abs(_caret_updown_x - dest_line.Location.X - dest_line.Size.X);
        int start_dist = Math::Abs(_caret_updown_x - other_dest_line.Location.X);
        if (start_dist < end_dist)
            char_index++;
    } else if (char_index == dest_line.StartIndex && char_line > 0 && GetMeasurements().Lines[char_line - 1].EndIndex == char_index)
    {
        const FudgetLineMeasurements &other_dest_line = GetMeasurements().Lines[char_line - 1];

        int start_dist = Math::Abs(_caret_updown_x - dest_line.Location.X);
        int end_dist = Math::Abs(_caret_updown_x - other_dest_line.Location.X - other_dest_line.Size.X);
        if (start_dist < end_dist)
            char_index--;
    }

    // For same reason as above, when lines end on same index as the next one starts on, the index found might move the caret one
    // line down.
    char_index = Math::Min(char_index, caret_pos);

    if (_snap_top_line)
    {
        // Find the line nearest to the scroll pos
        line_index = _text_painter->LineAtPos(this, GetMeasurements(), _scroll_pos.Y/* + 0.5f*/);
        const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];
        _scroll_pos.Y = line.Location.Y;
    }

    if (GetVerticalScrollBar() != nullptr)
        GetVerticalScrollBar()->SetScrollPos(_scroll_pos.Y);

    return char_index;
}

int FudgetTextBox::GetCaretPosPageDown()
{
    int caret_pos = GetCaretPos();
    if (_text_painter == nullptr || GetMeasurements().Lines.Count() == 0)
        return caret_pos;

    if (_caret_updown_x < 0)
        _caret_updown_x = _text_painter->GetCharacterPosition(this, GetMeasurements(), caret_pos).X;

    int line_index = _text_painter->GetCharacterLine(GetMeasurements(), caret_pos);
    if (line_index < 0 || line_index >= GetMeasurements().Lines.Count() - 1)
        return caret_pos;

    const FudgetLineMeasurements &line = GetMeasurements().Lines[line_index];

    FudgetPadding inner_padding = GetCombinedPadding();
    int height = (int)inner_padding.Padded(GetBounds()).GetHeight();
    int full_height = height;

    // TODO: make this an option?
    // Measure one line height less
    height = Math::Max(Math::Max(1, (int)(line.Size.Y * .5f)), height - line.Size.Y);
    Int2 test_pos = Int2(_caret_updown_x, Math::Max(0, line.Location.Y + Math::Max(1, (int)(line.Size.Y * .5f)) + height));


    int char_index = _text_painter->HitTest(this, GetMeasurements(), test_pos);

    // When placing line breaks after any character, the next line usually starts on the same character index where the pervious
    // one ended. In this case it's possible that instead of going up one page, this would move the caret to the end of two lines
    // above. To avoid that, make sure the index is changed to match the correct line.

    int char_line = _text_painter->GetCharacterLine(GetMeasurements(), char_index);
    const FudgetLineMeasurements &dest_line = GetMeasurements().Lines[char_line];

    if (char_index == dest_line.EndIndex && char_line < GetMeasurements().Lines.Count() - 1 && GetMeasurements().Lines[char_line + 1].StartIndex == char_index)
    {
        const FudgetLineMeasurements &other_dest_line = GetMeasurements().Lines[char_line + 1];

        int end_dist = Math::Abs(_caret_updown_x - dest_line.Location.X - dest_line.Size.X);
        int start_dist = Math::Abs(_caret_updown_x - other_dest_line.Location.X);
        if (start_dist < end_dist)
            char_index++;
    }
    else if (char_index == dest_line.StartIndex && char_line > line_index + 1 && GetMeasurements().Lines[char_line - 1].EndIndex == char_index)
    {
        const FudgetLineMeasurements &other_dest_line = GetMeasurements().Lines[char_line - 1];

        int start_dist = Math::Abs(_caret_updown_x - dest_line.Location.X);
        int end_dist = Math::Abs(_caret_updown_x - other_dest_line.Location.X - other_dest_line.Size.X);
        if (start_dist < end_dist)
            char_index--;
    }

    // For same reason as above, when lines end on same index as the next one starts on, the index found might move the caret one
    // line up. This can only happen on the very last line of the text.
    char_index = Math::Max(char_index, caret_pos);

    if (_snap_top_line)
    {
        // Find the nearest line on the top to snap to
        int scroll_line_index = _text_painter->LineAtPos(this, GetMeasurements(), _scroll_pos.Y + full_height - 1);
        const FudgetLineMeasurements &scroll_line = GetMeasurements().Lines[scroll_line_index];

        _scroll_pos.Y = scroll_line.Location.Y;

        FixScrollPos();
    }
    else
        _scroll_pos.Y = Math::Min(GetMeasurements().Size.Y - full_height, _scroll_pos.Y + height);

    if (GetVerticalScrollBar() != nullptr)
        GetVerticalScrollBar()->SetScrollPos(_scroll_pos.Y);

    return char_index;
}

void FudgetTextBox::SizeOrPosModified(FudgetLayoutDirtyReason dirt_flags)
{
    if ((dirt_flags & FudgetLayoutDirtyReason::Size) == FudgetLayoutDirtyReason::Size)
        _measure_space = -1;
    Base::SizeOrPosModified(dirt_flags);
}

FudgetTextBoxFlags FudgetTextBox::GetTextBoxInitFlags() const
{
    return FudgetTextBoxFlags::CaretVisible | FudgetTextBoxFlags::Editable | FudgetTextBoxFlags::WordSkip | FudgetTextBoxFlags::MouseSelectable | FudgetTextBoxFlags::KeySelectable;
}

FudgetPadding FudgetTextBox::GetCombinedPadding() const
{
    return _content_padding + GetFramePadding();
}

void FudgetTextBox::RequestScrollExtents()
{
    GetMeasurements();

    Rectangle bounds = GetCombinedPadding().Padded(GetBounds());
    bounds.Size += GetScrollBarWidths();
    FudgetScrollBarComponent *vbar = GetVerticalScrollBar();
    FudgetScrollBarComponent *hbar = GetHorizontalScrollBar();
    bool hvis = hbar != nullptr && GetMeasurements().Size.X > bounds.Size.X;
    bool vvis = vbar != nullptr && GetMeasurements().Size.Y > bounds.Size.Y;
    Int2 bounds_size = bounds.Size - GetScrollBarWidths(hvis, vvis);

    int expand = 0;
    if (_snap_top_line)
    {
        bool changed = true;
        while (changed)
        {
            int height = (int)bounds_size.Y;
            int sum = 0;
            int count = GetMeasurements().Lines.Count(); // _data->GetCount();

            int last_top_height = 0;

            int pos = count - 1;
            while (pos >= 0 && sum < height)
            {
                last_top_height = GetMeasurements().Lines[pos--].Size.Y;// GetItemSize(pos--).Y;
                sum += last_top_height;
            }

            if (sum > height)
            {
                if (sum <= last_top_height)
                    expand = sum - last_top_height;
                else
                    expand = last_top_height - (sum - height);
            }

            changed = (!hvis && hbar != nullptr && GetMeasurements().Size.X > bounds_size.X) || (!vvis && vbar != nullptr && GetMeasurements().Size.Y + expand > bounds_size.Y);
            if (changed)
            {
                hvis |= hbar != nullptr && GetMeasurements().Size.X > bounds_size.X;
                vvis |= vbar != nullptr && GetMeasurements().Size.Y + expand > bounds_size.Y;
                bounds_size = bounds.Size - GetScrollBarWidths(hvis, vvis);
            }
        }
    }


    if (vbar != nullptr)
    {
        vbar->SetScrollRange(GetMeasurements().Size.Y + expand);
        vbar->SetPageSize((int)bounds_size.Y);
        if (GetMeasurements().Lines.Count() > 0)
            vbar->SetLineSize(GetMeasurements().Lines[0].Size.Y);
        vbar->SetScrollPos(_scroll_pos.Y);
    }

    if (hbar != nullptr)
    {
        hbar->SetScrollRange(GetMeasurements().Size.X);
        hbar->SetPageSize((int)bounds_size.X);
        if (GetMeasurements().Lines.Count() > 0)
            hbar->SetLineSize(GetMeasurements().Lines[0].Size.Y);
        hbar->SetScrollPos(_scroll_pos.X);
    }
}
