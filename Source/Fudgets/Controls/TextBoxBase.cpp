#include "TextBoxBase.h"
#include "Engine/Core/Types/StringBuilder.h"
#include "Engine/Core/Types/StringView.h"

FudgetTextBoxBase::FudgetTextBoxBase(const SpawnParams &params) : Base(params),
    CaretChangeReason(FudgetTextBoxCaretChangeReason::Unknown), _caret_pos(0), _sel_pos(-1),
    _key_selecting(false), _mouse_selecting(false), _word_skip(false)
{

}

void FudgetTextBoxBase::SetText(const String &value)
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Unknown;
    SetTextInternal(value);
}

void FudgetTextBoxBase::SetCaretPos(int value)
{
    int newpos = Math::Min(GetTextLength(), Math::Max(0, value));
    if (newpos != _caret_pos || GetSelStart() != GetSelEnd())
    {
        int old_sel_pos = _sel_pos;
        int old_caret_pos = _caret_pos;
        _caret_pos = newpos;
        _sel_pos = -1;

        if (old_sel_pos != _sel_pos || old_caret_pos != _caret_pos)
            DoPositionChanged(old_caret_pos, old_sel_pos);
    }
}

void FudgetTextBoxBase::SetSelStart(int value)
{
    if (value < 0)
    {
        int old_sel_pos = _sel_pos;
        _sel_pos = -1;

        if (_sel_pos != old_sel_pos)
            DoPositionChanged(_caret_pos, old_sel_pos);
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
        DoPositionChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::SetSelLength(int value)
{
    if (value == 0)
    {
        SetCaretPos(value);
        return;
    }

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    if (_sel_pos < 0)
        _sel_pos = _caret_pos;
    _caret_pos = Math::Min(GetTextLength(), Math::Max(0, _sel_pos + value));

    if (old_sel_pos != _sel_pos || old_caret_pos != _caret_pos)
        DoPositionChanged(old_caret_pos, old_sel_pos);
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
        DoPositionChanged(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::DoPositionChanged(int old_caret_pos, int old_sel_pos)
{
    OnPositionChanged(old_caret_pos, old_sel_pos);
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Unknown;
}

void FudgetTextBoxBase::DoTextEdited(int old_caret_pos, int old_sel_pos)
{
    OnTextEdited(old_caret_pos, old_sel_pos);
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Unknown;
}

FudgetInputResult FudgetTextBoxBase::OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click)
{
    if (button != MouseButton::Left)
        return FudgetInputResult::Consume;

    int index = CharIndexAt(pos);
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Mouse;
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
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Mouse;
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
        BackspacePressed();
    }
    else if (key == KeyboardKeys::Delete)
    {
        DeletePressed();
    }
    else if (key == KeyboardKeys::ArrowLeft)
    {
        CaretLeft();
    }
    else if (key == KeyboardKeys::ArrowRight)
    {
        CaretRight();
    }
    else if (key == KeyboardKeys::ArrowUp)
    {
        CaretUp();
    }
    else if (key == KeyboardKeys::ArrowDown)
    {
        CaretDown();
    }
    else if (key == KeyboardKeys::PageUp)
    {
        CaretPageUp();
    }
    else if (key == KeyboardKeys::PageDown)
    {
        CaretPageDown();
    }
    else if (key == KeyboardKeys::Home)
        CaretHome();
    else if (key == KeyboardKeys::End)
        CaretEnd();
    else if (key == KeyboardKeys::Shift)
        _key_selecting = true;
    else if (key == KeyboardKeys::Control)
        _word_skip = true;
    else if (key == KeyboardKeys::Return)
        OnCharInput(L'\n');

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

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _caret_pos = sel_min;
    _sel_pos = -1;

    DoTextEdited(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::ReplaceSelected(const String &with)
{
    if (GetSelLength() == 0 && with.Length() == 0)
        return;

    int sel_min = Math::Min(GetSelStart(), GetSelEnd());
    int sel_max = Math::Max(GetSelStart(), GetSelEnd());

    ReplaceCharacters(sel_min, sel_max, with);

    int old_sel_pos = _sel_pos;
    int old_caret_pos = _caret_pos;

    _caret_pos = sel_min + with.Length();
    _sel_pos = -1;

    DoTextEdited(old_caret_pos, old_sel_pos);
}

void FudgetTextBoxBase::ReplaceSelected(Char ch)
{
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

void FudgetTextBoxBase::BackspacePressed()
{
    if (GetSelLength() == 0)
    {
        if (_caret_pos > 0)
        {
            int new_caret_pos = GetCaretPosLeft();
            DeleteCharacters(new_caret_pos, _caret_pos);

            int old_sel_pos = _sel_pos;
            int old_caret_pos = _caret_pos;

            _sel_pos = -1;
            _caret_pos = new_caret_pos;

            DoTextEdited(old_caret_pos, old_sel_pos);
        }
    }
    else
    {
        DeleteSelected();
    }
}

void FudgetTextBoxBase::DeletePressed()
{
    if (GetSelLength() == 0)
    {
        if (_caret_pos < GetTextLength())
        {
            DeleteCharacters(_caret_pos, GetCaretPosRight());
            DoTextEdited(_caret_pos, _sel_pos);
        }
    }
    else
    {
        DeleteSelected();
    }
}

void FudgetTextBoxBase::CaretLeft()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Left;

    if (!_key_selecting && GetSelStart() != GetSelEnd())
    {
        SetCaretPos(Math::Min(GetSelStart(), GetSelEnd()));
        return;
    }

    SetCaretPosInner(GetCaretPosLeft());
}

void FudgetTextBoxBase::CaretRight()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Right;

    if (!_key_selecting && GetSelStart() != GetSelEnd())
    {
        SetCaretPos(Math::Max(GetSelStart(), GetSelEnd()));
        return;
    }

    SetCaretPosInner(GetCaretPosRight());
}

void FudgetTextBoxBase::CaretUp()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Up;

    if (!_key_selecting && GetSelStart() != GetSelEnd())
    {
        SetCaretPos(Math::Min(GetSelStart(), GetSelEnd()));
        return;
    }

    SetCaretPosInner(GetCaretPosUp());
}

void FudgetTextBoxBase::CaretDown()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Down;

    if (!_key_selecting && GetSelStart() != GetSelEnd())
    {
        SetCaretPos(Math::Max(GetSelStart(), GetSelEnd()));
        return;
    }

    SetCaretPosInner(GetCaretPosDown());
}

void FudgetTextBoxBase::CaretHome()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::Home;

    SetCaretPosInner(GetCaretPosHome());
}

void FudgetTextBoxBase::CaretEnd()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::End;
    SetCaretPosInner(GetCaretPosEnd());
}

void FudgetTextBoxBase::CaretPageUp()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::PageUp;
    SetCaretPosInner(GetCaretPosPageUp());
}

void FudgetTextBoxBase::CaretPageDown()
{
    CaretChangeReason = FudgetTextBoxCaretChangeReason::PageDown;
    SetCaretPosInner(GetCaretPosPageDown());
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

int FudgetTextBoxBase::GetCaretPosLeft()
{
    int change = 1;
    if (_word_skip)
    {
        while (_caret_pos - change >= 0 && IsWhitespace(_caret_pos - change))
            ++change;
        while (_caret_pos - change >= 1 && !IsWhitespace(_caret_pos - change - 1))
            ++change;
    }

    return _caret_pos - change;
}

int FudgetTextBoxBase::GetCaretPosRight()
{
    int change = 1;
    if (_word_skip)
    {
        while (_caret_pos + change < GetTextLength() && !IsWhitespace(_caret_pos + change))
            ++change;
        while (_caret_pos + change < GetTextLength() && IsWhitespace(_caret_pos + change))
            ++change;
    }
    return _caret_pos + change;
}

int FudgetTextBoxBase::GetCaretPosUp()
{
    return Math::Min(GetSelStart(), GetCaretPos());
}

int FudgetTextBoxBase::GetCaretPosDown()
{
    return Math::Max(GetSelStart(), GetCaretPos());
}

int FudgetTextBoxBase::GetCaretPosHome()
{
    return 0;
}

int FudgetTextBoxBase::GetCaretPosEnd()
{
    return GetTextLength();
}

int FudgetTextBoxBase::GetCaretPosPageUp()
{
    return 0;
}

int FudgetTextBoxBase::GetCaretPosPageDown()
{
    return GetTextLength();
}
