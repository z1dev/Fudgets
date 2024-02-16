#pragma once

#include "../Control.h"

/// <summary>
/// Passed to FudgetTextBoxBase::DoPositionChanged as a hint why the caret position might have changed
/// </summary>
API_ENUM()
enum class FudgetTextBoxCaretChangeReason
{
    /// <summary>
    /// No reason. Probably changed by the user with a function call to caret or selection change.
    /// </summary>
    Unknown,
    /// <summary>
    /// Position changed by mouse click
    /// </summary>
    Mouse,
    /// <summary>
    /// Position changed by moving caret to the left by user input
    /// </summary>
    Left,
    /// <summary>
    /// Position changed by moving caret to the right by user input
    /// </summary>
    Right,
    /// <summary>
    /// Position changed by moving caret up by user input
    /// </summary>
    Up,
    /// <summary>
    /// Position changed by moving caret down by user input
    /// </summary>
    Down,
    /// <summary>
    /// Position changed by moving caret to the home position by user input
    /// </summary>
    Home,
    /// <summary>
    /// Position changed by moving caret to the end position by user input
    /// </summary>
    End,
    /// <summary>
    /// Position changed by moving caret one page up by user input
    /// </summary>
    PageUp,
    /// <summary>
    /// Position changed by moving caret one page down by user input
    /// </summary>
    PageDown,
};

API_CLASS()
class FUDGETS_API FudgetTextBoxBase : public FudgetControl
{
    using Base = FudgetControl;
    DECLARE_SCRIPTING_TYPE(FudgetTextBoxBase);
public:
    /// <summary>
    /// Text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual String GetText() const { return TEXT(""); }
    /// <summary>
    /// The text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual int GetTextLength() const { return 0; }

    /// <summary>
    /// Change the text displayed in the text box.
    /// </summary>
    API_PROPERTY() void SetText(const String &value);

    /// <summary>
    /// Position in the text where new characters will be inserted or used as the starting point of deletion.
    /// If the text box has selection, the caret will be at the movable end of the selected area.
    /// Changing the caret position will deselect the selected text.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetCaretPos() const { return _caret_pos; }
    /// <summary>
    /// Position in the text where new characters will be inserted or used as the starting point of deletion.
    /// If the text box has selection, the caret will be at the movable end of the selected area.
    /// Changing the caret position will deselect the selected text.
    /// </summary>
    API_PROPERTY() void SetCaretPos(int value);

    /// <summary>
    /// Returns the true value of the selection position. If is negative or matches the caret position then nothing is selected.
    /// Otherwise it is the start of the selection. Use GetSelStart to always get the caret position if nothing is selected.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelPos() const { return _sel_pos; }
    /// <summary>
    /// The starting position or stationary end of the text selection. If there is no selection in the text box, this is the same as
    /// the current caret position. Setting this value to zero or positive number will move the caret and deselect the selected text.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelStart() const { return _sel_pos < 0 ? _caret_pos : _sel_pos; }
    /// <summary>
    /// The starting position or stationary end of the text selection. If there is no selection in the text box, this is the same as
    /// the current caret position. Setting this value to zero or positive number will move the caret and deselect the selected text.
    /// </summary>
    API_PROPERTY() void SetSelStart(int value);
    /// <summary>
    /// The end position of the text selection. This is always the caret position. Setting this value will change the caret position
    /// and update the selection to be between the text selection's starting position and the caret position.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelEnd() const { return _caret_pos; }
    /// <summary>
    /// The end position of the text selection. This is always the caret position. Setting this value will change the caret position
    /// and update the selection to be between the text selection's starting position and the caret position.
    /// </summary>
    API_PROPERTY() void SetSelEnd(int value);
    /// <summary>
    /// Number of characters between the text selection's starting position to the caret position. This value is negative
    /// if the caret position is in front of the selection's starting position. Changing this value will change the selection and
    /// move the caret relative to the text selection's starting position. Call SetSelection to change both the selection's start
    /// and the caret position at the same time.
    /// </summary>
    API_PROPERTY() FORCE_INLINE int GetSelLength() const { return _sel_pos < 0 ? 0 : _caret_pos - _sel_pos; }
    /// <summary>
    /// Number of characters between the text selection's starting position to the caret position. This value is negative
    /// if the caret position is in front of the selection's starting position. Changing this value will change the selection and
    /// move the caret relative to the text selection's starting position. Call SetSelection to change both the selection's start
    /// and the caret position at the same time.
    /// </summary>    
    API_PROPERTY() void SetSelLength(int value);

    /// <summary>
    /// Changes the current text selection in the text box and places the caret at the end of the selection. The selectionLength
    /// can be negative, to place the caret at the front of the selected text. If selectionStart is outside the text's range, it
    /// will be clamped, and selectionLength is updated to match it.
    /// </summary>
    /// <param name="selectionStart">The starting position or stationary end of the selection</param>
    /// <param name="selectionLength">The number of characters to select or the distance of the end of the selection from selectionStart</param>
    API_FUNCTION() void SetSelection(int selectionStart, int selectionLength);

    /// <summary>
    /// Called when the caret or selection position changes. If the change was due to an edit action that changed the
    /// text as well, DoTextEdited is called instead. Override OnPositionChanged instead or call the base function
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void OnPositionChanged(int old_caret_pos, int old_sel_pos) {}

    /// <summary>
    /// Called when the text changed because of an edit action, like typing a character or pasting text.
    /// Override OnTextEdited instead or call the base function
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void OnTextEdited(int old_caret_pos, int old_sel_pos) {}

    /// <summary>
    /// Called when the caret or selection position changes. If the change was due to an edit action that changed the
    /// text as well, DoTextEdited is called instead. Override OnPositionChanged instead or call the base function
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void DoPositionChanged(int old_caret_pos, int old_sel_pos);

    /// <summary>
    /// Called when the text changed because of an edit action, like typing a character or pasting text.
    /// Override OnTextEdited instead or call the base function
    /// </summary>
    /// <param name="old_caret_pos">Caret position before the change</param>
    /// <param name="old_sel_pos">Selection position before the change</param>
    API_FUNCTION() virtual void DoTextEdited(int old_caret_pos, int old_sel_pos);

    /// <summary>
    /// Finds the character index at the control's local position
    /// </summary>
    /// <param name="pos">Local position in the control</param>
    /// <returns>Index of character in the text</returns>
    API_FUNCTION() virtual int CharIndexAt(Float2 pos) { return 0; };

    /// <inheritdoc />
    FudgetInputResult OnMouseDown(Float2 pos, Float2 global_pos, MouseButton button, bool double_click) override;

    /// <inheritdoc />
    bool OnMouseUp(Float2 pos, Float2 global_pos, MouseButton button) override;

    /// <inheritdoc />
    void OnMouseMove(Float2 pos, Float2 global_pos) override;

    /// <inheritdoc />
    bool WantsNavigationKey(KeyboardKeys key) override;

    /// <inheritdoc />
    FudgetInputResult OnCharInput(Char ch) override;

    /// <inheritdoc />
    FudgetInputResult OnKeyDown(KeyboardKeys key) override;

    /// <inheritdoc />
    bool OnKeyUp(KeyboardKeys key) override;

    /// <summary>
    /// Deletes the selected part of the edited text
    /// </summary>
    API_FUNCTION() void DeleteSelected();

    /// <summary>
    /// Replaces the selected part of the edited text with a string. If no text is selected, inserts the text at
    /// the caret's position
    /// </summary>
    /// <param name="with">The replacement string</param>
    API_FUNCTION() void ReplaceSelected(const String &with);

    /// <summary>
    /// Replaces the selected part of the edited text with a character. If no text is selected, inserts the character at
    /// the caret's position
    /// </summary>
    /// <param name="ch">The replacement character</param>
    API_FUNCTION() void ReplaceSelected(Char ch);

    /// <summary>
    /// Causes the action followed by the backspace key. Usually deletes the selection or the character before the caret.
    /// </summary>
    API_FUNCTION() virtual void BackspacePressed();
    /// <summary>
    /// Causes the action followed by the delete key. Usually deletes the selection or the character after the caret.
    /// </summary>
    API_FUNCTION() virtual void DeletePressed();
    /// <summary>
    /// Moves the caret to the left, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretLeft();
    /// <summary>
    /// Moves the caret to the right, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretRight();
    /// <summary>
    /// Moves the caret up, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretUp();
    /// <summary>
    /// Moves the caret down, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretDown();
    /// <summary>
    /// Moves the caret to the front, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretHome();
    /// <summary>
    /// Moves the caret to the end, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretEnd();
    /// <summary>
    /// Moves the caret to page up, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretPageUp();
    /// <summary>
    /// Moves the caret to page down, as if its key was pressed. Should update the selection to extend to
    /// the new caret position.
    /// </summary>
    API_FUNCTION() virtual void CaretPageDown();

    API_PROPERTY() bool WordSkipping() const { return _word_skip; }
    API_PROPERTY() bool KeySelecting() const { return _key_selecting; }
    API_PROPERTY() bool MouseSelecting() const { return _mouse_selecting; }
protected:
    /// <inheritdoc />
    FudgetControlFlags GetInitFlags() const override;

    /// <summary>
    /// Change the text displayed in the text box.
    /// </summary>
    API_PROPERTY() virtual void SetTextInternal(const String &value) {}

    /// <summary>
    /// Checks if a character in the text is a whitespace character.
    /// </summary>
    /// <param name="index">Character index</param>
    /// <returns>Whether character at index is whitespace or not</returns>
    API_FUNCTION() virtual bool IsWhitespace(int index) const { return false; }

    /// <summary>
    /// Inserts a single character into the editor's text.
    /// </summary>
    /// <param name="index">Character index</param>
    /// <param name="ch">The character to add</param>
    API_FUNCTION() virtual void InsertCharacter(int index, Char ch) {}

    /// <summary>
    /// Removes characters from the text starting at an index, and before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to remove</param>
    /// <param name="end_index">Index after last character to remove</param>
    API_FUNCTION() virtual void DeleteCharacters(int start_index, int end_index) {}

    /// <summary>
    /// Replaces characters in the text starting at an index before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to replace</param>
    /// <param name="end_index">Index after last character to replace</param>
    /// <param name="with">Replacement string</param>
    API_FUNCTION() virtual void ReplaceCharacters(int start_index, int end_index, const String &with) {}

    /// <summary>
    /// Replaces characters in the text starting at an index before the ending index.
    /// </summary>
    /// <param name="start_index">Index of first character to replace</param>
    /// <param name="end_index">Index after last character to replace</param>
    /// <param name="ch">Replacement character</param>
    API_FUNCTION() virtual void ReplaceCharacters(int start_index, int end_index, Char ch) {}

    /// <summary>
    /// Moves the caret to the specified position like SetCaretPos. If keyboard/mouse selection is active, the
    /// text between the selection start and the new caret position will be selected. Use this to implement
    /// caret movement with selection.
    /// </summary>
    /// <param name="value">The new caret position</param>
    API_FUNCTION() void SetCaretPosInner(int value);

    /// <summary>
    /// Caret position when moving to the left, considering word skipping as well.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosLeft();
    /// <summary>
    /// Caret position when moving to the right, considering word skipping as well.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosRight();
    /// <summary>
    /// Caret position when moving up.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosUp();
    /// <summary>
    /// Caret position when moving down.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosDown();
    /// <summary>
    /// Caret position when moving "home".
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosHome();
    /// <summary>
    /// Caret position when moving to the "end".
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosEnd();
    /// <summary>
    /// Caret position when moving one page up.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosPageUp();
    /// <summary>
    /// Caret position when moving one page down.
    /// </summary>
    API_FUNCTION() virtual int GetCaretPosPageDown();

    API_FIELD() FudgetTextBoxCaretChangeReason CaretChangeReason;
private:

    int _caret_pos;
    int _sel_pos;

    bool _key_selecting;
    bool _mouse_selecting;

    // Ctrl key is pressed, move between words when going left and right
    bool _word_skip;
};


