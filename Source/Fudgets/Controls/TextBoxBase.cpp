#include "TextBoxBase.h"
#include "Engine/Core/Types/StringBuilder.h"
#include "Engine/Core/Types/StringView.h"

FudgetTextBoxBase::FudgetTextBoxBase(const SpawnParams &params) : Base(params), _caret_pos(0), _sel_pos(-1),
    _key_selecting(false), _mouse_selecting(false), _word_skip(false)
{

}

void FudgetTextBoxBase::SetCaretPos(int value)
{
    int newpos = Math::Min(GetTextLength(), Math::Max(0, value));
    if (newpos != _caret_pos)
    {
        int old_caret_pos = _caret_pos;
        _caret_pos = newpos;

        int old_sel_pos = _sel_pos;
        _sel_pos = -1;

        DoPosChanged(old_caret_pos, old_sel_pos);
    }
    else if (GetSelStart() != GetSelEnd())
    {
        SetSelection(_caret_pos, 0);
    }
}

void FudgetTextBoxBase::SetSelStart(int value)
{
    if (value < 0)
    {
        int old_sel_pos = _sel_pos;
        _sel_pos = -1;

        if (_sel_pos != old_sel_pos)
            DoPosChanged(_caret_pos, old_sel_pos);
    }
    else
        SetCaretPos(value);
}

void FudgetTextBoxBase::SetSelEnd(int value)
{
    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    if (_sel_pos < 0)
        _sel_pos = _caret_pos;

    _caret_pos = Math::Max(0, Math::Min(GetTextLength(), value));

    if (old_sel_pos != _sel_pos || old_caret_pos != _caret_pos)
        DoPosChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::SetSelLength(int value)
{
    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    if (_sel_pos < 0)
        _sel_pos = _caret_pos;
    _caret_pos = Math::Min(GetTextLength(), Math::Max(0, _sel_pos + value));

    if (old_sel_pos != _sel_pos || old_caret_pos != _caret_pos)
        DoPosChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::SetSelection(int selectionStart, int selectionLength)
{
    int new_pos = selectionStart + selectionLength;
    if (selectionStart < 0)
        selectionStart = 0;
    else if (selectionStart > GetTextLength())
        selectionStart = GetTextLength();
    if (new_pos < 0)
        new_pos = 0;
    else if (new_pos > GetTextLength())
        new_pos = GetTextLength();

    if (_sel_pos == selectionStart && _caret_pos == new_pos)
        return;

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _sel_pos = selectionStart;
    _caret_pos = new_pos;

    if (old_sel_pos != _sel_pos || old_caret_pos != _caret_pos)
        DoPosChanged(old_caret_pos, old_sel_pos);
}

FudgetInputResult FudgetTextBoxBase::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (button != MouseButton::Left)
        return FudgetInputResult::Consume;

    int index = CharIndexAt(pos);
    SetSelection(index, 0);

    _mouse_selecting = true;

    return FudgetInputResult::Consume;
}

bool FudgetTextBoxBase::OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button)
{
    if (button == MouseButton::Left)
        _mouse_selecting = false;
    return true;
}

void FudgetTextBoxBase::OnMouseMove(Float2 pos, Float2 global_pos)
{
    if (!_mouse_selecting)
        return;

    int index = CharIndexAt(pos);
    SetCaretPosInner(index);
}

bool FudgetTextBoxBase::WantsNavigationKey(KeyboardKeys key)
{
    return key == KeyboardKeys::ArrowLeft || key == KeyboardKeys::ArrowRight || key == KeyboardKeys::ArrowUp || key == KeyboardKeys::ArrowDown;
}

FudgetInputResult FudgetTextBoxBase::OnCharInput(Char ch)
{
    if (GetSelLength() == 0)
    {
        InsertCharacter(_caret_pos, ch);
        //SetText(GetText().Substring(0, _caret_pos) + ch + GetText().Substring(_caret_pos, GetTextLength() - _caret_pos));

        int old_sel_pos = _sel_pos;
        int old_caret_pos = _caret_pos;

        _sel_pos = -1;
        _caret_pos += 1;

        DoTextEdited(old_caret_pos, old_sel_pos);
    }
    else
    {
        ReplaceSelected(ch);
    }

    return FudgetInputResult::Consume;
}

FudgetInputResult FudgetTextBoxBase::OnKeyDown(KeyboardKeys key)
{
    FudgetInputResult result = FudgetInputResult::Consume;

    if (key == KeyboardKeys::Backspace)
    {
        if (GetSelLength() == 0)
        {
            if (_caret_pos > 0)
            {
                DeleteCharacters(_caret_pos - 1, _caret_pos);
                //Text = string.Concat(Text.AsSpan(0, _caret_pos - 1), Text.AsSpan(_caret_pos, GetTextLength() - _caret_pos));

                int old_sel_pos = _sel_pos;
                int old_caret_pos = _caret_pos;

                _sel_pos = -1;
                _caret_pos -= 1;

                DoTextEdited(old_caret_pos, old_sel_pos);
            }
        }
        else
        {
            DeleteSelected();
        }
    }
    else if (key == KeyboardKeys::Delete)
    {
        if (GetSelLength() == 0)
        {
            if (_caret_pos < GetTextLength())
            {
                DeleteCharacters(_caret_pos, _caret_pos + 1);
                //Text = string.Concat(Text.AsSpan(0, CaretPos), Text.AsSpan(CaretPos + 1, GetTextLength() - CaretPos - 1));

                DoTextEdited(_caret_pos, _sel_pos);
            }
        }
        else
        {
            DeleteSelected();
        }
    }
    else if (key == KeyboardKeys::ArrowLeft)
    {
        int change = 1;
        if (_word_skip)
        {
            while (_caret_pos - change >= 0 && IsWhitespace(_caret_pos - change))
                ++change;
            while (_caret_pos - change >= 1 && !IsWhitespace(_caret_pos - change - 1))
                ++change;
        }

        if (!_key_selecting && GetSelStart() != GetSelEnd())
            SetCaretPos(Math::Min(GetSelStart(), GetSelEnd()));
        else
            SetCaretPosInner(_caret_pos - change);
    }
    else if (key == KeyboardKeys::ArrowRight)
    {
        int change = 1;
        if (_word_skip)
        {
            while (_caret_pos + change < GetTextLength() && !IsWhitespace(_caret_pos + change))
                ++change;
            while (_caret_pos + change < GetTextLength() && IsWhitespace(_caret_pos + change))
                ++change;
        }
        if (!_key_selecting && GetSelStart() != GetSelEnd())
            SetCaretPos(Math::Max(GetSelStart(), GetSelEnd()));
        else
            SetCaretPosInner(_caret_pos + change);
    }
    else if (key == KeyboardKeys::Shift)
        _key_selecting = true;
    else if (key == KeyboardKeys::Control)
        _word_skip = true;
    else if (key == KeyboardKeys::Home)
        SetCaretPosInner(0);
    else if (key == KeyboardKeys::End)
        SetCaretPosInner(GetTextLength());

    return result;
}

bool FudgetTextBoxBase::OnKeyUp(KeyboardKeys key)
{
    if (key == KeyboardKeys::Shift)
        _key_selecting = false;
    else if (key == KeyboardKeys::Control)
        _word_skip = false;

    return true;
}

void FudgetTextBoxBase::DeleteSelected()
{
    if (GetSelLength() == 0)
        return;
    int sel_min = Math::Min(GetSelStart(), GetSelEnd());
    int sel_max = Math::Max(GetSelStart(), GetSelEnd());

    DeleteCharacters(sel_min, sel_max);
    //Text = string.Concat(Text.AsSpan(0, sel_min), Text.AsSpan(sel_max, Text.Length - sel_max));

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _caret_pos = sel_min;
    _sel_pos = -1;

    if (sel_min != sel_max)
        DoTextEdited(old_caret_pos, old_sel_pos);
    else if (old_caret_pos != _caret_pos || old_sel_pos != _sel_pos)
        DoPosChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::ReplaceSelected(const String &with)
{
    if (GetSelLength() == 0)
        return;
    int sel_min = Math::Min(GetSelStart(), GetSelEnd());
    int sel_max = Math::Max(GetSelStart(), GetSelEnd());

    ReplaceCharacters(sel_min, sel_max, with);
    //Text = string.Concat(Text.AsSpan(0, sel_min), with, Text.AsSpan(sel_max, Text.Length - sel_max));

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _caret_pos = sel_min + with.Length();
    _sel_pos = -1;

    if (sel_min != sel_max || with.Length() != 0)
        DoTextEdited(old_caret_pos, old_sel_pos);
    else if (old_caret_pos != _caret_pos || old_sel_pos != _sel_pos)
        DoPosChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::ReplaceSelected(char ch)
{
    if (GetSelLength() == 0)
        return;
    int sel_min = Math::Min(GetSelStart(), GetSelEnd());
    int sel_max = Math::Max(GetSelStart(), GetSelEnd());

    ReplaceCharacters(sel_min, sel_max, ch);
    //Text = Text.Substring(0, sel_min) + ch + Text.Substring(sel_max, Text.Length - sel_max);

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _caret_pos = sel_min + 1;
    _sel_pos = -1;

    DoTextEdited(old_caret_pos, old_sel_pos);
}

FudgetControlFlags FudgetTextBoxBase::GetInitFlags() const
{
    return FudgetControlFlags::CanHandleMouseMove | FudgetControlFlags::CanHandleMouseEnterLeave | FudgetControlFlags::CanHandleMouseUpDown |
        FudgetControlFlags::CaptureReleaseMouseLeft | FudgetControlFlags::FocusOnMouseLeft |
        FudgetControlFlags::CanHandleKeyEvents | FudgetControlFlags::CanHandleNavigationKeys | Base::GetInitFlags();
}

void FudgetTextBoxBase::SetCaretPosInner(int value)
{
    if (_key_selecting || _mouse_selecting)
        SetSelEnd(value);
    else
        SetSelection(value, 0);
}

